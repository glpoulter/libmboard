/* $Id$ */
/*!
 * \file parallel/comm.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Routines used to manage communication
 * 
 * \image html CommNode.png
 * \image latex CommNode.eps
 */
#include "mb_parallel.h"
#include "mb_commqueue.h"
#include "mb_pooled_list.h"
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
/*! \brief get address of bits representing a row in the tag table */
#define GET_TAG_ROW_FOR_MSG(table, msg, bytes) ((char*)table + (msg*bytes))
/*! \brief query if most significant bit is set */
#define MSB_IS_SET(octet) ((octet & MSBMASK) == MSBMASK)
/*! \brief deallocate tag table memory */
#define FREE_TAG_TABLE(ptr) if (ptr)                    \
{                                                       \
    if (ptr->tt) free(ptr->tt);                         \
    if (ptr->tagged) free(ptr->tagged);                 \
    free(ptr); ptr = NULL;                              \
}

/* if extra checks required (debug mode) */
#ifdef _EXTRA_CHECKS
static int check_equal_fh_and_paramsize(struct MBIt_commqueue *node);

struct funcdata_t {
    MBt_Function fh;
    size_t param_size;
};
#endif /*_EXTRA_CHECKS*/

/* #define DEBUG_TAG_TABLE */
#ifdef DEBUG_TAG_TABLE
static void print_tagtable_info(void *tt, int msgcount);
#endif /*DEBUG_TAG_TABLE*/
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
 * registered functions associated with it.
 * 
 * If the target board has no function parameters assigned (\c node->fparams 
 * is \c NULL), the node is move into the ::TAGGING stage and the routine
 * returns successfully.
 * 
 * If the function paramemters are set, boards will send its parameters
 * to other boards, and in turn receive parameters of all other boards.
 * Non-blocking sends and receives are issued to propagate the parameters.
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
 * 
 * 
 */
int MBIt_Comm_InitTagging(struct MBIt_commqueue *node) {
    
    int i, rc, tag;
    size_t psize;
    MBIt_Board *board;
    
    /* get reference to message board */
    assert(node != NULL);
    assert(node->stage == PRE_TAGGING);
    board = (MBIt_Board*)MBI_getMBoardRef(node->mb);
    assert(board->tagging == MB_TRUE);
    
    /* make sure function handle is valid */
    assert(board->fh != MB_NULL_FUNCTION);
    assert(NULL != MBI_getFunctionRef(board->fh));
    
    /* define MPI tag to use for communication */
    tag = MBI_TAG_FHDATA + ((int)node->mb % MBI_TAG_BASE);
    
    
    /* if compiled in debug mode */
#ifdef _EXTRA_CHECKS
    /* check that all boards have same fh and fparam_size */
    rc = check_equal_fh_and_paramsize(node);
    assert(rc == MB_SUCCESS);
#endif /*_EXTRA_CHECKS*/
    
    /* if fh doesn't need params, go straight to tagging */
    if (board->fparams == NULL)
    {
        /* move comm progress to next stage */
        node->stage = TAGGING;
        return MB_SUCCESS;
    }
    
    /* get parameter size */
    psize = board->fparams_size;
    assert(psize > 0);
    
    
    /* ---------------- SEND TAGINFO ---------------------------- */
    
    /* prepare output buffer (only need one since we're sending
     * the same data to all procs) 
     * Yes, using a pointer array here seems odd. Perhaps we 
     * shouldn't reuse outbuf for storing this buffer? 
     */
    node->outbuf = (void **)malloc(sizeof(void*));
    assert(node->outbuf != NULL);
    if (node->outbuf == NULL) return MB_ERR_MEMALLOC;
    
    node->outbuf[0] = (void *)malloc(psize);
    assert(node->outbuf[0] != NULL);
    if (node->outbuf[0] == NULL) return MB_ERR_MEMALLOC;
    
    /* Allocate memory for send requests */
    node->sendreq = (MPI_Request *)malloc(sizeof(MPI_Request) * MBI_CommSize);
    assert(node->sendreq != NULL);
    if (node->sendreq == NULL) return MB_ERR_MEMALLOC;
    
    /* copy fparams into outbuf */
    memcpy(node->outbuf[0], board->fparams, psize);

    /* initiate non-blocking sends */
    for(i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank) 
        {
            node->sendreq[i] = MPI_REQUEST_NULL;
            continue;
        }
        
        rc = MPI_Issend(node->outbuf[0], (int)psize, MPI_BYTE, i, tag, 
                   MBI_CommWorld, &(node->sendreq[i]));
        assert(rc == MPI_SUCCESS);
    }
    
    /* ---------------- RECV TAGINFO ---------------------------- */
    
    /* prep input buffer */
    node->inbuf = (void **)malloc(sizeof(void*) * MBI_CommSize);
    assert(node->inbuf != NULL);
    if (node->inbuf == NULL) return MB_ERR_MEMALLOC;
    
    /* Allocate memory for send requests */
    node->recvreq = (MPI_Request *)malloc(sizeof(MPI_Request) * MBI_CommSize);
    assert(node->recvreq != NULL);
    if (node->recvreq == NULL) return MB_ERR_MEMALLOC;
    
    /* initiate non-blocking recv */
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank)
        {
            node->inbuf[i]   = NULL;
            node->recvreq[i] = MPI_REQUEST_NULL;
            
            continue;
        }
        
        node->inbuf[i] = malloc(psize);
        assert(node->inbuf[i] != NULL);
        if (node->inbuf[i] == NULL) return MB_ERR_MEMALLOC;
        
        rc = MPI_Irecv(node->inbuf[i], (int)psize, MPI_BYTE, i, tag,
                MBI_CommWorld, &(node->recvreq[i]));
        assert(rc == MPI_SUCCESS);
    }
    
    /* move comm progress to next stage */
    node->stage = TAGINFO_SENT;
    
    return MB_SUCCESS;
}

