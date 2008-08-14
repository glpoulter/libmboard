/* $Id$ */
/*!
 * \file parallel/comm.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : July 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief State Transition Routines for processing communication nodes
 * 
 * \image html CommNode.png
 * \image latex CommNode.eps
 * 
 * \todo Revert to sending whole board if total tagged messages (for all
 * procs) > number of messages. We can prevent full data replication by
 * applying the message filters on the recipient node.
 * 
 * \todo Replace blocking MPI_Alltoall and MPI_Allgather with non-blocking
 * sends/receives. This would introduce more communication states, but 
 * avoid unnecessary synchronisation of processors.
 */
#include "mb_parallel.h"
#include "mb_commqueue.h"
#include "mb_pooled_list.h"
#include "mb_tag_table.h"
#include <string.h>
#include <stdio.h>

/* use macros for bit operations */
/*! \brief Binary octet with only LSB set to 1 */
#define UNITY    (0x01)
/*! \brief Binary octet with only MSB set to 1 */
#define MSBMASK  (0x80)
/*! \brief Binary octet with all bits set to 0 */
#define ALLZEROS (0x00)
/*! \brief set all bits to 0 */
#define SETZEROS(octet) octet = ALLZEROS
/*! \brief set bit at position 'pos' to 1 (indexing from MSB to LSB)*/
#define SETBIT(octet, pos) octet = ((octet) | (UNITY << (7 - pos)))
/*! \brief query if most significant bit is set */
#define MSB_IS_SET(octet) (((octet) & MSBMASK) == MSBMASK)


/*! 
 * \brief Intialises tagging of a message board
 * \param[in] node address of CommQueue node
 * \return Return Code
 * 
 * This routine should only be called by processPendingComms() within
 * the communication thread when processing a node that is in the
 * ::PRE_TAGGING stage.
 * 
 * It should not be called when the target board has no
 * registered functions associated with it, and only when run in parallel 
 * (with more than one MPI task).
 * 
 * If the target board has no function parameters assigned (\c node->fparams 
 * is \c NULL), the node is moved into the ::TAGGING stage and the routine
 * returns successfully.
 * 
 * If the function paramemters are set, <tt>MPI_Allgather</tt> is used to distribute
 * the size of parameters on each board. This is necessary as we now allow
 * for parameters of different sizes across all MPI tasks.
 * Do note that at this point, all boards MUST have their parameters 
 * defined with \c board->fparams_size \c > \c 0.
 * 
 * Once sizes are known, boards will send its parameters to other boards, and 
 * in turn receive parameters of all other boards. Non-blocking sends and 
 * receives are issued to propagate the parameters.
 * 
 * The user defined \c fparams pointer is used as the MPI output buffer to
 * avoid unnecessary duplication of memory. Therefore, users must be reminded
 * that their parameters should never be modified or deallocated while 
 * synchronisation is in progress.
 * 
 * The node is then moved to stage ::TAGINFO_SENT and the routine returns.
 * 
 * Side effects:
 * - if <tt>node->fparams != NULL</tt>
 *  - <tt>node->outbuf</tt> allocated with memory
 *  - <tt>node->outbuf[0]</tt> allocated with memory
 *  - <tt>node->outbuf[0]</tt> contains same data as \c node->fparams
 *  - <tt>node->sendreq</tt> allocated with memory
 *  - <tt>node->sendreq[*]</tt> contains valid MPI_Request values, apart 
 *    from <tt>node->sendreq[::MBI_CommRank]</tt> which should be set to
 *    \c MPI_REQUEST_NULL.
 *  - <tt>node->recvreq</tt> allocated with memory
 *  - <tt>node->recvreq[*]</tt> contains valid MPI_Request values, apart 
 *    from <tt>node->sendreq[::MBI_CommRank]</tt> which should be set to
 *    \c MPI_REQUEST_NULL.
 *  - <tt>node->inbuf</tt> allocated with memory
 *  - <tt>node->inbuf[*]</tt> allocated with memory, apart from 
 *    <tt>node->inbuf[::MBI_CommRank]</tt> which should be set to \c NULL
 *  - <tt>node->stage</tt> set to ::TAGINFO_SENT
 * - if <tt>node->fparams == NULL</tt>
 *  - <tt>node->outbuf</tt> remains \c NULL
 *  - <tt>node->inbuf</tt> remains \c NULL
 *  - <tt>node->sendreq</tt> remains \c NULL
 *  - <tt>node->recvreq</tt> remains \c NULL
 *  - <tt>node->stage</tt> set to ::TAGGING
 * Notes:
 *  - It might be possible to reduce memory usage by not copying param
 *    data to <tt>node->outbuf[0]</tt>. Instead, we pass address of 
 *    \c node->fparams to Issend(). However, since fparams holds a user 
 *    supplied pointer, there is a danger that the data might inadvertently 
 *    be modified while the transmission is in progress, leading to 
 *    hard-to-debug errors.
 *  
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_MEMALLOC (Could not allocate required memory)
 *  - ::MB_ERR_MPI (MPI routine call failed)
 * 
 * 
 */
