/* $Id$ */
/*!
 * \file parallel/comm.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief State Transition Routines for processing communication nodes
 * 
 * \image html CommNode.png
 * \image latex CommNode.eps
 * 
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
/*! \brief query if specific bits are set */
#define BIT_IS_SET(octet, mask) ((octet & mask) == mask)

/*! 
 * \brief Tag messages in the board
 * \param[in] node address of CommQueue node
 * \return Return Code
 * 
 * This routine is to be executed during the firs commncation stage
 * (::PRE_TAGGING).
 * 
 * Pre:
 * - node->mb is valid
 * - board->locked == ::MB_TRUE
 * - board->syncCompleted == ::MB_FALSE
 * - node->stage == ::PRE_TAGGING
 * - node->flag_fdrFallback = ::MB_FALSE
 * - node->flag_shareOutbuf = ::MB_FALSE
 * - node->incount == \c NULL
 * - node->outcount == \c NULL
 * - node->inbuf == \c NULL
 * - node->outbuf == \c NULL
 * - node->sendreq == \c NULL
 * - node->recvreq == \c NULL
 * - node->board == \c NULL
 * 
 * Steps:
 * -# Get pointer to board object and cache it in node->board
 * -# Allocate memory for node->outcount
 * -# If node->mb->filter == \c NULL or node->board->data->count_current = 0
 *  -# set node->outcount[*] = node->board->data->count_current
 *  -# set node->flag_shareOutbuf = ::MB_TRUE
 * -# If node->board->filter != \c NULL
 *  -# Use node->board->filter to build tag table in node->board->tt
 *  -# Allocate memory for node->outcount
 *  -# Initialise values in node->outcount[] based on contents of node->board->tt.
 *     Keep count of outcount total as we go along. If total > node->board->data->count_current, 
 *     fallback to full data replication
 *   - clear tag table
 *   - set node->outcount[*] = node->board->data->count_current
 *   - set node->flag_fdrFallback = ::MB_TRUE
 *   - set node->flag_shareOutbuf = ::MB_TRUE
 * -# set node->stage to ::READY_FOR_PROP
 * 
 * Post:
 * - node->stage == ::READY_FOR_PROP
 * - node->outcount != \c NULL
 * - node->board != \c NULL
 * - if (node->board->filter != NULL)
 *  - if (node->flag_fdrFallback == ::MB_TRUE) node->board->tt == \c NULL
 *  - if (node->flag_fdrFallback == ::MB_FALSE) node->board->tt != \c NULL
 * - if node->board->filter == \c NULL or node->fdr_fallback == \c ::MB_TRUE
 *  - node->flag_shareOutbuf == ::MB_TRUE
 */
