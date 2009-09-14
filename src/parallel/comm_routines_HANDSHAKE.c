/* $Id$ */
/*!
 * \file parallel/comm_routines_HANDSHAKE.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Sept 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief State Transition Routines for processing communication nodes
 * 
 * These transition routines uses the HANDSHAKE protocol where buffer
 * sizes are pre-agreed before actual data is sent. In addition, receives
 * are always posted before sends so the underlying MPI implementation
 * is always ready for an incoming communication.
 * 
 * This protocol is the default protocol used by libmboard. It is not 
 * the fastest possible solution but should provides a good balance
 * between performance and portability.   
 * 
 * It was designed to work safely with all MPI libraries
 * and avoids optimisations that are dependent on the performance 
 * characteristics of certain operations within the underlying MPI 
 * implementation.
 * 
 */
#include "mb_parallel.h"
#include "mb_commqueue.h"
#include "mb_commroutines.h"
#include "mb_pooled_list.h"
#include "mb_tag_table.h"

/*! 
 * \brief Tag messages in the board
 * \param[in] node address of CommQueue node
 * \return Return Code
 * 
 * This routine is to be executed during the first communication stage
 * (::MB_COMM_HANDSHAKE_PRE_PROP).
 * 
 * Pre:
 * - node->mb is valid
 * - node->mb maps to valid board where board->locked == ::MB_TRUE
 * - node->mb maps to valid board where board->syncCompleted == ::MB_FALSE
 * - node->stage == ::MB_COMM_HANDSHAKE_PRE_PROP
 * - node->flag_fdrFallback = ::MB_FALSE
 * - node->flag_shareOutbuf = ::MB_FALSE
 * - node->incount == NULL
 * - node->outcount == NULL
 * - node->inbuf == NULL
 * - node->outbuf == NULL
 * - node->sendreq == NULL
 * - node->recvreq == NULL
 * - node->board == NULL
 *
 * Steps:
 * -# Get pointer to board and cache it in node->board
 * -# if no readers and no writers, we're done
 *  - set node->stage = ::MB_COMM_END
 *  - return ::MB_SUCCESS_2
 * -# Allocate memory for node->incount and node->outcount
 * -# Allocate memory for node->sendreq and node->recvreq
 * -# Populate \c node->incount 
 *  - if board is reader and writer_count > 0:
 *   - issue MPI_Irecv() from all readers. Use node->recvreq[i] to hold MPI request. 
 *      Use node->incount[i] as receive buffer.
 *   - set node->recvreq[non-writers] = MPI_REQUEST_NULL
 *   - set node->incount[non-writers] = -1
 *   - sets node->pending_in to number of MPI_Irecv() issued
 *  - else:
 *   - set node->incount[*] = -1
 *   - set node->recvreq[*] = MPI_REQUEST_NULL
 *   - set node->pending_id = 0
 * -# Generate and propagate outcount
 *  - if board is not writer or board->reader_count == 0:
 *   - set node->outcount[*] = -1
 *   - set node->sendreq[*]  = MPI_REQUEST_NULL
 *   - set node->pending_out = 0
 *  - else
 *   - run rc = MBI_CommUtil_TagMessages(board, node->outcount)
 *   - if rc == MB_SUCCESS_2
 *    - set node->flag_shareOutbuf = ::MB_TRUE
 *    - set node->flag_fdrFallback = ::MB_TRUE
 *   - if board->filter == NULL (no filter)
 *    - set node->flag_shareOutbuf = ::MB_TRUE
 *   - where node->outcount[i] != -1, issue MPI_Issend() using node->recvreq[i] 
 *     to hold MPI request and use node->outcount[i] as send buffer. For other i,
 *     set node->recvreq[i] = MPI_REQUEST_NULL
 *   - sets node->pending_out to number of MPI_Issend() issued
 * -# set node->stage = MB_COMM_HANDSHAKE_START_PROP
 *     
 * 
 * Test cases:
 * - All boards READWRITE
 *  - node->board != NULL
 *  - node->incount != NULL
 *  - node->outcount != NULL
 *  - node->recvreq : all not MPI_REQUEST_NULL except self
 *  - node->incount[self] == -1
 *  - node->flag_fdrFallback = ::MB_FALSE
 *  - node->flag_shareOutbuf = ::MB_TRUE
 *  - node->outcount[!self] = board->data->count_current - board->synced_cursor
 *  - node->outcount[self] = -1
 *  - node->sendreq[!self] != MPI_REQUEST_NULL
 *  - node->sendreq[self] == MPI_REQUEST_NULL
 *  - node->board->tt == NULL
 *  - node->stage == MB_COMM_HANDSHAKE_START_PROP
 *  - node->pending_in  = board->writer_count
 *  - node->pending_out = board->reader_count
 *  - returns ::MB_SUCCESS
 *
 * - All boards READWRITE (with filter)
 *  - node->board != NULL
 *  - node->incount != NULL
 *  - node->outcount != NULL
 *  - node->recvreq : all not MPI_REQUEST_NULL except self
 *  - node->incount[self] == -1
 *  - node->flag_fdrFallback = ::MB_FALSE
 *  - node->flag_shareOutbuf = ::MB_FALSE
 *  - node->outcount[!self] = board->data->count_current - board->synced_cursor
 *  - node->outcount[self] = -1
 *  - node->sendreq[!self] != MPI_REQUEST_NULL
 *  - node->sendreq[self] == MPI_REQUEST_NULL
 *  - node->board->tt != NULL
 *  - node->stage == MB_COMM_HANDSHAKE_START_PROP
 *  - node->pending_in  = board->writer_count
 *  - node->pending_out = board->reader_count
 *  - returns ::MB_SUCCESS
 * 
 * - All boards READWRITE (with filter + fallback to FDR)
 *  - node->board != NULL
 *  - node->incount != NULL
 *  - node->outcount != NULL
 *  - node->recvreq : all not MPI_REQUEST_NULL except self
 *  - node->incount[self] == -1
 *  - node->flag_fdrFallback = ::MB_TRUE
 *  - node->flag_shareOutbuf = ::MB_TRUE
 *  - node->outcount[!self] = board->data->count_current - board->synced_cursor
 *  - node->outcount[self] = -1
 *  - node->sendreq[!self] != MPI_REQUEST_NULL
 *  - node->sendreq[self] == MPI_REQUEST_NULL
 *  - node->board->tt == NULL
 *  - node->stage == MB_COMM_HANDSHAKE_START_PROP
 *  - node->pending_in  = board->writer_count
 *  - node->pending_out = board->reader_count
 *  - returns ::MB_SUCCESS
 * 
 * - Even boards READWRITE, odd boards IDLE
 *  - node->board != NULL
 *  - node->incount != NULL
 *  - node->outcount != NULL
 *  - node->flag_fdrFallback = ::MB_FALSE
 *  - node->flag_shareOutbuf = ::MB_TRUE
 *  - on READWRITE boards
 *   - node->recvreq[writers] != MPI_REQUEST_NULL
 *   - node->recvreq[!writers] == MPI_REQUEST_NULL
 *   - node->incount : 0 for odd boards and self
 *   - node->outcount[readers] = board->data->count_current - board->synced_cursor
 *   - node->outcount[!readers] = -1
 *   - node->sendreq[readers] != MPI_REQUEST_NULL
 *   - node->sendreq[!readers] == MPI_REQUEST_NULL
 *   - node->board->tt == NULL
 *   - node->stage == MB_COMM_HANDSHAKE_START_PROP
 *   - node->pending_in  = board->writer_count
 *   - node->pending_out = board->reader_count
 *   - returns ::MB_SUCCESS
 *  - on IDLE boards
 *   - node->board == NULL
 *   - node->incount == NULL
 *   - node->outcount == NULL
 *   - node->sendreq == NULL
 *   - node->sendreq == NULL
 *   - node->stage == MB_COMM_END
 *   - returns ::MB_SUCCESS_2
 * 
 * - Even board READONLY, odd boards WRITEONLY
 *  - node->board != NULL
 *  - node->incount != NULL
 *  - node->outcount != NULL
 *  - on READONLY boards
 *   - node->recvreq[writers] != MPI_REQUEST_NULL
 *   - node->recvreq[!writers] == MPI_REQUEST_NULL
 *   - node->incount : 0 for even boards and self
 *   - node->outcount[*] == -1
 *   - node->sendreq[*] == MPI_REQUEST_NULL
 *  - on WRITEONLY boards
 *   - node->recvreq[*] == MPI_REQUEST_NULL
 *   - node->incount[*] == -1
 *   - node->outcount[readers] = board->data->count_current - board->synced_cursor
 *   - node->outcount[!readers] = -1
 *   - node->sendreq[readers] != MPI_REQUEST_NULL
 *   - node->sendreq[!readers] == MPI_REQUEST_NULL
 *  - node->pending_in  = board->writer_count
 *  - node->pending_out = board->reader_count
 *  - node->board->tt == NULL
 *  - node->stage == MB_COMM_HANDSHAKE_START_PROP
 *  - returns ::MB_SUCCESS
 */