int MBIt_Comm_InitTagging(struct MBIt_commqueue *node) {
    
    int i, rc, tag;
    int param_size;
    MBIt_Board *board;
    
    /* check that all is well before we proceed */
    assert(node != NULL);
    assert(node->stage == PRE_TAGGING);
    assert(node->incount == NULL);
    assert(MBI_CommSize != 1);
    
    /* get reference to mboard */
    board = (MBIt_Board*)MBI_getMBoardRef(node->mb);
    assert(board != NULL);
    assert(board->tagging == MB_TRUE);
    assert(board->fh != MB_NULL_FUNCTION);
    assert(NULL != MBI_getFunctionRef(board->fh));
    
    /* check if tagging requires function parameters */
    if (board->fparams == NULL)
    {
        /* skip parameter propagation */
        node->stage = TAGGING;
        return MB_SUCCESS;
    }
    
    /* if fparams != NULL, then size can't be 0 */
    assert(board->fparams_size != 0);
    
    /* ------------ propagate fparam size --------------- */

    /* prep array to receive remote param sizes 
     * - reuse incount var within "node"
     */
    node->incount = (int *)malloc(sizeof(int) * MBI_CommSize);
    assert(node->incount != NULL);
    if (node->incount == NULL) return MB_ERR_MEMALLOC;

    
    /* get local param size */
    param_size = (int)board->fparams_size;
    
    /* NOTE: this is an unoptimised version
     * - for simplicity, we use MPI_Allgather to propagate param sizes.
     *   We would ideally want to use non-blocking sends/recvs to begin
     *   the propagation, then complete the communication at a later stage.
     * - This is blocking, and will synchronise all mpi tasks.
     */
    rc = MPI_Allgather(&param_size, 1, MPI_INT, 
                       node->incount, 1, MPI_INT, MBI_CommWorld);
    assert(rc == MPI_SUCCESS);
    if (rc != MPI_SUCCESS) return MB_ERR_MPI;
    
    /* we don't need to send anything to ourself */
    node->incount[MBI_CommRank] = 0;
    
    /* quick check to make sure we're expecting something */

    
    /* ----------- initiate non-blocking sends ---------- */
    
    /* define MPI tag to use for communication */
    assert(node->mb <= MBI_TAG_BASE);
    tag = MBI_TAG_FHDATA | node->mb;
    assert(tag < MBI_TAG_MAX);
    
    /* NOTE: we don't prep an output buffer. Instead, we use the param pointer
     *       provided by the user
     * - This saves having to do a potentially huge memory copy.
     * - Make sure users are aware that they should not change the param
     *   pointer or the data it points to mid-sync!!!
     */
    node->outbuf = NULL;
    
    /* quick check to ensure function param pointer was set */
    assert(board->fparams != NULL);
    assert(param_size != 0);
    
    /* allocate memory for send requests */
    node->sendreq = (MPI_Request *)malloc(sizeof(MPI_Request) * MBI_CommSize);
    assert(node->sendreq != NULL);
    if (node->sendreq == NULL) return MB_ERR_MEMALLOC;
    
    /* issue sends to all other MPI tasks */
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank)
        {
            /* don't send to self */
            node->sendreq[i] = MPI_REQUEST_NULL;
            continue;
        }
        
        /* issue non-blocking send */
        rc = MPI_Issend(board->fparams, param_size, MPI_BYTE, i, tag,
                        MBI_CommWorld, &(node->sendreq[i]));
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
    }

    
    /* ----------- initiate non-blocking receives ---------- */
      
    

    

    /* allocate memory for input buffer */
    node->inbuf = (void **)malloc(sizeof(void*) * MBI_CommSize);
    assert(node->inbuf != NULL);
    if (node->inbuf == NULL) return MB_ERR_MEMALLOC;
    
    /* allocate memory for recv request array */
    node->recvreq = (MPI_Request *)malloc(sizeof(MPI_Request) * MBI_CommSize);
    assert(node->recvreq != NULL);
    if (node->recvreq == NULL) return MB_ERR_MEMALLOC;
    
    /* issue non-blocking receives from appropriate MPI tasks */
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (node->incount[i] == 0)
        {
            node->inbuf[i]   = NULL;
            node->recvreq[i] = MPI_REQUEST_NULL;
            continue;
        }
        
        /* we should never have to send to ourself */
        assert(i != MBI_CommRank);
        
        /* allocate memory for incoming message */
        /* note: at this point, incount[] stores sizes in bytes */
        node->inbuf[i] = malloc((size_t)node->incount[i]);
        assert(node->inbuf[i] != NULL);
        if (node->inbuf[i] == NULL) return MB_ERR_MEMALLOC;
        
        rc = MPI_Irecv(node->inbuf[i], node->incount[i], MPI_BYTE, i, tag,
                       MBI_CommWorld, &(node->recvreq[i]));
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
    }

    /* ----------------- move comm progress to next stage -------------- */
    node->stage = TAGINFO_SENT;
    
    return MB_SUCCESS;
}