int MBI_Comm_TagMessages(struct MBIt_commqueue *node) {
    
    char window;
    int rc, i, c, w, p;
    int total_tagged, mcount;
    void *msg;
    MBIt_TagTable *tt;
    pl_address_node *pl_itr;
    
    /* check that initial values are set properly */
    assert(node->stage == PRE_TAGGING);
    assert(node->flag_fdrFallback == MB_FALSE);
    assert(node->flag_shareOutbuf == MB_FALSE);
    assert(node->incount == NULL);
    assert(node->outcount == NULL);
    assert(node->inbuf == NULL);
    assert(node->outbuf == NULL);
    assert(node->recvreq == NULL);
    assert(node->sendreq == NULL);
    assert(node->board == NULL);

    /* get reference to board object and cache ptr in node */
    node->board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    assert(node->board != NULL);
    if (node->board == NULL) return MB_ERR_INVALID;
    
    /* check board state */
    assert(node->board->locked == MB_TRUE);
    assert(node->board->syncCompleted == MB_FALSE);
    
    /* get message count */
    mcount = (int)node->board->data->count_current;
    
    /* allocate memory for outcount */
    node->outcount = (int *)calloc((size_t)MBI_CommSize, sizeof(int)); 
    assert(node->outcount != NULL);
    if (node->outcount == NULL) return MB_ERR_MEMALLOC;
    
    /* determined number of messages to send to remote procs */
    if (mcount == 0 || MBI_CommSize == 1) /* nothing to send */
    {
        /* outcount already initialised to 0 (calloc) */
        /*for (i = 0; i < MBI_CommSize; i++) node->outcount[i] = 0;*/
    }
    else if (node->board->filter == (MBIt_filterfunc)NULL) /* no filter */
    {   
        /* send all messages to all procs (except self) */
        for (i = 0; i < MBI_CommSize; i++)
        {
            node->outcount[i] = (i == MBI_CommRank) ? 0 : mcount;
        }
        
        /* outgoing buffer can be shared */
        node->flag_shareOutbuf = MB_TRUE;
        
    }
    else /* filter assigned */
    {        
        /* create tag_table and assign to board */
        rc = tt_create(&tt, mcount, MBI_CommSize);
        assert(rc == TT_SUCCESS);
        if (rc != TT_SUCCESS)
        {
            if (rc == TT_ERR_MEMALLOC) return MB_ERR_MEMALLOC;
            else return MB_ERR_INTERNAL;
        }
        node->board->tt = tt; /* assign to board */
        
        /* initialise counters */
        i = 0;
        total_tagged = 0;
        
        /* loop thru messages and fill up tag table */
        for (pl_itr = PL_ITERATOR(node->board->data); pl_itr; pl_itr = pl_itr->next)
        {
            assert(i < mcount);
            
            /* get reference to message from iterator */
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
                    /* if message accepted by filter */
                    if (1 == (*node->board->filter)(msg, p))
                    {
                        /* set bit within our byte buffer */
                        SETBIT(window, c);
                        
                        /* update outcount */
                        node->outcount[p]++;
                        total_tagged++;

                    }
                }
                
                
                /* move index within window */
                c++;
                
                /* when window full, write to table and shift window */
                if (c == 8)
                {
                    /* write byte buffer to table */
                    rc = tt_setbyte(node->board->tt, i, w, window);
                    assert(rc == TT_SUCCESS);
                    
                    /* move window */
                    w += 1;
                    
                    /* reset byte buffer */
                    SETZEROS(window);
                    c = 0;
                }
            }
            
            /* write remaining byte buffer */
            if (w < (int)node->board->tt->row_size)
            {
                rc = tt_setbyte(node->board->tt, i, w, window);
                assert(rc == TT_SUCCESS);
            }
            
            /* increment counter */
            i++;
        }
        assert(node->outcount[MBI_CommRank] == 0);
        
        /* Should we fall back to full data replication? */
        if (total_tagged > mcount)
        {
            /* we don't need the tagtable any more */
            node->board->tt = NULL;
            rc = tt_delete(&tt);
            assert(rc == TT_SUCCESS);
            
            /* send all messages to all remote procs */
            node->flag_fdrFallback = MB_TRUE; /* fallback to full data replication */
            node->flag_shareOutbuf = MB_TRUE; /* use shared buffer */
            for (i = 0; i < MBI_CommSize; i++)
            {
                if (node->outcount[i] != 0) node->outcount[i] = mcount;
            }
        }
    }
    
    /* move on to next stage */
    node->stage = READY_FOR_PROP;
    return MB_SUCCESS;
    
}

/*! 
 * \brief Send out expected buffer sizes to all procs
 * \param[in] node address of CommQueue node
 * \return Return Code
 * 
 * This routine is to be executed during the ::READY_FOR_PROP stage.
 * 
 * Steps:
 * -# Allocate memory for node->incount
 * -# Allocate memory for node->recvreq
 * -# Issue MPI_Irecv from all remote procs
 * -# Set node->pending_in = 1
 * -# Allocate memory for node->sendreq
 * -# Issue MPI_Issend to all remote procs
 * -# Set node->pending_out = 1
 * -# Set node->stage == ::BUFINFO_SENT
 * 
 * Post:
 * - node->stage == ::BUFINFO_SENT
 * - node->incount != \c NULL
 * - node->outcount != \c NULL
 * - node->sendreq != \c NULL
 * - node->recvreq != \c NULL
 * - node->pending_out == 1
 * - node->pending_in == 1
 * 
 */ 