int MBI_CommRoutine_HANDSHAKE_AgreeBufSizes(struct MBIt_commqueue *node) {
    
    int i, p, rc, tag;
    
    assert(node != NULL);
    assert(node->incount  == NULL);
    assert(node->outcount == NULL);
    assert(node->inbuf    == NULL);
    assert(node->outbuf   == NULL);
    assert(node->sendreq  == NULL);
    assert(node->recvreq  == NULL);
    assert(node->sendreq2 == NULL);
    assert(node->recvreq2 == NULL);
    assert(node->board    == NULL);
    assert(node->pending_in  == 0);
    assert(node->pending_out == 0);
    assert(node->flag_fdrFallback == MB_FALSE);
    assert(node->flag_shareOutbuf == MB_FALSE);
    assert(node->stage == MB_COMM_HANDSHAKE_PRE_PROP);
    
    /* get reference to board object and cache ptr in node */
    node->board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    assert(node->board != NULL);
    assert(node->board->locked == MB_TRUE);
    assert(node->board->syncCompleted == MB_FALSE);
 
    /* if no readers not writers, sync process is done */
    if (node->board->writer_count == 0 && node->board->reader_count == 0)
    {   
        node->stage = MB_COMM_END;
        return MB_SUCCESS_2; /* signal sync completed */
    }   
    
    /* allocate memory */
    node->incount = (int *)malloc(sizeof(int) * MBI_CommSize);
    assert(node->incount != NULL);
    if (node->incount == NULL) return MB_ERR_MEMALLOC;
    node->outcount = (int *)malloc(sizeof(int) * MBI_CommSize);
    assert(node->outcount != NULL);
    if (node->outcount == NULL) return MB_ERR_MEMALLOC;
    node->sendreq = (MPI_Request *)malloc(sizeof(MPI_Request) * MBI_CommSize);
    assert(node->sendreq != NULL);
    if (node->sendreq == NULL) return MB_ERR_MEMALLOC;
    node->recvreq = (MPI_Request *)malloc(sizeof(MPI_Request) * MBI_CommSize);
    assert(node->recvreq != NULL);
    if (node->recvreq == NULL) return MB_ERR_MEMALLOC;
    
    /* initialise values */
    for (i = 0; i < MBI_CommSize; i++)
    {
        node->incount[i] = node->outcount[i] = -1;
        node->sendreq[i] = node->recvreq[i] = MPI_REQUEST_NULL;
    }
    
    /* generate unique tag from this board */
    assert(node->mb <= MBI_TAG_BASE);
    tag = MBI_TAG_MSGDATA | node->mb;
    assert(tag < MBI_TAG_MAX);
    
    /* gather information from writers on incoming messages */
    assert(node->board->writer_list != NULL);
    assert(node->board->writer_count < MBI_CommSize);
    for (i = 0; i < node->board->writer_count; i++)
    {
        p = node->board->writer_list[i];
        rc = MPI_Irecv(&(node->incount[p]), 1, MPI_INT, p, 
                       tag, MBI_CommWorld, &(node->recvreq[p]));
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        
        node->pending_in++;
    }
    
    /* distribute information to readers on outgoing messages */
    assert(node->board->reader_list != NULL);
    assert(node->board->reader_count < MBI_CommSize);
    node->flag_shareOutbuf = MB_TRUE;
    if (node->board->reader_count > 0)
    {
        rc = MBI_CommUtil_TagMessages(node->board, node->outcount);
        if (rc == MB_SUCCESS_2) /* fallback to full data replication */
        {
            node->flag_fdrFallback = MB_TRUE;
        }
        else if (rc == MB_SUCCESS)
        {
            node->flag_fdrFallback = MB_FALSE;
            if (node->board->filter != NULL)
            {
                assert(node->board->tt != NULL);
                node->flag_shareOutbuf = MB_FALSE;
            }
        }
        else return MB_ERR_INTERNAL;
        
        for (i = 0; i < MBI_CommSize; i++)
        {
            if (node->outcount[i] != -1)
            {
                assert(i != MBI_CommRank);
                rc = MPI_Issend(&(node->outcount[i]), 1, MPI_INT, i,
                        tag, MBI_CommWorld, &(node->sendreq[i]));
                assert(rc == MPI_SUCCESS);
                if (rc != MPI_SUCCESS) return MB_ERR_MPI;
                
                node->pending_out++;
            }
        }
    }
    
    node->stage = MB_COMM_HANDSHAKE_START_PROP;
    return MB_SUCCESS;
}