/*! 
 * \brief Wait for propagation of function parameters to complete
 * \param[in] node address of CommQueue node
 * \return Return Code
 *
 * This routine should only be called by processPendingComms() within
 * the communication thread when processing a node that is in the
 * ::TAGINFO_SENT stage.
 * 
 * It should only be called when run in parallel (with more than one MPI task).
 * 
 * If \c node->sendreq or \c node->recvreq are not \c NULL, we assume
 * that there are pending sends/receives to complete.
 * 
 * Use <tt>MPI_Testall()</tt> to check if communication has completed.
 * If either sends or receives are completed, \c node->*req is freed
 * and set to \c NULL.
 * 
 * When all sends and receives are completed, the node is moved into
 * the ::TAGGING stage.
 * 
 * Side effects:
 * - if <tt>node->sendreq != NULL</tt> and sends are completed
 *  - \c node->sendreq is freed and set to \c NULL
 *  - <tt>node->outbuf[0]</tt> is freed
 *  - \c node->outbuf is freed
 * - if <tt>node->recvreq != NULL</tt> and receives are completed
 *  - \c node->recvreq is freed and set to \c NULL
 * - if both \c node->sendreq and \c node->recvreq are \c NULL
 *  - \c node->stage is set to ::TAGGING
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_MPI (MPI routine call failed)
 */ 