int MBI_Comm_SendBufInfo(struct MBIt_commqueue *node) {
    
    int i, rc, tag;
    
    assert(node->stage == READY_FOR_PROP);
    assert(node->outcount != NULL);
    assert(node->incount  == NULL);
    assert(node->board != NULL);
    
    /* allocate memory for incount */
    node->incount = (int*)malloc(MBI_CommSize * sizeof(int));
    assert(node->incount != NULL);
    if (node->incount == NULL) return MB_ERR_MEMALLOC;
    
    /* allocate memory for sendreq */
    node->sendreq = (MPI_Request *)malloc(MBI_CommSize * sizeof(MPI_Request));
    assert(node->sendreq != NULL);
    if (node->sendreq == NULL) return MB_ERR_MEMALLOC;
    
    /* allocate memory for recvreq */
    node->recvreq = (MPI_Request *)malloc(MBI_CommSize * sizeof(MPI_Request));
    assert(node->recvreq != NULL);
    if (node->recvreq == NULL) return MB_ERR_MEMALLOC;
    
    /* generate unique tag from this board */
    assert(node->mb <= MBI_TAG_BASE);
    tag = MBI_TAG_MSGDATA | node->mb;
    assert(tag < MBI_TAG_MAX);
    
    /* issue irecv from all remote procs */
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank)
        {
            node->incount[i] = 0;
            node->recvreq[i] = MPI_REQUEST_NULL;
        }
        else
        {
            rc = MPI_Irecv(&(node->incount[i]), 1, MPI_INT, i, tag, 
                    MBI_CommWorld, &(node->recvreq[i])); 
            assert(rc == MPI_SUCCESS);
            if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        }
    }

    
    /* issue issends to all remote procs */
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank)
        {
            node->sendreq[i] = MPI_REQUEST_NULL;
        }
        else
        {
            /* send send send... */
            rc = MPI_Issend(&(node->outcount[i]), 1, MPI_INT, i, tag, 
                    MBI_CommWorld, &(node->sendreq[i])); 
            assert(rc == MPI_SUCCESS);
            if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        }
    }
    
    node->pending_in  = 1;
    node->pending_out = 1;
    
    /* move on to next stage */
    node->stage = BUFINFO_SENT;
    return MB_SUCCESS;
    
}

/*! 
 * \brief Wait for all send/receives of bufinfo to complete
 * \param[in] node address of CommQueue node
 * \return Return Code
 *
 * This routine is to be executed during the ::BUFINFO_SENT stage.
 * 
 * Steps:
 * -# if node->pending_in != 0, MPI_Testall() receives
 *  - if completed, set node->pending_in = 0
 * -# if node->pending_out != 0, MPI_Testall() sends
 *  - if completed, set node->pending_out = 0
 * -# if node->pending_in == 0 and node->pending_out == 0
 *  - set node->stage = PRE_PROPAGATION
 * 
 * Post:
 * - if node->pending_in == 0 and node->pending_out == 0
 *  - node->stage == PRE_PROPAGATION
 * - else
 *  - node->stage == BUFINFO_SENT
 */ 