/*! 
 * \brief Allocate and populate buffers then start message propagation
 * \param[in] node address of CommQueue node
 * \return Return Code
 * 
 * This routine is to be executed during stage ::MB_COMM_HANDSHAKE_START_PROP.
 * 
 * Pre:
 * - node->board != NULL
 * - node->incount != NULL
 * - node->outcount != NULL
 * - node->sendreq != NULL
 * - node->recvreq != NULL
 * - node->stage == ::MB_COMM_HANDSHAKE_START_PROP
 * 
 * Steps:
 * - if node->sendreq2 == NULL
 *  - allocate memory for node->sendreq2
 * - if node->recvreq2 == NULL
 *  - allocate memory for node->recvreq2
 * - if node->inbuf == NULL
 *  - allocate memory for node->inbuf
 *  - set all values to NULL
 * - if node->outbuf == NULL
 *  - allocate memory for node->outbuf
 *  - set all values to NULL
 *  - if board->data->count_current != board->synced_cursor 
 *   - if node->flag_shareOutbuf == ::MB_TRUE
 *    - call MBI_CommUtil_BuildBuffer_All(...node->outbuf[0]...)
 *   - else
 *    - call MBI_CommUtil_BuildBuffer_Tagged(...node->outbuf...)
 * - if node->pending_out > 0
 *  - MPI_Testsome(node->recvreq). For each completed send:
 *   - decrement node->pending_out
 *   - if node->outcount[p] > 0
 *    - if node->flag_shareOutbuf == ::MB_TRUE
 *     - MPI_Issend(p ... node->outbuf[0])
 *    - else
 *     - MPI_Issend(p ... node->outbuf[p])
 *    - increment node->pending_out2
 *   - else
 *    - set node->sendreq2[p] == MPI_REQUEST_NULL
 * - if node->pending_in > 0:
 *  - MPI_Testsome(node->recvreq). For each completed recv:
 *   - decrement node->pending_in
 *   - if node->incount[p] > 0:
 *    - allocate memory for node->inbuf[p] (1 + (incount * msgsize))
 *    - issue MPI_Irecv(p, ..., node->inbuf[p], ..., node->recvreq2[p])
 *    - increment node->pending_in2
 *   - else :
 *    - set node->recvreq2[p] = MPI_REQUEST_NULL
 *  - if node->pending_in == 0 and node->pending_out == 0:
 *   - free node->sendreq
 *   - free node->recvreq
 *   - free node->outcount
 *   - set node->stage = ::MB_COMM_HANDSHAKE_COMPLETE_PROP
 *  - return ::MB_SUCCESS
 * 
 * Test cases (routine should be called repeatedly until state changes before
 * performing the following checks):
 * - All boards READWRITE
 *  - node->stage == ::MB_COMM_HANDSHAKE_COMPLETE_PROP
 *  - node->sendreq == NULL
 *  - node->recvreq == NULL
 *  - node->outcount == NULL
 *  - node->incount != NULL
 *  - node->sendreq2 != NULL
 *  - node->recvreq2 != NULL
 *  - node->sendreq2[!self] != MPI_REQUEST_NULL
 *  - node->sendreq2[self] == MPI_REQUEST_NULL
 *  - node->recvreq2[!self] != MPI_REQUEST_NULL
 *  - node->recvreq2[self] == MPI_REQUEST_NULL
 *  - node->pending_in2 == MBI_CommSize - 1
 *  - node->pending_out2 == MBI_CommSize - 1
 *  - node->outbuf[0] != NULL
 *  - node->outbuf[!0] == NULL
 *  - node->inbuf[!self] != NULL
 *  - node->inbuf[self] == NULL 
 *  
 * - All boards READWRITE (with filter)
 *  - node->stage == ::MB_COMM_HANDSHAKE_COMPLETE_PROP
 *  - node->sendreq == NULL
 *  - node->recvreq == NULL
 *  - node->outcount == NULL
 *  - node->incount != NULL
 *  - node->sendreq2 != NULL
 *  - node->recvreq2 != NULL
 *  - node->sendreq2[!self] != MPI_REQUEST_NULL
 *  - node->sendreq2[self] == MPI_REQUEST_NULL
 *  - node->recvreq2[!self] != MPI_REQUEST_NULL
 *  - node->recvreq2[self] == MPI_REQUEST_NULL
 *  - node->pending_in2 == MBI_CommSize - 1
 *  - node->pending_out2 == MBI_CommSize - 1
 *  - node->outbuf[!self] != NULL
 *  - node->outbuf[self] == NULL
 *  - node->inbuf[!self] != NULL
 *  - node->inbuf[self] == NULL 
 * 
 * - All boards READWRITE (with filter + fallback to FDR)
 *  - node->stage == ::MB_COMM_HANDSHAKE_COMPLETE_PROP
 *  - node->sendreq == NULL
 *  - node->recvreq == NULL
 *  - node->outcount == NULL
 *  - node->incount != NULL
 *  - node->sendreq2 != NULL
 *  - node->recvreq2 != NULL
 *  - node->sendreq2[!self] != MPI_REQUEST_NULL
 *  - node->sendreq2[self] == MPI_REQUEST_NULL
 *  - node->recvreq2[!self] != MPI_REQUEST_NULL
 *  - node->recvreq2[self] == MPI_REQUEST_NULL
 *  - node->pending_in2 == MBI_CommSize - 1
 *  - node->pending_out2 == MBI_CommSize - 1
 *  - node->outbuf[0] != NULL
 *  - node->outbuf[!0] == NULL
 *  - node->inbuf[!self] != NULL
 *  - node->inbuf[self] == NULL 
 * 
 * - Even boards READWRITE, odd boards IDLE
 *  - IDLE boards should never reach this stage!
 *  - on READWRITE boards
 *   - node->stage == ::MB_COMM_HANDSHAKE_COMPLETE_PROP
 *   - node->sendreq == NULL
 *   - node->recvreq == NULL
 *   - node->outcount == NULL
 *   - node->incount != NULL
 *   - node->sendreq2 != NULL
 *   - node->recvreq2 != NULL
 *   - node->sendreq2[readers] != MPI_REQUEST_NULL
 *   - node->sendreq2[!readers] == MPI_REQUEST_NULL
 *   - node->recvreq2[writers] != MPI_REQUEST_NULL
 *   - node->recvreq2[!writers] == MPI_REQUEST_NULL
 *   - node->pending_in2 == board->writer_count
 *   - node->pending_out2 == board->reader_count
 *   - node->outbuf[0] != NULL
 *   - node->outbuf[!0] == NULL
 *   - node->inbuf[writers] != NULL
 *   - node->inbuf[!writers] == NULL 
 * 
 * - Even board READONLY, odd boards WRITEONLY
 *  - node->stage == ::MB_COMM_HANDSHAKE_COMPLETE_PROP
 *  - node->sendreq == NULL
 *  - node->recvreq == NULL
 *  - node->outcount == NULL
 *  - node->incount != NULL
 *  - node->sendreq2 != NULL
 *  - node->recvreq2 != NULL
 *  - node->sendreq2[readers] != MPI_REQUEST_NULL
 *  - node->sendreq2[!readers] == MPI_REQUEST_NULL
 *  - node->recvreq2[writers] != MPI_REQUEST_NULL
 *  - node->recvreq2[!writers] == MPI_REQUEST_NULL
 *  - node->pending_in2 == board->writer_count
 *  - node->pending_out2 == board->reader_count
 *  - node->outbuf[0] != NULL
 *  - node->outbuf[!0] == NULL
 *  - node->inbuf[writers] != NULL
 *  - node->inbuf[!writers] == NULL
 *  
 */