int MBIt_Comm_WaitTagInfo(struct MBIt_commqueue *node) {
    
    int rc, completed;
    
    /* check that all is well before we proceed */
    assert(node != NULL);
    assert(node->stage == TAGINFO_SENT);
    assert(MBI_CommSize != 1);
    
    /* handle sends */
    if (node->sendreq != NULL)
    {
        rc = MPI_Testall(MBI_CommSize, node->sendreq, &completed, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        
        if (completed)
        {
            free(node->sendreq);
            node->sendreq = NULL;
        }
    }
    
    /* handle receives */
    if (node->recvreq != NULL)
    {
        rc = MPI_Testall(MBI_CommSize, node->recvreq, &completed, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        
        if (completed) 
        {
            free(node->recvreq);
            node->recvreq = NULL;
        }
    }
    
    /* if sends and recvs are completed, tag messages */
    if (node->sendreq == NULL && node->recvreq == NULL)
    {
        /* move on to next stage */
        node->stage = TAGGING;

    }
    
    return MB_SUCCESS;
}

/*! 
 * \brief Allocate and populate tag table within message board
 * \param[in] node address of CommQueue node
 * \return Return Code
 *
 * This routine should only be called by processPendingComms() within
 * the communication thread when processing a node that is in the
 * ::TAGGING stage.
 * 
 * It should only be called when run in parallel (with more than one MPI task).
 * 
 * If there are messages in the board, a ::tag_table object is created 
 * and assigned to the \c board. It is then populated
 * by looping thru the message board (using an iterator), and for each 
 * message, applying the filter function from all remote boards.
 * 
 * \c node->inbuf is then freed and the node is moved into the
 * ::PRE_PROPAGATION stage.
 * 
 * Side effects:
 * - If \c board is not empty, ::tag_table within board is initialised and populated
 * - \c node->inbuf is freed and set to \c NULL
 * - \c node->stage is set to ::PRE_PROPAGATION
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_MEMALLOC (could not allocate required memory)
 *  - ::MB_ERR_INTERNAL (calls to internal routines failed. Run in debug mode for details)
 */ 
int MBIt_Comm_TagMessages(struct MBIt_commqueue *node) {
    
    char window;
    void *params;
    void *msg;
    int i, c, w, p;
    int rc, mcount;
    MBIt_Board *board;
    MBIt_filterfunc func;
    pl_address_node *pl_itr;
    
    /* check that all is well before we proceed */
    assert(node != NULL);
    assert(node->stage == TAGGING);
    assert(MBI_CommSize != 1);
    
    /* get reference to mboard */
    board = (MBIt_Board*)MBI_getMBoardRef(node->mb);
    assert(board != NULL);
    assert(board->tagging == MB_TRUE);
    assert(board->fh != MB_NULL_FUNCTION);
    assert(NULL != MBI_getFunctionRef(board->fh));
    assert(board->data != NULL);
    assert(board->tt == NULL);
    if (board == NULL) return MB_ERR_INTERNAL;
    
    /* -------------- init TagTable object ----------------- */
    
    /* how many messages do we have in the board? */
    mcount = (int)board->data->count_current;
    
    /* create tagtable object */
    if (mcount > 0)
    {
        rc = tt_create(&(board->tt), mcount, MBI_CommSize);
        assert(rc == TT_SUCCESS);
        assert(board->tt != NULL);
        if (rc != TT_SUCCESS || board->tt == NULL) return MB_ERR_INTERNAL;
    }
    
    /* --------------- start tagging messages ------------------ */
    
    /* get ptr to associated filter function */
    /* NOTE: We're assuming that all MPI Tasks have this board assigned with
     *       the same Function Handle
     * - This is checked during MB_Function_Assign() when run in debug mode
     * - Do remind users!
     */
    func = ((MBIt_filterfunc_wrapper*)MBI_getFunctionRef(board->fh))->func;
    assert(func != NULL);
    if (func == NULL) return MB_ERR_INTERNAL;
    
    /* loop thru messages */
    i = 0;
    for (pl_itr = PL_ITERATOR(board->data); pl_itr; pl_itr = pl_itr->next)
    {
        /* get reference to message object */
        msg = PL_NODEDATA(pl_itr);
        assert(msg != NULL);
        if (msg == NULL) return MB_ERR_INTERNAL;
        
        /* c : offset within byte buffer (window)
         * w : window offset within table row
         */
        c = w = 0;
        SETZEROS(window);
        
        /* run filter on message per MPI task */
        for (p = 0; p < MBI_CommSize; p++)
        {
            if (p != MBI_CommRank)
            {   
                /* assign params pointer for this MPI task */
                params = (board->fparams == NULL)? NULL : node->inbuf[p];
                
                /* if message accepted by filter */
                if (1 == (*func)(msg, params))
                {
                    /* set bit within our byte buffer */
                    SETBIT(window, c);
                }
            }
            
            /* move index within window */
            c += 1;
            
            /* when window full, write to table and shift window */
            if (c == 8)
            {
                /* write byte buffer to table */
                rc = tt_setbyte(board->tt, i, w, window);
                assert(rc == TT_SUCCESS);
                
                /* move window */
                w += 1;
                
                /* reset byte buffer */
                SETZEROS(window);
                c = 0;
            }
        }
        
        /* write remaining byte buffer */
        if (w < (int)board->tt->row_size)
        {
            rc = tt_setbyte(board->tt, i, w, window);
            assert(rc == TT_SUCCESS);
        }
        
        /* before we go on to next message */
        i++;
        assert(i <= mcount);
    }
    
    /* free inbuf */
    if (node->inbuf)
    {
        for (i = 0; i < MBI_CommSize; i++)
        {
            if (node->inbuf[i]) free(node->inbuf[i]);
        }
        free(node->inbuf);
        node->inbuf = NULL;
    }
    
    /* move on to next stage */
    node->stage = PRE_PROPAGATION;
    
    return MB_SUCCESS;
}

/*! 
 * \brief Start propagation of messages
 * \param[in] node address of CommQueue node
 * \return Return Code
 *
 * This routine should only be called by processPendingComms() within
 * the communication thread when processing a node that is in the
 * ::PRE_PROPAGATION stage.
 * 
 * It should only be called when run in parallel (with more than one MPI task).
 * 
 * At this point, all buffers (\c node->inbuf, \c node->outbuf) and request 
 * tables (\c node->sendreq, \c node->recvreq) should be have been dealloacted
 * and set to \c NULL.
 * 
 * If \c node->incount was not previously allocated (it would have been if 
 * the \c node went through the ::PRE_TAGGING stage), allocate it as an
 * \c int array of size ::MBI_CommSize. \c outcount is also allocated as an 
 * \c int array of the same size.
 * 
 * We populate and distribute the \c outcount array to inform
 * remote nodes of the number of messages to expect. Count information
 * from remote nodes are received into \c node->incount.
 * If the board's ::tag_table is not present, all messages in the local board has
 * to be sent to all remote nodes, and we end up with full data replication.
 * 
 * \c MPI_Alltoall is used to distribute and receive the count arrays. This
 * will effective acts as a barrier that synchronises all procs. To avoid 
 * this, we could possibly split this routine further into two stages and 
 * use \c MPI_Issend and \c MPI_Irecv.
 * 
 * Initiating receives: by looking at \c node->incount, we know how many 
 * messages to expect from each remote board. If not \c 0, 
 * <tt>node->inbuf[?]</tt> is allocated and MPI_Irecv issued. If \c 0,
 * <tt>node->inbuf[?]</tt> is set to NULL and <tt>node->recvreq[?]</tt>
 * set to \c MPI_REQUEST_NULL.
 * 
 * Initiating sends (no tagging): All remote boards are to be sent the 
 * same data - everything in the local board. Only <tt>node->outbuf[0]</tt>
 * need to be allocate and populated. \c MPI_Issend() is used to send
 * <tt>node->outbuf[0]</tt> to all remote boards.
 * 
 * Initiating sends (with tagging): by looking at \c outcount, we know 
 * how many we need to send to each remote board. 
 * <tt>node->outbuf[?]</tt> is allocated based on this information 
 * (set to \c NULL if no messages need to be sent). Messages in the local
 * board are then copied to the appropriate <tt>node->outbuf[?]</tt> 
 * depending on the tagging. \c MPI_Issend() is used to send 
 * <tt>node->outbuf[?]</tt> to all remote boards that are expecting messages. 
 * 
 * If available, the board's ::tag_table is deleted.
 * 
 * 
 * Side effects:
 * - Tag table (\c board->tt) freed and set to \c NULL
 * - \c node->incount allocated with memory
 * - \c node->sendreq allocated with memory 
 * - \c node->recvreq allocated with memory 
 * - \c node->inbuf allocated with memory 
 * - <tt>node->inbuf[i]</tt> allocate with memory for \c i where
 *   <tt>node->incount[i] != 0</tt>. Otherwise, <tt>node->inbuf[i]</tt>
 *   set to \c NULL.
 * - <tt>node->recvreq[i]</tt> set with valid \c MPI_Request for \c i where
 *   <tt>node->incount[i] != 0</tt>. Otherwise, <tt>node->recvreq[i]</tt>
 *   set to \c MPI_REQUEST_NULL.
 * - \c node->outbuf allocated with memory 
 * - if <tt>board->tagging == MB_TRUE</tt>
 *  - if each proc \c i that has messages tagged
 *   - <tt>node->outbuf[i]</tt> allocated with memory
 *   - <tt>node->sendreq[i]</tt> set with valid MPI_Request
 *  - else
 *   - <tt>node->outbuf[i]</tt> set to \c NULL
 *   - <tt>node->sendreq[i]</tt> set t \c MPI_REQUEST_NULL
 * - if <tt>board->tagging == MB_FALSE</tt>
 *  - <tt>node->outbuf[*]</tt> set to \c NULL, except for 
 *    <tt>node->outbuf[0]</tt> which is allocated with memory
 *  - <tt>node->outbuf[0]</tt> has copy of all messages in local board
 *  - <tt>node->sendreq[*]</tt> set with valid \c MPI_Request, except for
 *    <tt>node->sendreq[::MBI+CommRank]</tt> which is set to 
 *    \c MPI_REQUEST_NULL.
 * - \c node->stage set to ::PROPAGATION
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_MEMALLOC (could not allocate required memory)
 *  - ::MB_ERR_MPI (MPI routine call failed)
 *  - ::MB_ERR_INTERNAL (calls to internal routines failed. Run in debug mode for details)
 */ 
int MBIt_Comm_InitPropagation(struct MBIt_commqueue *node) {
  
    int rc;
    int i, w, b, p, m;
    int tag, msgsize;
    int *bufindex = NULL;
    int *outcount;
    void *msg;
    char *row, window;
    pl_address_node *pl_itr;
    MBIt_Board *board;
    
    assert(node != NULL);
    assert(node->inbuf == NULL);
    assert(node->outbuf == NULL);
    assert(node->sendreq == NULL);
    assert(node->recvreq == NULL);
    assert(node->stage == PRE_PROPAGATION);
    assert(MBI_CommSize != 1);
    
    /* get reference to board */
    board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    assert(board != NULL);
    if(board == NULL) return MB_ERR_INTERNAL;
    
    /* prep memory for storing incount */
    if (node->incount == NULL) /* if not created in previous stages */
    {
        node->incount = (int*)malloc(sizeof(int) * MBI_CommSize);
        assert(node->incount != NULL);
        if (node->incount == NULL) return MB_ERR_MEMALLOC;
    }
    
    /* prepare memory for storing outcount */
    outcount = (int*)malloc(sizeof(int) * MBI_CommSize);
    assert(outcount != NULL);
    if (outcount == NULL) return MB_ERR_MEMALLOC;
    
    /* define MPI tag to use for communication */
    assert(node->mb <= MBI_TAG_BASE);
    tag = MBI_TAG_MSGDATA | node->mb;
    assert(tag < MBI_TAG_MAX);
    
    /* -------- determine number of messages to send to each MPI task ---- */
    
    if (board->tagging == MB_FALSE || board->data->count_current == 0)
    { /* messages not tagged or if board empty */
        for (i = 0; i < MBI_CommSize; i++)
        {
            if (i == MBI_CommRank) 
            {
                outcount[i] = 0;
                continue;
            }
            outcount[i] = (int)board->data->count_current;
        }
    }
    else
    {
        assert(board->tt != NULL);
        
        /* allocate memory for buffer index */
        bufindex = (int *)malloc(sizeof(int) * MBI_CommSize);
        assert(bufindex != NULL);
        if (bufindex == NULL) return MB_ERR_MEMALLOC;
        
        for (i = 0; i < MBI_CommSize; i++)
        {
            /* init bufindex value */
            bufindex[i] = 0;
            
            if (i == MBI_CommRank) 
            {
                outcount[i] = 0;
                continue;
            }
            
            rc = tt_getcount_col(board->tt, i, &(outcount[i]));
            assert(rc == TT_SUCCESS);
        }
    }
    
    /* ----- determine number of messages to expect from each MPI Task --- */
    
    /* NOTE: For simplicity, we use MPI_Alltoall. 
     * - This is blocking and collective, so it synchronises all MPI Tasks
     * - To optimise this, use non-blocking sends and complete the comm later
     */
    assert(outcount != NULL);
    assert(node->incount != NULL);
    rc = MPI_Alltoall(outcount, 1, MPI_INT, 
                      node->incount, 1, MPI_INT, MBI_CommWorld);
    assert(rc == MPI_SUCCESS);
    if (rc != MPI_SUCCESS) return MB_ERR_MPI;
    
    
    /* ---- Issue non-blocking receives ------------------------ */
    
    /* allocate memory for requests */
    node->recvreq = (MPI_Request *)malloc(sizeof(MPI_Request) * MBI_CommSize);
    assert(node->recvreq != NULL);
    if (node->recvreq == NULL) return MB_ERR_MEMALLOC;
    
    /* allocate array for input buffers  */
    node->inbuf = (void **)malloc(sizeof(void *) * MBI_CommSize);
    assert(node->inbuf != NULL);
    if (node->inbuf == NULL) return MB_ERR_MEMALLOC;
    
    /* determine size of message */
    msgsize = (int)board->data->elem_size;
    
    /* issue receives from appropriate MPI Tasks */
    node->pending_in = 0;
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (node->incount[i] == 0)
        {
            node->inbuf[i] = NULL;
            node->recvreq[i] = MPI_REQUEST_NULL;
            continue;
        }
        
        /* quick check. We should not need to send any to self */
        assert(i != MBI_CommRank);
        
        /* allocate memory for input buffer */
        node->inbuf[i] = malloc((size_t)(node->incount[i] * msgsize));
        assert(node->inbuf[i] != NULL);
        if (node->inbuf[i] == NULL) return MB_ERR_MEMALLOC;
        
        /* issue non-blocking receive */
        rc = MPI_Irecv(node->inbuf[i], node->incount[i] * msgsize, MPI_BYTE,
                       i, tag, MBI_CommWorld, &(node->recvreq[i]));
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        
        node->pending_in ++;
    }
    
    
    /* ------ issue non-blocking sends ---------------------------- */
    
    /* allocate memory for requests */
    node->sendreq = (MPI_Request *)malloc(sizeof(MPI_Request) * MBI_CommSize);
    assert(node->sendreq != NULL);
    if (node->sendreq == NULL) return MB_ERR_MEMALLOC;
    
    /* allocate array for output buffers  */
    node->outbuf = (void **)malloc(sizeof(void *) * MBI_CommSize);
    assert(node->outbuf != NULL);
    if (node->outbuf == NULL) return MB_ERR_MEMALLOC;
    
    node->pending_out = 0;
    
    if (board->data->count_current > 0)
    {
        /* message board not tagged */
        if (board->tagging == MB_FALSE)
        {             
            /* quick check */
            assert(board->tt == NULL);
            
            /* we only need to allocate one output buffer since we're sending
             * the same data to all MPI tasks */

            node->outbuf[0] = malloc((size_t)(board->data->count_current * msgsize));
            assert(node->outbuf[0] != NULL);
            if (node->outbuf[0] == NULL) return MB_ERR_MEMALLOC;

            /* copy messages into buffer */
            i = 0;
            for (pl_itr = PL_ITERATOR(board->data); pl_itr; pl_itr = pl_itr->next)
            {
                /* get reference to message object */
                msg = PL_NODEDATA(pl_itr);
                assert(msg != NULL);
                
                memcpy((char*)node->outbuf[0] + (i * msgsize), msg, (size_t)msgsize);
                i++;
            }
            
            for (i = 0; i < MBI_CommSize; i++)
            {
                /* don't need other outbuf since we're sharing */
                if (i != 0) node->outbuf[i] = NULL;
                
                /* don't send to self */
                if (i == MBI_CommRank)
                {
                    node->sendreq[i] = MPI_REQUEST_NULL;
                    continue;
                }
                
                /* issue send */
                rc = MPI_Issend(node->outbuf[0], (int)board->data->count_current * msgsize,
                                MPI_BYTE, i, tag, MBI_CommWorld, &(node->sendreq[i]));
                assert(rc == MPI_SUCCESS);
                if (rc != MPI_SUCCESS) return MB_ERR_MPI;
                
                node->pending_out ++;
            }
        }
        else
        {
            /* quick check */
            assert(board->tt != NULL);
            
            /* prep output buffers */
            for (i = 0; i < MBI_CommSize; i++)
            {
                if (outcount[i] == 0)
                {
                    node->outbuf[i]  = NULL;
                    node->sendreq[i] = MPI_REQUEST_NULL;
                    continue;
                }
                
                /* we should never need to send to self */
                assert(i != MBI_CommRank);
                
                /* allocate output buffers */

                node->outbuf[i] = malloc((size_t)(outcount[i] * msgsize));

                assert(node->outbuf[i] != NULL);
                if (node->outbuf[i] == NULL) return MB_ERR_MEMALLOC;
            }
            
            /* copy tagged messages */
            i = 0;
            for (pl_itr = PL_ITERATOR(board->data); pl_itr; pl_itr = pl_itr->next)
            {
                /* get reference to message object */
                msg = PL_NODEDATA(pl_itr);
                assert(msg != NULL);
                
                /* get ptr to row in tag table */
                rc = tt_getrow(board->tt, i, &row);
                assert(rc == TT_SUCCESS);
                assert(row != NULL);
                if (rc != TT_SUCCESS || row == NULL) return MB_ERR_INTERNAL;
                
                /* w: window index within row (in units of bytes)
                 * b: bit index within window (in units of bits)
                 * p: process (mpi task) represented by w&b
                 * m: offset within output buffer (in units of bytes)
                 */
                for (w = 0; w < (int)board->tt->row_size; w++)
                {
                    window = *(row + w);
                    
                    b = 0;
                    while (window != ALLZEROS)
                    {
                        if (MSB_IS_SET(window))
                        {
                            /* determine which MPI task this refers to */
                            p = (w * 8) + b;
                            assert(p >= 0);
                            assert(p < MBI_CommSize);
                            assert(p != MBI_CommRank);
                            
                            /* calculate offser within output buffer */
                            m = bufindex[p] * msgsize;
                            
                            /* copy message to appropriate output buffer */
                            memcpy((void *)((char*)(node->outbuf[p]) + m), 
                                   msg, (size_t)msgsize);
                            
                            /* move buffer index */
                            bufindex[p]++;
                        }
                        
                        /* shift bit and repeat */
                        window = window << 1;
                        b ++;
                    }
                }
                
                /* on to next message */
                i++;
                assert(i <= (int)board->data->count_current);
            }
            
            /* delete tag table */
            rc = tt_delete(&(board->tt));
            assert(rc == TT_SUCCESS);
            assert(board->tt == NULL);
            if (rc != TT_SUCCESS || board->tt != NULL) return MB_ERR_INTERNAL;
            
            #ifdef _EXTRA_CHECKS
            for (i = 0; i < MBI_CommSize; i++)
            {
                assert (outcount[i] == bufindex[i]);
            }
            #endif
            
            /* free buf index array */
            free(bufindex);
            
            for (i = 0; i < MBI_CommSize; i++)
            {
                
                if (outcount[i] == 0) continue;
                
                /* issue send */
                rc = MPI_Issend(node->outbuf[i], outcount[i] * msgsize, MPI_BYTE,
                                i, tag, MBI_CommWorld, &(node->sendreq[i]));
                assert(rc == MPI_SUCCESS);
                if (rc != MPI_SUCCESS) return MB_ERR_MPI;
                
                node->pending_out ++;
            }

        }
        
    }
    
    /* free memory */
    
    free(outcount);
    
    if (node->pending_in == 0)
    {
        if (node->inbuf != NULL) free(node->inbuf);
        node->inbuf = NULL;
        if (node->incount != NULL) free(node->incount);
        node->incount = NULL;
        if (node->recvreq != NULL) free(node->recvreq);
        node->recvreq = NULL;
    }
    
    if (node->pending_out == 0)
    {
        if (node->outbuf != NULL) free(node->outbuf);
        node->outbuf = NULL;
        if (node->sendreq != NULL) free(node->sendreq);
        node->sendreq = NULL;
    }
    
    /* more comm to next stage */
    node->stage = PROPAGATION;
    return MB_SUCCESS;
}

/*! 
 * \brief Complete the message propagation process
 * \param[in] node address of CommQueue node
 * \return Return Code
 *
 * This routine should only be called by processPendingComms() within
 * the communication thread when processing a node that is in the
 * ::PROPAGATION stage.
 * 
 * It should only be called when run in parallel (with more than one MPI task).
 * 
 * If there are pending sends, use \c MPI_Testsome to check
 * if any sends have completed. For each completed send, free associated memory
 * and decrement \c node->pending_out.
 * If \c node->pending_out reduces to \c 0, free the rest of
 * the associated memory.
 * 
 * If there are pending receives, use \c MPI_Testsome to 
 * determine which ones have completed. For each completed 
 * receive, copy received messages into local board, 
 * free associated memory, and decrement \c node->pending_in.
 * If \c node->pending_in reduces to \c 0, free the rest of
 * the associated memory.
 * 
 * Finally, if there are no more pending sends and receives, pop
 * \c node from CommQueue and unlock the board. Also send a signal
 * on the pthread condition variable associated to wake up the main 
 * thread if it is waiting for the sync completion of this board. 
 * 
 * Side effects:
 * - for each completed send/receive
 *  - associated buffers are freed and counters decremented.
 * - if all sends and receives have completed
 *  - \c node is removed from CommQueue and allocated memory is freed
 *  - \c board->locked set to ::MB_FALSE
 *  - \c board->syncCompleted set to ::MB_TRUE
 *  - Local board contains all relevant messages from remote boards
 *  - Signal was sent on pthread condition variable associated to board
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_MEMALLOC (could not allocate required memory)
 *  - ::MB_ERR_MPI (MPI routine call failed)
 *  - ::MB_ERR_INTERNAL (calls to internal routines failed. Run in debug mode for details)
 * 
 */ 
int MBIt_Comm_CompletePropagation(struct MBIt_commqueue *node) {
    
    int rc;
    int i, p, m;
    int completed;
    void *mloc, *ptr_new;
    MBIt_Board *board = NULL;
    
    /* sanity check */
    assert(node != NULL);
    assert(node->stage == PROPAGATION);
    assert(MBI_comm_indices != NULL);
    assert(MBI_CommSize != 1);
    
    /* process sends */
    if (node->pending_out > 0)
    {
        if (board == NULL)
        {
            /* get reference to board */
            board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
            assert(board != NULL);
            if (board == NULL) return MB_ERR_INTERNAL;
        }
        
        rc = MPI_Testsome(MBI_CommSize, node->sendreq, &completed,
                MBI_comm_indices, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        assert(completed < MBI_CommSize);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        
        if (completed > 0)
        {
            
            if (board->tagging == MB_TRUE)
            {
                for (i = 0; i < completed; i++)
                {
                    p = MBI_comm_indices[i];
                    assert(p >= 0);
                    assert(p < MBI_CommSize);
                    
                    if (node->outbuf[p] != NULL) free(node->outbuf[p]);
                    node->outbuf[p] = NULL;
                    node->pending_out --;
                }
            }
            else
            {
                node->pending_out -= completed;
                if (node->pending_out == 0)
                {
                    if (node->outbuf[0] != NULL) free(node->outbuf[0]);
                }
            }
            
            /* clear up memory when all sends have completed */
            if (node->pending_out == 0)
            {
                if (node->outbuf != NULL) free(node->outbuf);
                if (node->sendreq != NULL) free(node->sendreq);
            }
        }
    }
    
    /* process recvs */
    if (node->pending_in > 0)
    {
        
        rc = MPI_Testsome(MBI_CommSize, node->recvreq, &completed,
                MBI_comm_indices, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        assert(completed < MBI_CommSize);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        
        if (completed > 0)
        {
            if (board == NULL)
            {
                /* get reference to board */
                board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
                assert(board != NULL);
                if (board == NULL) return MB_ERR_INTERNAL;
            }
            
            for (i = 0; i < completed; i++)
            {
                p = MBI_comm_indices[i];
                assert(p >= 0);
                assert(p < MBI_CommSize);
                
                for(m = 0; m < node->incount[p]; m++)
                {
                    /* copy message into local board */
                    rc = pl_newnode(board->data, &ptr_new);
                    assert(rc == PL_SUCCESS);
                    mloc = (void*)((char*)node->inbuf[p] + (m * board->data->elem_size));
                    memcpy(ptr_new, mloc, board->data->elem_size);
                }
                
                if (node->inbuf[p] != NULL) free(node->inbuf[p]);
                node->pending_in --;
                assert(node->pending_in >= 0);
            }
            
            /* clear up memory when all sends have completed */
            if (node->pending_in == 0)
            {
                if (node->incount != NULL) free(node->incount);
                if (node->inbuf != NULL) free(node->inbuf);
                if (node->recvreq != NULL) free(node->recvreq);
            }
        }
        
    }
    
    /* if sends and recvs are completed, finalise sync process */
    if ((node->pending_in == 0 && node->pending_out == 0))
    {
        
        if (board == NULL)
        {
            /* get reference to board */
            board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
            assert(board != NULL);
            if (board == NULL) return MB_ERR_INTERNAL;
        }
        
        /* remove node from queue */
        rc = MBI_CommQueue_Pop(node->mb);
        assert(rc == MB_SUCCESS);
        
        /* unlock board */
        rc = pthread_mutex_lock(&(board->syncLock));
        assert(0 == rc);
        
        board->syncCompleted = MB_TRUE;
        
        rc = pthread_mutex_unlock(&(board->syncLock));
        assert(0 == rc);
        
        /* send signal to wake main thread waiting on this board */
        rc = pthread_cond_signal(&(board->syncCond));
        assert(0 == rc);
        
        /* inform the calling routine that the Comm queue has been modified */
        return MB_SUCCESS_2;
    }
    
    return MB_SUCCESS;
}