int MBI_Comm_WaitBufInfo(struct MBIt_commqueue *node) {
    
    int rc, flag;
    
    assert(node->stage == BUFINFO_SENT); 
    assert(node->outcount != NULL);
    assert(node->incount  != NULL);
    assert(node->sendreq  != NULL);
    assert(node->recvreq  != NULL);
    assert(node->board != NULL);
    
    /* check receives */
    if (node->pending_in != 0)
    {
        rc = MPI_Testall(MBI_CommSize, node->recvreq, &flag, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        
        if (flag) node->pending_in = 0;
    }
    
    /* check sends */
    if (node->pending_out != 0)
    {
        rc = MPI_Testall(MBI_CommSize, node->sendreq, &flag, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        
        if (flag) node->pending_out = 0;
    }
    
    /* if all done, move on to next stage */
    if (node->pending_in == 0 && node->pending_out == 0)
    {
        node->stage = PRE_PROPAGATION;
    }

    return MB_SUCCESS;
}

/*! 
 * \brief Start propagation of messages
 * \param[in] node address of CommQueue node
 * \return Return Code
 *
 * This routine is to be executed during the ::PRE_PROPAGATION stage.
 * 
 * Steps:
 * -# Allocate memory for node->inbuf (based on node->incount)
 * -# Issue MPI_Irecv() for each non-0 counts. node->pending_in++
 * -# Allocate memory for node->outbuf 
 * -# Set up non-blocking sends
 *  - If node->flag_shareOutbuf == ::MB_TRUE
 *   - Allocate memory for node->outbuf[0] + 1 byte for header
 *   - if node->board->filter != \c NULL Set delayed_filtering flag in header to ::MB_TRUE
 *   - if node->board->filter == \c NULL Set delayed_filtering flag in header to ::MB_FALSE
 *   - Issue MPI_Issend() to all remote procs. node->pending_out++
 *  - If node->flag_shareOutbuf == ::MB_FLASE
 *   - Ensure that node->board->filter != \c NULL and 
 *        node->flag_fdrFallback == ::MB_FALSE
 *   - For each remote node i, if node->outcount[i] != 0
 *    - Allocate memory for node->outbuf[i] + 1 byte for header
 *    - Set delayed_filtering flag in header to ::MB_FALSE
 *    - Copy tagged messages for proc i to buffer
 *    - delete node->board->tt
 *    - Issue MPI_Issend(). node->pending_out++
 * -# free node->outcount
 * -# Set node->stage == ::PROPAGATION
 * 
 * Post:
 * -# node->stage == ::PROPAGATION
 * -# node->outcount == \c NULL
 * -# node->outbuf != \c NULL
 * -# node->inbuf != \c NULL
 * -# node->board->tt == \c NULL
 * 
 */ 
int MBI_Comm_InitPropagation(struct MBIt_commqueue *node) {
  

    int mcount;
    int w, b, p;
    int i, rc, tag, bufloc;
    void *msg;
    char *outptr, *row;
    char *header_byte;
    char **loc;
    char window;
    size_t msgsize;
    pl_address_node *pl_itr;
    
#ifdef _EXTRA_CHECKS
    int *msg_copied;
    msg_copied = (int*)calloc((size_t)MBI_CommSize, sizeof(int));
#endif
    
    assert(node->stage == PRE_PROPAGATION);
    assert(node->outcount != NULL);
    assert(node->incount  != NULL);
    assert(node->sendreq  != NULL);
    assert(node->recvreq  != NULL);
    assert(node->pending_in  == 0);
    assert(node->pending_out == 0);
    assert(node->inbuf  ==  NULL);
    assert(node->outbuf ==  NULL);
    assert(node->board  != NULL);
    
    /* generate unique tag from this board */
    assert(node->mb <= MBI_TAG_BASE);
    tag = MBI_TAG_MSGDATA | node->mb;
    assert(tag < MBI_TAG_MAX);
    
    /* get message size and count */
    msgsize = node->board->data->elem_size;
    mcount  = (int)node->board->data->count_current;
    
    /* Allocate memory for input buffers */
    node->inbuf = (void **)malloc(sizeof(void*) * MBI_CommSize);
    assert(node->inbuf != NULL);
    if (node->inbuf == NULL) return MB_ERR_MEMALLOC;
    
    /* Allocate memory for output buffers */
    node->outbuf = (void **)malloc(sizeof(void*) * MBI_CommSize);
    assert(node->outbuf != NULL);
    if (node->outbuf == NULL) return MB_ERR_MEMALLOC;
    

    /* ------- issue receives --------- */
    
    assert(node->incount[MBI_CommRank] == 0);
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (node->incount[i] == 0)
        {
            /* no comms from this proc */
            node->inbuf[i]   = NULL;
            node->recvreq[i] = MPI_REQUEST_NULL;
        }
        else
        {
            /* allocate memory for input buffer */
            node->inbuf[i] = (void*)malloc(1 + (msgsize * node->incount[i]));
            assert(node->inbuf[i] != NULL);
            if (node->inbuf[i] == NULL) return MB_ERR_MEMALLOC;
            
            /* issue non-blocking receive */
            rc = MPI_Irecv(node->inbuf[i], 1 + (int)(msgsize * node->incount[i]), 
                    MPI_BYTE, i, tag, MBI_CommWorld, &(node->recvreq[i]));
            assert(rc == MPI_SUCCESS);
            if (rc != MPI_SUCCESS) return MB_ERR_MPI;
            
            /* increment counter */
            node->pending_in++;
        }
    }
    
    /* ----------- build output buffers ----------------- */
    
    for (i = 0; i < MBI_CommSize; i++) node->outbuf[i] = NULL;

    /* create output buffers and copy in messages */
    if (MBI_CommSize == 1 || mcount == 0)
    {
        /* nothing to do if only one proc or no messages */
    }
    else if (node->flag_shareOutbuf == MB_TRUE)
    {
        
        #ifdef _EXTRA_CHECKS
        /* if filter is assigned, buffer sharing only occurs during
         * fallback to full data replication 
         */
        
        if (node->board->filter != (MBIt_filterfunc)NULL && 
                node->board->data->count_current != 0)
        {
            assert(node->flag_fdrFallback == MB_TRUE);
            if (node->flag_fdrFallback != MB_TRUE) return MB_ERR_INTERNAL;
            
            for (i = 0; i< MBI_CommSize; i++) 
            {
                if (i == MBI_CommRank)
                {
                    assert(node->outcount[i] == 0);
                }
                else
                {
                    assert(node->outcount[i] == mcount || node->outcount[i] == 0);
                }
            }
        }
        #endif
        
        /* allocate shared buffer */
        node->outbuf[0] = (void*)malloc(1 + /* one byte for header info */
                                       (msgsize * mcount));
        assert(node->outbuf[0] != NULL);
        if (node->outbuf[0] == NULL) return MB_ERR_MEMALLOC;
        
        /* set header byte */
        header_byte = (char*)(node->outbuf[0]);
        *header_byte = ALLZEROS;
        if (node->flag_fdrFallback == MB_TRUE) /* set flag for FDR */
            *header_byte = *header_byte | MBI_COMM_HEADERBYTE_FDR;
        
        /* location of message buffer is one byte after header */
        outptr = (char*)(node->outbuf[0]) + 1;
        
        /* copy messages into output buffer */
        i = 0;
        for (pl_itr = PL_ITERATOR(node->board->data); pl_itr; pl_itr = pl_itr->next)
        {
            /* get reference to message object */
            msg = PL_NODEDATA(pl_itr);
            assert(msg != NULL);
            
            /* copy into buffer */
            memcpy(outptr + (i*msgsize), msg, msgsize);
            i++;
        }
        assert(i == mcount);
        
    }
    else /* messages are tagged */
    {
        assert(node->board->filter != (MBIt_filterfunc)NULL);
        assert(node->flag_fdrFallback == MB_FALSE);
        
        /* array of pointers to store next location in output buffer */
        loc = (char **)malloc(sizeof(char*) * MBI_CommSize);
        
        /* initialise output buffers */
        assert(node->outcount[MBI_CommRank] == 0);
        for (i = 0; i < MBI_CommSize; i++)
        {
            if (node->outcount[i] == 0) 
            {
                loc[i] = NULL;
            }
            else
            {
                /* allocate memory for output buffers */
                node->outbuf[i] = (void*)malloc(1 + (msgsize * node->outcount[i]));
                assert(node->outbuf[i] != NULL);
                if (node->outbuf[i] == NULL) return MB_ERR_MEMALLOC;
                
                /* set header byte */
                header_byte = (char*)(node->outbuf[i]);
                *header_byte = ALLZEROS;
                
                /* move loc to first message, after header */
                loc[i] = (char*)(node->outbuf[i]) + 1;
            }
        }
        
        /* copy in tagged messages */
        i = 0;
        for (pl_itr = PL_ITERATOR(node->board->data); pl_itr; pl_itr = pl_itr->next)
        {
            /* get reference to message object */
            msg = PL_NODEDATA(pl_itr);
            assert(msg != NULL);
            
            /* get ptr to row in tag table */
            rc = tt_getrow(node->board->tt, i, &row);
            assert(rc == TT_SUCCESS);
            assert(row != NULL);
            if (rc != TT_SUCCESS || row == NULL) return MB_ERR_INTERNAL;
            
            /* w: window index within row (in units of bytes)
             * b: bit index within window (in units of bits)
             * p: process (mpi task) represented by w&b
             */
            for (w = 0; w < (int)node->board->tt->row_size; w++)
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
                        assert(node->outcount[p] != 0);
                        assert(loc[p] != NULL);
                        
                        #ifdef _EXTRA_CHECKS
                        /* keep track of messages copied into each buffer */
                        msg_copied[p] ++;
                        assert(msg_copied[p] <= node->outcount[p]);
                        #endif
                        
                        /* copy message to appropriate output buffer */
                        memcpy(loc[p], msg, msgsize);
                        
                        /* move to next free location in buffer */
                        loc[p] += msgsize;
                    }
                    
                    /* shift bit and repeat */
                    window = window << 1;
                    b++;
                }
            }
            
            /* on to next message */
            i++;
        }
        assert(i == (int)node->board->data->count_current);
        free(loc);
        
        /* tag table no longer needed */
        rc = tt_delete(&(node->board->tt));
        assert(rc == TT_SUCCESS);
        assert(node->board->tt == NULL);
        
        #ifdef _EXTRA_CHECKS
        for (i = 0; i < MBI_CommSize; i++)
        {
            assert(msg_copied[i] == node->outcount[i]);
        }
        #endif
    }
    
    
    /* ----------- issue sends ----------------- */
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (node->outcount[i] == 0)
        {
            node->sendreq[i] = MPI_REQUEST_NULL;
        }
        else
        {
            /* choose output bufer */
            bufloc = (node->flag_shareOutbuf == MB_TRUE) ? 0 : i;
            assert(node->outbuf[bufloc] != NULL);
            
            /* issue non-blocking send */
            rc = MPI_Issend(node->outbuf[bufloc], 
                    1 + (int)(node->outcount[i] * msgsize), 
                    MPI_BYTE, i, tag, MBI_CommWorld, &(node->sendreq[i]));
            assert(rc == MPI_SUCCESS);
            if (rc != MPI_SUCCESS) return MB_ERR_MEMALLOC;
            
            /* increment counter */
            node->pending_out++;
        }

    }
    

    #ifdef _EXTRA_CHECKS
    free(msg_copied);
    #endif
    
    /* outcount no longer needed */
    free(node->outcount);
    node->outcount = NULL;
    
    /* move on to next stage */
    node->stage = PROPAGATION;
    return MB_SUCCESS;
}