/*! 
 * \brief Wait for propagation of function parameters to complete
 * \param[in] node address of CommQueue node
 * \return Return Code
 *
 * If \c node->sendreq or \c node->recvreq are not \c NULL, we assume
 * that there are pending sends/receives to complete.
 * 
 * Use <tt>MPI_Testall()</tt> to check if communication has completed.
 * 
 * When both sends and receives are completed, call immediately tag
 * messages by calling MBIt_Comm_TagMessages().
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
 */ 
int MBIt_Comm_WaitTagInfo(struct MBIt_commqueue *node) {
    
    int rc, completed;
    
    /* sanity check */
    assert(node != NULL);
    assert(node->stage == TAGINFO_SENT);
    
    /* handle sends */
    if (node->sendreq != NULL)
    {
        rc = MPI_Testall(MBI_CommSize, node->sendreq, &completed, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        
        if (completed)
        {
            free(node->sendreq);
            free(node->outbuf[0]);
            free(node->outbuf);
            node->outbuf  = NULL;
            node->sendreq = NULL;
        }
    }
    
    /* handle receives */
    if (node->recvreq != NULL)
    {
        rc = MPI_Testall(MBI_CommSize, node->recvreq, &completed, MPI_STATUSES_IGNORE);
        
        if (completed) 
        {
            free(node->recvreq);
            node->recvreq = NULL;
        }
    }
    
    /* if sends and recvs are completed, tag messages */
    if (node->sendreq == NULL && node->recvreq == NULL)
    {
        /* this routine will update node to PRE_PROPAGATION stage */
        node->stage = TAGGING;
        /* automatically chaining routines is just a tad more efficient,
         * but makes things so much harder to test and debug. Disabled.
         */
        /*return MBIt_Comm_TagMessages(node);*/
    }
    
    return MB_SUCCESS;
}

/*! 
 * \brief Allocate and populate tag table within message board
 * \param[in] node address of CommQueue node
 * \return Return Code
 *
 * Tag table within messag board is allocated. It is then populated
 * by looping thru the message board (using an iterator), and for each 
 * message, applying the filter function from all remote boards.
 * 
 * \c node->inbuf is then freed and propagation is initialised by
 * calling MBIt_Comm_TagMessages().
 * 
 * Side effects:
 * - Tag table within board is initialised and populated
 * - \c node->inbuf is freed
 * - \c node->stage is set to ::PRE_PROPAGATION
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_MEMALLOC (could not allocate required memory)
 */ 
int MBIt_Comm_TagMessages(struct MBIt_commqueue *node) {
    
    int i, p, c, w;
    char window;
    int mcount, bytes_required;
    MBIt_Board *board;
    MBIt_TagTable *tbl;
    pl_address_node *pl_itr;
    void *msg, *params, *tag_row;
    MBIt_filterfunc func;
    
    /* get reference to board object */
    assert(node != NULL);
    assert(node->stage == TAGGING);
    board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    assert(board->data != NULL);
    assert(board->tt == NULL);
    
    /* get num of messages */
    mcount = (int)board->data->count_current;

    /* allocate memory for TagTable */
    tbl = (MBIt_TagTable *)malloc(sizeof(MBIt_TagTable));
    assert(tbl != NULL);
    if (tbl == NULL) return MB_ERR_MEMALLOC;
    
    /* attach TagTable to board */
    board->tt = tbl;
    
    /* init Tagtable */
    tbl->pcount = MBI_CommSize;
    tbl->mcount = mcount;
    
    /* allocate tbl->tt */
    bytes_required = (int)(MBI_CommSize / 8) + 1;
    tbl->tt = malloc((size_t)(mcount * bytes_required));
    assert(tbl->tt != NULL);
    if (tbl->tt == NULL) return MB_ERR_MEMALLOC;
    
    /* allocate tbl->tagged */
    tbl->tagged = (int*)malloc(sizeof(int) * MBI_CommSize);
    assert(tbl->tagged != NULL);
    if (tbl->tagged == NULL) return MB_ERR_MEMALLOC;
    for (i = 0; i < MBI_CommSize; i++) tbl->tagged[i] = 0;
    
    /* ------------ start tagging messages ----------------- */
    
    params = NULL;
    func = ((MBIt_filterfunc_wrapper*)MBI_getFunctionRef(board->fh))->func;
    i = 0;

    for (pl_itr = PL_ITERATOR(board->data); pl_itr; pl_itr = pl_itr->next)
    {
        /* get reference to message object */
        msg = PL_NODEDATA(pl_itr);
        assert(msg != NULL);
        
        /* get refernce to row in TagTable */
        tag_row = (void*)GET_TAG_ROW_FOR_MSG(tbl->tt, i, bytes_required);
  
        /* w : window position within table
         * c : offset within window
         */
        c = w = 0;
        SETZEROS(window);
        assert(window == 0x0);
        
        /* for each processor */
        for (p = 0; p < MBI_CommSize; p++)
        {
            if (p != MBI_CommRank)
            {
                /* get reference to params */
                params = (board->fparams == NULL)? NULL : node->inbuf[p];

                /* if message accepted by filter */
                if (1 == (*func)(msg, params))
                {
                    SETBIT(window, c);
                    tbl->tagged[p]++;
                }
            }
            
            /* move window index */
            c++;
            
            if (c == 8) /* window full */
            {
                /* write buffer */
                memcpy((char*)tag_row + w, &window, 1);
                
                /* move window */
                w++;
                
                /* reset buffer */
                SETZEROS(window);
                c = 0;
            }
            
        }
        
        /* write remaining buffer */
        if (w < bytes_required) memcpy((char*)tag_row + w, &window, 1);
        
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
    
    node->stage = PRE_PROPAGATION;
    
    /* automatically chaining routines is just a tad more efficient,
     * but makes things so much harder to test and debug. Disabled.
     */
    /* return MBIt_Comm_InitPropagation(node); */
    return MB_SUCCESS;
}

/*! 
 * \brief Start propagation of messages
 * \param[in] node address of CommQueue node
 * \return Return Code
 *
 * We first populate and distribute the \c outcount array to inform
 * remote nodes of the number of messages to expect. Count information
 * from remote nodes are also received into \c node->incount.
 * If tag table is not present, all messages in the local board has
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
 * need to be allocate and populated. MPI_Issend() is used to send
 * <tt>node->outbuf[0]</tt> to all remote boards.
 * 
 * Initiating sends (with tagging): by looking at \c outcount, we know 
 * how many we need to send to each remote board. 
 * <tt>node->outbuf[?]</tt> is allocated based on this information 
 * (set to \c NULL if no messages need to be sent). Messages in the local
 * board are then copied to the
 * appropriate <tt>node->outbuf[?]</tt> depending on the tagging.
 * MPI_Issend() is used to send <tt>node->outbuf[?]</tt> to all 
 * remote boards that are expecting messages. 
 * 
 * The tag table is deleted.
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
 */ 
int MBIt_Comm_InitPropagation(struct MBIt_commqueue *node) {
    
    int i, p, w, m, b;
    int rc, tag, bytes_required;
    int *outcount;
    int msgsize;
    void *msg, *tag_row;
    pl_address_node *pl_itr;
    MBIt_Board *board;
    char window;

    
    assert(node != NULL);
    assert(node->inbuf == NULL);
    assert(node->outbuf == NULL);
    assert(node->sendreq == NULL);
    assert(node->recvreq == NULL);
    assert(node->stage == PRE_PROPAGATION);
    
    /* get reference to board */
    board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    assert(board != NULL);
    
    /* get size of single message in board */
    msgsize = (int)board->data->elem_size;
    
    /* prep outcount/incount array */
    outcount = (int *)malloc(sizeof(int) * MBI_CommSize);
    assert(outcount != NULL);
    if (outcount == NULL) return MB_ERR_MEMALLOC;
    
    node->incount = (int *)malloc(sizeof(int) * MBI_CommSize);
    assert(node->incount != NULL);
    if (node->incount == NULL) return MB_ERR_MEMALLOC;
    
    /* determined number of messages to send to each proc */

    if (board->tagging == MB_FALSE) 
    {   /* if messages not tagged */
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
        assert(board->tt->tt != NULL);
        assert(board->tt->tagged != NULL);
        for (i = 0; i < MBI_CommSize; i++) 
        {
            outcount[i] = board->tt->tagged[i];
        }
    }
       
    /* TODO: Change this to use MPI_Issend + probes so we don't 
     *       synchronised the procs?
     */
    rc = MPI_Alltoall(outcount, 1, MPI_INT, node->incount, 1, MPI_INT, MBI_CommWorld);
    assert(rc == MPI_SUCCESS);

    /* define MPI tag to use for communication */
    tag = MBI_TAG_MSGDATA + ((int)node->mb % MBI_TAG_BASE);

    
    /* ---------- issue receives -------------------------- */
    /* allocate request table memory */
    node->recvreq = (MPI_Request *)malloc(sizeof(MPI_Request) * MBI_CommSize);
    assert(node->recvreq != NULL);
    if (node->recvreq == NULL) return MB_ERR_MEMALLOC;
    /* allocate output buffer table */
    node->inbuf = (void **)malloc(sizeof(void *) * MBI_CommSize);
    assert(node->inbuf != NULL);
    if (node->inbuf == NULL) return MB_ERR_MEMALLOC;

    node->pending_in = 0; 
    
    /* issue receives from each remote proc */
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (node->incount[i] == 0)
        {
            node->inbuf[i]  = NULL;
            node->recvreq[i] = MPI_REQUEST_NULL;
            continue;
        }
        
        /* allocate memory for input buffer for proc i */
        node->inbuf[i] = malloc((size_t)(node->incount[i] * msgsize));
        assert(node->inbuf[i] != NULL);
        if (node->inbuf[i] == NULL) return MB_ERR_MEMALLOC;
        
        rc = MPI_Irecv(node->inbuf[i], node->incount[i] * msgsize,
                MPI_BYTE, i, tag, MBI_CommWorld, &(node->recvreq[i]));
        assert(rc == MPI_SUCCESS);
        node->pending_in ++;
    }
    
    /* ---------- issue sends -------------------------- */
    /* allocate request table memory */
    node->sendreq = (MPI_Request *)malloc(sizeof(MPI_Request) * MBI_CommSize);
    assert(node->sendreq != NULL);
    if (node->sendreq == NULL) return MB_ERR_MEMALLOC;
    /* allocate output buffer table */
    node->outbuf = (void **)malloc(sizeof(void *) * MBI_CommSize);
    assert(node->outbuf != NULL);
    if (node->outbuf == NULL) return MB_ERR_MEMALLOC;
    
    node->pending_out = 0; 
    
    if (board->tagging == MB_FALSE)
    {   /* messages not tagged, send the same thing to everyone */
        
        
        if (MBI_CommSize == 1 || board->data->count_current == 0) 
        {   /* if there's nothing to send */  
            for (i = 0; i < MBI_CommSize; i++) node->sendreq[i] = MPI_REQUEST_NULL;
        }
        else
        {
            
            /* allocate memory */
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
                
                rc = MPI_Issend(node->outbuf[0], (int)board->data->count_current * msgsize,
                        MPI_BYTE, i, tag, MBI_CommWorld, &(node->sendreq[i]));
                assert(rc == MPI_SUCCESS);
                node->pending_out ++;
            }       
        }

    }
    else
    {
        /* prep output buffers */
        for (i = 0; i < MBI_CommSize; i++)
        {
            if (outcount[i] == 0 || i == MBI_CommRank)
            {
                node->outbuf[i]  = NULL;
                node->sendreq[i] = MPI_REQUEST_NULL;
                continue;
            }
            
            /* allocate memory for output buffer for proc i */
            node->outbuf[i] = malloc((size_t)(outcount[i] * msgsize));
            assert(node->outbuf[i] != NULL);
            if (node->outbuf[i] == NULL) return MB_ERR_MEMALLOC;
            
        }
        
        /* copy tagged messages into buffer */
        i = 0;
        bytes_required = (int)(MBI_CommSize / 8) + 1;
        for (pl_itr = PL_ITERATOR(board->data); pl_itr; pl_itr = pl_itr->next)
        {
            /* get reference to message object */
            msg = PL_NODEDATA(pl_itr);
            assert(msg != NULL);
            
            /* get refernce to row in TagTable */
            tag_row = (void*)GET_TAG_ROW_FOR_MSG(board->tt->tt, i, bytes_required);
            
            /* Check message tagging and assign to target proc */
            for (w = 0; w < bytes_required; w++)
            {
                /* copy 8-bit window */
                memcpy(&window, (char *)tag_row + w, 1);
                
                b = 0;
                while (window != ALLZEROS)
                {
                    if (MSB_IS_SET(window))
                    {
                        /* target proc */
                        p = (w * 8) + b; 
                        assert(p >= 0);
                        assert(p < MBI_CommSize);
                        assert(p != MBI_CommRank);
                        
                        /* calculate position in buffer */
                        m = outcount[p] - board->tt->tagged[p]; 
                        assert(m >= 0);
                        
                        board->tt->tagged[p] --;
                        assert(board->tt->tagged[p] >= 0);
                        
                        memcpy((char*)node->outbuf[p] + (m * msgsize), 
                                msg, (size_t)msgsize);
                    }
                    
                    /* shift bit and move on */
                    window = window << 1; 
                    b++;
                    
                    
                    /* case where MBI_CommSize < 8 should be handled properly */
                    assert(b <= MBI_CommSize);

                }
            }
                  
            i++;
            assert(i <= (int)board->data->count_current);
        }
        #ifdef DEBUG_TAG_TABLE
        print_tagtable_info(board->tt->tt, board->data->count_current);
        #endif
        FREE_TAG_TABLE(board->tt);
        
        /* issue sends */
        for (i = 0; i < MBI_CommSize; i++)
        {
            if (outcount[i] == 0 || i == MBI_CommRank) 
            {
                node->sendreq[i] = MPI_REQUEST_NULL;
                continue;
            }
            
            rc = MPI_Issend(node->outbuf[i], outcount[i] * msgsize,
                    MPI_BYTE, i, tag, MBI_CommWorld, &(node->sendreq[i]));
            assert(rc == MPI_SUCCESS);
            node->pending_out ++;
        }

    }
    free(outcount);
   
    /* free non required memory */
    if (node->pending_in == 0)
    {
        free(node->inbuf);
        node->inbuf = NULL;
        free(node->incount);
        node->incount = NULL;
        free(node->recvreq);
        node->recvreq = NULL;
    }
    if (node->pending_out == 0)
    {
        free(node->outbuf);
        node->outbuf = NULL;
        free(node->sendreq);
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
 * If there are pending sends, use \c MPI_Testall to check
 * if all sends have completed. If so, free associated memory
 * and set \c node->pending_out to \c 0.
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
 * - \c node is removed from CommQueue and allocated memory is freed
 * - \c board->locked set to ::MB_FALSE
 * - \c board->syncCompleted set to ::MB_TRUE
 * - Local board contains all relevant messages from remote boards
 * - Signal was sent on pthread condition variable associated to board
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_MEMALLOC (could not allocate required memory)
 */ 
int MBIt_Comm_CompletePropagation(struct MBIt_commqueue *node) {
    
    int p, incoming;
    int i, m, rc, completed;
    MBIt_Board *board;
    void *ptr_new, *mloc;
    
    /* sanity check */
    assert(node != NULL);
    assert(node->stage == PROPAGATION);
    
    /* process sends */
    if (node->pending_out > 0)
    {
        rc = MPI_Testall(MBI_CommSize, node->sendreq, &completed, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        
        if (completed)
        {
            free(node->sendreq);
            
            /* get reference to board */
            board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
            assert(board != NULL);
            
            /* free output buffers */
            if (board->tagging == MB_TRUE)
            {
                for (i = 0; i < MBI_CommSize; i++)
                {
                    if (node->outbuf[i] != NULL) free(node->outbuf[i]);
                }

            }
            else
            {
                free(node->outbuf[0]);
            }
            
            free(node->outbuf);
            node->outbuf = NULL;
            node->pending_out = 0;
            
        }
    }
    
    /* process receives */
    if (node->pending_in > 0)
    {
        rc = MPI_Testsome(MBI_CommSize, node->recvreq, &incoming, 
                MBI_comm_indices, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        
        if (incoming > 0)
        {
            /* get reference to board */
            board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
            assert(board != NULL);
            
            for (i = 0; i < incoming; i++)
            {
                p = MBI_comm_indices[i];

                for(m = 0; m < node->incount[p]; m++)
                {
                    /* copy message into local board */
                    rc = pl_newnode(board->data, &ptr_new);
                    assert(rc == PL_SUCCESS);
                    mloc = (void*)((char*)node->inbuf[p] + (m * board->data->elem_size));
                    memcpy(ptr_new, mloc, board->data->elem_size);
                }
                
                free(node->inbuf[p]);
                
            }
            
            node->pending_in -= incoming;
            assert(node->pending_in >= 0);
            
            if (node->pending_in == 0)
            {
                free(node->inbuf);
                node->inbuf = NULL;
                free(node->recvreq);
                node->recvreq = NULL;
                free(node->incount);
                node->incount = NULL;
            }
        }
    }
    
    /* if sends and recvs are completed, finalise sync process */
    if ((node->pending_in == 0 && node->pending_out == 0))
    {

        /* get reference to board */
        board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
        assert(board != NULL);
        
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
    }
    
    return MB_SUCCESS;
}

#ifdef _EXTRA_CHECKS
static int check_equal_fh_and_paramsize(struct MBIt_commqueue *node) {
    
    int tag;
    MBIt_Board *board;
    struct funcdata_t fdata;
    
    /* define MPI tag to use for communication */
    tag = MBI_TAG_FHDATA + ((int)node->mb % MBI_TAG_BASE);
    
    assert(node != NULL);
    board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    assert(board != NULL);
    
    if (MASTERNODE)
    {
        fdata.fh = board->fh;
        fdata.param_size = board->fparams_size;
    }
    
    MPI_Bcast(&fdata, (int)sizeof(struct funcdata_t), MPI_BYTE, 0, MBI_CommWorld);
    
    if (!MASTERNODE)
    {
        assert(fdata.fh == board->fh);
        assert(fdata.param_size == board->fparams_size);
        if (fdata.fh != board->fh || fdata.param_size != board->fparams_size)
        {
            return MB_ERR_INVALID;
        }
    }
    
    return MB_SUCCESS;
}
#endif /*_EXTRA_CHECKS*/

#ifdef DEBUG_TAG_TABLE
static void print_tagtable_info(void *tt, int msgcount) {
    
    int bytes_required;
    int i, j, count, *tagged;
    void *row;
    char window;
    
    assert(tt != NULL);
    assert(msgcount >= 0);
    
    if (msgcount == 0)
    {    
        printf("\n>>TT[%d]<< No messages. Nothing to debug! \n", MBI_CommRank);
        return;
    }
    
    tagged = (int*)malloc(sizeof(int) * MBI_CommSize);
    assert(tagged != NULL);   
    for (i = 0; i < MBI_CommSize; i++)tagged[i] = 0;
 
    bytes_required = (int)(MBI_CommSize / 8) + 1;
    
    printf("\n>>TT[%d]<< TABLE CONTENT:\n", MBI_CommRank);
    for (i = 0; i < msgcount; i++)
    {
        printf(">>TT[%d]<< msg %3d : ", MBI_CommRank, i);
        j = 0;
        row = GET_TAG_ROW_FOR_MSG(tt, i, bytes_required);
        for (count = 0; count < bytes_required * 8; count++)
        {
            if (count % 8 == 0) 
            {
                memcpy(&window, (void *)((char*)row + j), 1);
                j++;
            }
            
            if (MSB_IS_SET(window))
            {
                printf("1");
                assert(count < MBI_CommSize);
                tagged[count]++;
            }
            else
            {
                printf("0");
            }
            
            window = window << 1;
        }
        printf("\n");
    }
    
}
#endif /* DEBUG_TAG_TABLE  */