int MBI_CommRoutine_HANDSHAKE_PropagateMessages(struct MBIt_commqueue *node) {
    
    int i, p, rc, tag, size;
    int completed;
    unsigned int mcount;
    size_t ret_size;
    void *buf;
    
    assert(node != NULL);
    assert(node->incount  != NULL);
    assert(node->outcount != NULL);
    assert(node->sendreq  != NULL);
    assert(node->recvreq  != NULL);
    assert(node->board    != NULL);
    assert(node->stage == MB_COMM_HANDSHAKE_START_PROP);
    
    /* if first time routine called, allocate and initialise buffers */
    if (node->inbuf == NULL)
    {
        assert(node->outbuf == NULL);
        assert(node->sendreq2 == NULL);
        assert(node->recvreq2 == NULL);
        
        /* array to store MPI send requests */
        node->sendreq2 = (MPI_Request *)malloc(sizeof(MPI_Request) 
                                               * MBI_CommSize);
        assert(node->sendreq2 != NULL);
        if (node->sendreq2 == NULL) return MB_ERR_MEMALLOC;
        for (i = 0; i < MBI_CommSize; i++) 
            node->sendreq2[i] = MPI_REQUEST_NULL;

        /* array to store MPI recv requests */
        node->recvreq2 = (MPI_Request *)malloc(sizeof(MPI_Request) 
                                               * MBI_CommSize);
        assert(node->recvreq2 != NULL);
        if (node->recvreq2 == NULL) return MB_ERR_MEMALLOC;
        for (i = 0; i < MBI_CommSize; i++) 
            node->recvreq2[i] = MPI_REQUEST_NULL;

        /* array to store input buffers */
        node->inbuf = malloc(sizeof(void *) * MBI_CommSize);
        assert(node->inbuf != NULL);
        if (node->inbuf == NULL) return MB_ERR_MEMALLOC;
        for (i = 0; i < MBI_CommSize; i++) node->inbuf[i] = NULL;
        
        /* array to store output buffers */
        node->outbuf = (void**)malloc(sizeof(void *) * MBI_CommSize);
        assert(node->outbuf != NULL);
        if (node->outbuf == NULL) return MB_ERR_MEMALLOC;
        for (i = 0; i < MBI_CommSize; i++) node->outbuf[i] = NULL;
        
        /* build send buffers */
        mcount = node->board->data->count_current -
                 node->board->synced_cursor;
        if (mcount != 0)
        {
            if (node->flag_shareOutbuf == MB_TRUE)
            {
                rc = MBI_CommUtil_BuildBuffer_All(node->board, 
                                                  node->outbuf, 
                                                  node->flag_fdrFallback, 
                                                  &ret_size);
                assert(rc == MB_SUCCESS);
                if (rc != MB_SUCCESS) return rc;
                assert(node->outbuf[0] != NULL);
                assert((int)ret_size == 
                       (int)(1 + (mcount * node->board->data->elem_size)));
            }
            else /* messages tagged */
            {
                assert(node->board->tt != NULL);
                rc = MBI_CommUtil_BuildBuffers_Tagged(node->board, 
                                                      node->outbuf, 
                                                      node->outcount);
                assert(rc == MB_SUCCESS);
                if (rc != MB_SUCCESS) return rc;
                
                /* delete tag table */
                rc = tt_delete(&(node->board->tt));
                assert(rc == TT_SUCCESS);
                assert(node->board->tt == NULL);
            }
        }
    }
    assert(node->inbuf != NULL);
    assert(node->outbuf != NULL);
    assert(node->sendreq2 != NULL);
    assert(node->recvreq2 != NULL);
    
    /* generate unique tag from this board */
    assert(node->mb <= MBI_TAG_BASE);
    tag = MBI_TAG_PROPDATA | node->mb;
    assert(tag < MBI_TAG_MAX);
    
    /* process pending sends */
    if (node->pending_out > 0)
    {
        rc = MPI_Testsome(MBI_CommSize, node->sendreq, &completed,
                          MBI_comm_indices, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        
        for (i = 0; i < completed; i++)
        {
            node->pending_out--;
            p = MBI_comm_indices[i];
            if (node->outcount[p] < 1) continue; /* nothing to send */
            
            if (node->flag_shareOutbuf == MB_TRUE) buf = node->outbuf[0];
            else buf = node->outbuf[p];
            assert(buf != NULL);
           
            size = 1 + (node->outcount[p] * node->board->data->elem_size);
            
            rc = MPI_Issend(buf, size, MPI_BYTE, p, 
                            tag, MBI_CommWorld, &(node->sendreq2[p]));
            assert(rc == MPI_SUCCESS);
            if (rc != MPI_SUCCESS) return MB_ERR_MPI;
            
            node->pending_out2 ++;
        }
    }
    
    /* process pending receives */
    if (node->pending_in > 0)
    {
        rc = MPI_Testsome(MBI_CommSize, node->recvreq, &completed,
                          MBI_comm_indices, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        
        for (i = 0; i < completed; i++)
        {
            node->pending_in--;
            p = MBI_comm_indices[i];
            if (node->incount[p] < 1) continue; /* nothing to receive */
            
            size = 1 + (node->incount[p] * node->board->data->elem_size);
            node->inbuf[p] = malloc((size_t)size);
            assert(node->inbuf[p] != NULL);
            if (node->inbuf[p] == NULL) return MB_ERR_MEMALLOC;
            
            rc = MPI_Irecv(node->inbuf[p], size, MPI_BYTE, p, 
                            tag, MBI_CommWorld, &(node->recvreq2[p]));
            assert(rc == MPI_SUCCESS);
            if (rc != MPI_SUCCESS) return MB_ERR_MPI;
            
            node->pending_in2 ++;
        }
    }
    
    /* when all pending sends/receives completed
     * clean up and move on to next stage
     */
    if (node->pending_in == 0 && node->pending_out == 0)
    {
        free(node->sendreq);  node->sendreq = NULL;
        free(node->recvreq);  node->recvreq = NULL;
        free(node->outcount); node->outcount = NULL;
        node->stage = MB_COMM_HANDSHAKE_COMPLETE_PROP;
    }
    
    return MB_SUCCESS;
}

/*! 
 * \brief Complete message propagation and load messages into local board
 * \param[in] node address of CommQueue node
 * \return Return Code
 * 
 * This routine is to be executed during stage ::MB_COMM_HANDSHAKE_COMPLETE_PROP.
 * 
 * Pre:
 * - node->board != NULL
 * - node->incount != NULL
 * - node->outcount == NULL
 * - node->sendreq == NULL
 * - node->recvreq == NULL
 * - node->recvreq2 != NULL
 * - node->sendreq2 != NULL
 * - node->inbuf != NULL
 * - node->outbuf != NULL
 * - node->stage == ::MB_COMM_HANDSHAKE_COMPLETE_PROP
 * 
 * Steps:
 * - if node->pending_out2 > 0
 *  - MPI_Testsome(node->sendreq). For each completed send:
 *   - decrement node->pending_out2
 *   - if node->flag_shareOutbuf == ::MB_FALSE, free node->outbuf[p]
 * - if node->pending_in2 > 0
 *  - MPI_Testsome(node->recvreq). For each completed recv:
 *   - decrement node->pending_in2
 *   - determine bufsize based on node->incount[p] (1 + (msgsize * count)) 
 *   - run MBI_CommUtil_LoadBuffer(node->inbuf[p], bufsize)
 *   - free node->inbuf[p]
 * - if node->pending_in == 0 and node->pending_out == 0
 *  - if node->flag_shareOutbuf == ::MB_TRUE
 *   - free node->outbuf[0]
 *  - free node->inbuf
 *  - free node->outbuf
 *  - free node->incount
 *  - free node->sendreq2
 *  - free node->recvreq2
 *  - node->stage == ::MB_COMM_END
 *  - return MB_SUCCESS_2
 * - return MB_SUCCESS
 * 
 * Test cases (routine should be called repeatedly until state changes before
 * performing the following checks):
 * - All boards READWRITE
 *  - node->sendreq == NULL
 *  - node->recvreq == NULL
 *  - node->sendreq2 == NULL
 *  - node->recvreq2 == NULL
 *  - node->inbuf == NULL
 *  - node->outbuf == NULL
 *  - node->incount == NULL
 *  - node->outcount == NULL
 *  - node->pending_in == 0
 *  - node->pending_out == 0
 *  - node->pending_in2 == 0
 *  - node->pending_out2 == 0
 *  - node->stage == ::MB_COMM_END
 *  - return code == ::MB_SUCCESS_2
 *  - node->board should have correct number of messages
 * 
 * - All boards READWRITE (with filter)
 *  - node->sendreq == NULL
 *  - node->recvreq == NULL
 *  - node->sendreq2 == NULL
 *  - node->recvreq2 == NULL
 *  - node->inbuf == NULL
 *  - node->outbuf == NULL
 *  - node->incount == NULL
 *  - node->outcount == NULL
 *  - node->pending_in == 0
 *  - node->pending_out == 0
 *  - node->pending_in2 == 0
 *  - node->pending_out2 == 0
 *  - node->stage == ::MB_COMM_END
 *  - return code == ::MB_SUCCESS_2
 *  - node->board should have correct number of messages
 * 
 * - All boards READWRITE (with filter + fallback to FDR)
 *  - node->sendreq == NULL
 *  - node->recvreq == NULL
 *  - node->sendreq2 == NULL
 *  - node->recvreq2 == NULL
 *  - node->inbuf == NULL
 *  - node->outbuf == NULL
 *  - node->incount == NULL
 *  - node->outcount == NULL
 *  - node->pending_in == 0
 *  - node->pending_out == 0
 *  - node->pending_in2 == 0
 *  - node->pending_out2 == 0
 *  - node->stage == ::MB_COMM_END
 *  - return code == ::MB_SUCCESS_2
 *  - node->board should have correct number of messages
 * 
 * - Even boards READWRITE, odd boards IDLE
 *  - IDLE boards should never reach this stage!
 *  - on READWRITE boards
 *   - node->sendreq == NULL
 *   - node->recvreq == NULL
 *   - node->sendreq2 == NULL
 *   - node->recvreq2 == NULL
 *   - node->inbuf == NULL
 *   - node->outbuf == NULL
 *   - node->incount == NULL
 *   - node->outcount == NULL
 *   - node->pending_in == 0
 *   - node->pending_out == 0
 *   - node->pending_in2 == 0
 *   - node->pending_out2 == 0
 *   - node->stage == ::MB_COMM_END
 *   - return code == ::MB_SUCCESS_2
 *   - node->board should have correct number of messages
 * 
 * - Even board READONLY, odd boards WRITEONLY
 *  - node->sendreq == NULL
 *  - node->recvreq == NULL
 *  - node->sendreq2 == NULL
 *  - node->recvreq2 == NULL
 *  - node->inbuf == NULL
 *  - node->outbuf == NULL
 *  - node->incount == NULL
 *  - node->outcount == NULL
 *  - node->pending_in == 0
 *  - node->pending_out == 0
 *  - node->pending_in2 == 0
 *  - node->pending_out2 == 0
 *  - node->stage == ::MB_COMM_END
 *  - return code == ::MB_SUCCESS_2
 *  - node->board should have correct number of messages
 */
int MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers(struct MBIt_commqueue *node) {
    
    int i, p, rc, completed;
    int size;
    
    assert(node != NULL);
    assert(node->board  != NULL);
    assert(node->pending_in2 == 0  || node->inbuf  != NULL);
    assert(node->pending_out2 == 0 || node->outbuf != NULL);
    assert(node->sendreq  == NULL);
    assert(node->recvreq  == NULL);
    assert(node->sendreq2 != NULL);
    assert(node->recvreq2 != NULL);
    assert(node->incount  != NULL);
    assert(node->outcount == NULL);
    assert(node->stage == MB_COMM_HANDSHAKE_COMPLETE_PROP);
    
    /* handle pending sends */
    if (node->pending_out2 > 0)
    {
        rc = MPI_Testsome(MBI_CommSize, node->sendreq2, &completed,
                          MBI_comm_indices, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        assert(completed <= (int)node->pending_out2);
        
        for (i = 0; i < completed; i++)
        {
            node->pending_out2 --;
            p = MBI_comm_indices[i];
            if (node->flag_shareOutbuf == MB_FALSE)
            {
                assert(node->outbuf[p] != NULL);
                free(node->outbuf[p]);
                node->outbuf[p] = NULL;
            }
        }
    }
    
    /* handle pending recvs */
    if (node->pending_in2 > 0)
    {
        rc = MPI_Testsome(MBI_CommSize, node->recvreq2, &completed,
                          MBI_comm_indices, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        assert(completed <= (int)node->pending_in2);
        
        for (i = 0; i < completed; i++)
        {
            node->pending_in2 --;
            p = MBI_comm_indices[i];
            
            size = 1 + (node->incount[p] * node->board->data->elem_size);
            rc = MBI_CommUtil_LoadBuffer(node->board, node->inbuf[p], 
                                         (size_t)size);
            assert(rc == MB_SUCCESS);
            if (rc != MB_SUCCESS) return rc;
            
            free(node->inbuf[p]);
            node->inbuf[p] = NULL;
        }
    }
    
    /* if all comms completed, we're done! */
    if (node->pending_in2 == 0 && node->pending_out2 == 0)
    {
        if (node->outbuf[0] != NULL) free(node->outbuf[0]);
        
        free(node->outbuf);   node->outbuf = NULL;
        free(node->inbuf);    node->inbuf = NULL;
        free(node->incount);  node->incount = NULL;
        free(node->sendreq2); node->sendreq2 = NULL;
        free(node->recvreq2); node->recvreq2 = NULL;
        

        
        /* capture lock for board */
        rc = pthread_mutex_lock(&(node->board->syncLock));
        assert(0 == rc);
        
        /* mark sync as completed */
        node->board->syncCompleted = MB_TRUE;

        /* update cursor */
        rc = MBI_CommUtil_UpdateCursor(node->board);
        assert(rc == MB_SUCCESS);
        
        /* release lock */
        rc = pthread_mutex_unlock(&(node->board->syncLock));
        assert(0 == rc);
        
        /* send signal to wake main thread waiting on this board */
        rc = pthread_cond_signal(&(node->board->syncCond));
        assert(0 == rc);
        
        node->stage = MB_COMM_END;
        return MB_SUCCESS_2; /* signal end of comm process */
    }
    
    return MB_SUCCESS;
}