/*! 
 * \brief Complete the message propagation process
 * \param[in] node address of CommQueue node
 * \return Return Code
 *
 * This routine is to be executed during the ::PROPAGATION stage.
 * 
 * Steps:
 * -# if node->pending_in != 0, check receives using MPI_Testany().
 *    For each completed comm:
 *  -# Decrement node->pending_in
 *  -# Check buffer header if delayed_filtering is set
 *   - If set, run each message in buffer through node->board->filter
 *     before adding to local board
 *   - If not set, add all messages in buffer to local board
 *  -# Free node->inbuf[i]
 * 
 * -# if node->pending_out != 0, check sends using MPI_Testany().
 *    For each completed comm:
 *  -# Decrement node->pending_out
 *  -# if node->flag_shareOutbuf == ::MB_FALSE, free node->outbuf[i]
 *  -# if node->flag_shareOutbuf == ::MB_TRUE, free node->outbuf[0] if
 *     node->pending_out == 0
 *
 * -# Check if comms completed?
 *  - if node->pending_in == 0 and node->pending_out == 0
 *   -# free node->incount
 *   -# free node->inbuf
 *   -# free node->outbuf
 *   -# free node->sendreq
 *   -# free node->recvreq
 *   -# Capture node->board->syncLock
 *   -# set node->board->syncCompleted = ::MB_TRUE
 *   -# Release node->board->syncLock
 *   -# Signal node->board->syncCond
 *   -# set node->stage = ::COMM_END
 *   -# return ::MB_SUCCESS_2
 *  - else
 *   -# return ::MB_SUCCESS
 * 
 * 
 * 
 * Post:
 * - if node->pending_in == 0 and node->pending_out == 0
 *  - node->incount == \c NULL
 *  - node->inbuf == \c NULL
 *  - node->outbuf == \c NULL
 *  - node->sendreq == \c NULL
 *  - node->recvreq == \c NULL
 *  - return code == MB_SUCCESS_2
 *  - node->board->syncCompleted == ::MB_TRUE
 *  - node->stage == ::COMM_END
 */ 
int MBI_Comm_CompletePropagation(struct MBIt_commqueue *node) {
    
    int i, m, p, rc;
    int completed;
    int filter_required;
    void *ptr_new, *msg;
    char *header_byte, *bufptr;

    
    assert(node->stage == PROPAGATION);
    assert(node->outcount == NULL);
    assert(node->incount  != NULL);
    assert(node->sendreq  != NULL);
    assert(node->recvreq  != NULL);
    assert(node->inbuf    !=  NULL);
    assert(node->outbuf   !=  NULL);
    assert(node->board    != NULL);
    
    
    /* ---------- check for completed sends -------------- */
    if (node->pending_out > 0)
    {
        /* check if any of the sends completed */
        rc = MPI_Testsome(MBI_CommSize, node->sendreq, &completed,
                MBI_comm_indices, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        
        if (completed > 0)
        {
            /* decrement counter */
            node->pending_out -= completed;

            if (node->flag_shareOutbuf == MB_FALSE)
            {
                assert(node->flag_fdrFallback == MB_FALSE);
                
                /* free buffer of completed sends */
                for (p = 0; p < completed; p++) 
                {
                    i = MBI_comm_indices[p];
                    assert(i != MBI_CommRank);
                    assert(node->outbuf[i] != NULL);
                    assert(node->sendreq[i] == MPI_REQUEST_NULL);
                    
                    free(node->outbuf[i]);
                    node->outbuf[i] = NULL;
                }
            }
            else if (node->pending_out == 0) /* outbuf shared */
            {
                assert(node->outbuf[0] != NULL);
                assert(node->flag_fdrFallback == MB_TRUE || 
                        node->board->filter == (MBIt_filterfunc)NULL);
                
                /* free shared buffer */
                free(node->outbuf[0]);
                node->outbuf[0] = NULL;
            }
        }
    }
    
    /* ---------- check for completed receives -------------- */
    if (node->pending_in > 0)
    {
        /* check if any of the sends completed */
        rc = MPI_Testsome(MBI_CommSize, node->recvreq, &completed,
                MBI_comm_indices, MPI_STATUSES_IGNORE);
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
        
        if (completed > 0)
        {
            /* decrement counter */
            node->pending_in -= completed;
            
            /* for each completed receive, load messages and clear buffer */
            for (p = 0; p < completed; p++)
            {
                /* which receive completed? */
                i = MBI_comm_indices[p];
                
                assert(node->inbuf[i] != NULL);
                assert(node->recvreq[i] == MPI_REQUEST_NULL); 
                
                /* get reference to header byte */
                header_byte = (char*)(node->inbuf[i]);
                
                /* get flag indicating if filter should be run */
                filter_required = BIT_IS_SET(*header_byte, MBI_COMM_HEADERBYTE_FDR);                 
                
                /* location of message buffer is after header (of size 1 byte) */
                bufptr = (char*)(node->inbuf[i]) + 1;
                
                /* for each received message */
                for (m = 0; m < node->incount[i]; m++)
                {
                    /* get pointer to message in buffer */
                    msg = (void*)(bufptr + (node->board->data->elem_size * m));
                    
                    /* do we need to run msg thru filter before storing? */
                    if (filter_required)
                    {
                        assert(node->board->filter != (MBIt_filterfunc)NULL);
                        if (0 == (*node->board->filter)(msg, MBI_CommRank))
                           continue; /* we don't want this message */
                    }
                    
                    /* add new node to local board */
                    rc = pl_newnode(node->board->data, &ptr_new);
                    assert(rc == PL_SUCCESS);
                    /* copy message into node */
                    memcpy(ptr_new, msg, node->board->data->elem_size);

                }
                
                /* we can now free the buffer */
                free(node->inbuf[i]);
                node->inbuf[i] = NULL;
            }
        }
    }
    

    
    /* ------------ if all comms completed, clean up and end ------------ */
    
    if (node->pending_in == 0 && node->pending_out == 0)
    {
        /* free up memory */
        free(node->incount);  node->incount = NULL;
        free(node->inbuf);    node->inbuf = NULL;
        free(node->outbuf);   node->outbuf = NULL;
        free(node->sendreq);  node->sendreq = NULL;
        free(node->recvreq);  node->recvreq = NULL;
        
        /* capture lock for board */
        rc = pthread_mutex_lock(&(node->board->syncLock));
        assert(0 == rc);
        
        /* mark sync as completed */
        node->board->syncCompleted = MB_TRUE;
        
        /* release lock */
        rc = pthread_mutex_unlock(&(node->board->syncLock));
        assert(0 == rc);
        
        /* send signal to wake main thread waiting on this board */
        rc = pthread_cond_signal(&(node->board->syncCond));
        assert(0 == rc);
        
        /* move to end state and indicate that we're done */
        node->stage = COMM_END;
        return MB_SUCCESS_2; /* node can be removed from queue */
    }
    else
    {
        /* there are still pending comms. No state change  */
        return MB_SUCCESS;
    }

}
