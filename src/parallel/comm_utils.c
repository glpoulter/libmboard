/* $Id$ */
/*!
 * \file parallel/comm_utils.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Aug 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Utilities routine for building different communication strategies.
 * 
 */
#include "mb_parallel.h"
#include "mb_commqueue.h"
#include "mb_commroutines.h"
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
 * \param[in,out] board Message Board object
 * \param[out] outcount Array of number of messages tagged per proc
 * \return Return Code
 * 
 * 
 * Create tag_table object \c board->tt and tag messages in board. The
 * number of messages tagged for each remote proc is written into 
 * the \c outcount array. \c board->synced_cursor set to 
 * \c board->data->count_current.
 * 
 * All procs not listed in \c board->reader_list will be given an \c outcount
 * value of -1. If <tt>\c board->is_writer == MB_FALSE</tt>, all entries
 * in \c outcount will be set to -1 to indicate that this board is not
 * involved in sending out messages.
 * 
 * This routine expects readers in \c board->reader_list to be sorted in 
 * an ascending order.
 * 
 * If the total number of tagged messages exceed the number of messages
 * in board, we fall back to full data replication. \c board->tt will 
 * not be created (\c NULL) and all readers will get an \c outcount 
 * value equal to the total number of messages. This routine then returns 
 * with ::MB_SUCCESS_2.
 * 
 * If there are no filters assigned to the board (<tt>\c board->filter == 
 * NULL</tt>), tagging is not performed. Instead, \c outcount is set to
 * to the number of new messages for readers and -1 for non readers. It
 * then returns turns with ::MB_SUCCESS. \c board->tt will remain \c NULL;  
 * 
 * Test cases (repeated with different configurations of 
 * \c board->reader_list where relevant):
 * 
 * -# Board is not a writer (<tt>board->is_writer == MB_FALSE</tt>) 
 *  - <tt>board->tt == NULL</tt>
 *  - All values in \c outcount should be \c -1
 *  - returns ::MB_SUCCESS
 * 
 * -# No readers (<tt>board->reader_count == 0</tt>)
 *  - <tt>board->tt == NULL</tt>
 *  - All values in \c outcount should be \c -1
 *  - returns ::MB_SUCCESS
 * 
 * -# Empty board
 *  - <tt>board->tt == NULL</tt>
 *  - \c outcount should be \c 0 for readers and \c -1 for non readers
 *  - returns ::MB_SUCCESS
 * 
 * -# Board with filtered content
 *  - <tt>board->tt != NULL</tt>
 *  - \c outcount depends on filter, but must be \c -1 for non readers
 *  - returns ::MB_SUCCESS
 * 
 * -# Fallback to full data replication 
 *  - <tt>board->tt == NULL</tt>
 *  - \c outcount equals to \c board->data->count_current for readers, and
 *    \c -1 for non readers
 *  - returns ::MB_SUCCESS_2
 * 
 * -# Board with non-zero synced_cursor set (recently synced)
 *  - <tt>board->tt != NULL</tt>
 *  - \c board->tt only contains messages beyond synced_cursor
 *  - \c outcount depends on filter, but must be \c -1 for non readers
 *  - returns ::MB_SUCCESS
 * 
 * -# Board without filter
 *  - <tt>board->tt == NULL</tt>
 *  - \c outcount should be board->data->count_current for readers 
 *    and \c -1 for non readers
 *  - returns ::MB_SUCCESS
 * 
 * -# Board without filter second sync
 *  - <tt>board->tt == NULL</tt>
 *  - \c outcount should be board->data->count_current - board synced_cursor
 *    for readers and \c -1 for non readers
 *  - returns ::MB_SUCCESS
 */
int MBI_CommUtil_TagMessages(MBIt_Board *board, int *outcount) {
    
    char window;
    int c, w, p, p_index, i;
    int rc, msg_index, mcount;
    int tagged;
    void *msg;
    MBIt_TagTable *tt;
    pl_address_node *pl_itr;

    /* sanity checks */
    assert(outcount != NULL);
    assert(board    != NULL);
    assert(board->data   != NULL);
    assert(board->tt     == NULL);
    assert(board->reader_list != NULL);
    assert(board->writer_list != NULL);
    
    /* conditions that must be met by non-writers */
    assert(board->is_writer == MB_TRUE || board->synced_cursor == 0);
    assert(board->is_writer == MB_TRUE || board->data->count_current == 0);
    
    /* reset outcount */
    for (i = 0; i < MBI_CommSize; i++) outcount[i] = -1;
    for (i = 0; i < board->reader_count; i++) 
        outcount[board->reader_list[i]] = 0;
    
    /* no readers, we're done! */ 
    if (board->reader_count == 0)  return MB_SUCCESS;
    
    /* number of new messages */
    mcount = (int)board->data->count_current - (int)board->synced_cursor;
    
    /* no new messages to sync, we're done! */
    if (mcount == 0) return MB_SUCCESS;

    /* not much to do if there is no assigned filter */
    if (board->filter == NULL)
    {
        for (i = 0; i < board->reader_count; i++)
            outcount[board->reader_list[i]] = mcount;
        return MB_SUCCESS;
    }
    
    /* ------------- let the tagging begin! -------------- */
    
    /* reset outcount */
    
    /* create tag table object */
    rc = tt_create(&tt, mcount, MBI_CommSize);
    assert(rc == TT_SUCCESS);
    if (rc != TT_SUCCESS)
    {
        if (rc == TT_ERR_MEMALLOC) return MB_ERR_MEMALLOC;
        else return MB_ERR_INTERNAL;
    }
    
    /* get iterator for messages */
    if (board->synced_cursor == 0) 
    {
        /* start from beginning of message list */
        pl_itr = PL_ITERATOR(board->data);
    }
    else 
    {
        /* jump straight to position specified by synced_cursor */
        rc = pl_getnode_container(board->data, (int)board->synced_cursor, &pl_itr);
        assert(rc == PL_SUCCESS);
    }
    assert(pl_itr != NULL);
    
    /* start iterating thru messages */
    tagged = msg_index = 0;
    
    while(pl_itr)
    {
        msg = PL_NODEDATA(pl_itr);
        assert(msg != NULL);
        pl_itr = pl_itr->next;
        
        /* 
         * c : offset within byte buffer (window)
         * w : window offset within table row
         */
        c = w = 0;
        SETZEROS(window);
        
        p = 0;
        for (i = 0; i < MBI_CommSize; i++)
        {
            /* this assertion will be tripped if board->reader_list is 
             * not sorted in ascending order as required
             */
            assert(p == board->reader_count || i <= board->reader_list[p]);
            assert(p <= board->reader_count);
            
            /* p_index == -1 when i is not a reader, p_index == i otherwise */
            if (p == board->reader_count || i < board->reader_list[p]) p_index = -1;
            else p_index = board->reader_list[p++]; /* i == board->reader_list[p] */

            assert(p_index >= -1 && p_index < MBI_CommSize);
            assert(p_index != MBI_CommRank);
            
            /* if this proc is a reader, run filter on msg */
            if (p_index != -1 && 1 == (*board->filter)(msg, p_index))
            {
                SETBIT(window, c);
                outcount[p_index]++;
                tagged++;
            }
            c++; /* move index within window */
            
            /* when window full, write to table and shift window */
            if (c == 8)
            {
                /* write byte buffer to table */
                rc = tt_setbyte(tt, msg_index, w, window);
                assert(rc == TT_SUCCESS);

                w += 1; /* move window */
                
                /* reset byte buffer */
                SETZEROS(window);
                c = 0;
            }
        }
        
        /* if the number of tagged messages exceed that of mcount,
         * we will end up using more memory for send buffers than 
         * it takes to store all messages. Fall back to full data replication!
         */ 
        if (tagged > mcount)
        {
           rc = tt_delete(&tt);
           assert(rc == TT_SUCCESS);
           
           for (i = 0; i < board->reader_count; i++) 
               outcount[board->reader_list[i]] = mcount;

           board->tt = NULL;
           return MB_SUCCESS_2;
        }
        
        /* write remaining byte buffer */
        if (w < (int)tt->row_size)
        {
            rc = tt_setbyte(tt, msg_index, w, window);
            assert(rc == TT_SUCCESS);
        }
        
        /* increment message index */
        msg_index ++;
    }
    assert(msg_index == mcount);
    
    /* assign tag table to board */
    board->tt = tt; 
    return MB_SUCCESS;
}


/*! 
 * \brief Allocate a send buffer and populate with all new messages
 * \param[in] board Message Board object
 * \param[out] bufptr Address to write pointer of new buffer
 * \param[in] flag_fdr Flag indicating if FDR flag should be set
 * \param[out] size Size of buffer allocated
 * \return Return Code
 * 
 * This routine should only be called when there are new messages to sync
 * (<tt>board->data->count_current > board->synced_cursor</tt>)
 * 
 * This routine will allocate a buffer large enought to store all 
 * new messages (beyond \c board->synced_cursor) plus an extra header byte.
 * The address of the allocated memory is written to \c bufptr, and the size
 * written to \c size.
 * 
 * The relevant messages will be copied into the buffer, and the appropriate
 * flags will be set in the buffer header depending on the contents of
 * \c flag_fdr
 * 
 * If an error occurs, an appropriate error code is returned, \c size is set 
 * to \c 0, and \c bufptr set to \c NULL.
 * 
 * Test cases:
 * -# <tt>board->synced_cursor == 0, board->data->count_current > 0</tt>
 *  - buffer is allocated and populated with \c board->data->count_current
 *    messages
 *  - buffer header is set to 0x00
 *  - \c size should be <tt>(board->data->count_current * 
 *    board->data->elem_size) + 1</tt>
 *  - ::MB_SUCCESS returned
 * 
 * -# <tt>0 < board->synced_cursor < board->data->count_current</tt>
 *  - buffer is allocated and populated with <tt>board->data->count_current -
 *    board->synced_cursor</tt> messages
 *  - buffer header is set to 0x00
 *  - \c size should be <tt>((board->data->count_current - board->synced_cursor) * 
 *    board->data->elem_size) + 1</tt>
 *  - ::MB_SUCCESS returned
 * 
 * -# <tt>flag_fdr == MB_TRUE</tt>
 *  - Same as above, bit with relevant flag set in byte header
 * 
 */
int MBI_CommUtil_BuildBuffer_All(MBIt_Board *board, void **bufptr, 
                                 unsigned int flag_fdr, size_t *size) {
    
    size_t bufsize, msgcount;
    int rc; 
    void *buf;
    void *msg;
    char *current;
    pl_address_node *pl_itr;
    
    assert(board != NULL);
    assert(board->data != NULL);
    assert(board->data->count_current != 0);
    assert(board->data->count_current != board->synced_cursor);
    
    /* initialise return values in case of error conditions */
    *size = 0;
    *bufptr = NULL;
   
    msgcount = board->data->count_current - board->synced_cursor;
    bufsize  = (msgcount * board->data->elem_size) + 1;
    assert(msgcount > 0);
    assert(bufsize > 1);
    
    buf = malloc(bufsize);
    assert(buf != NULL);
    if (buf == NULL) return MB_ERR_MEMALLOC;
    
    /* set buffer header */
    current = (char*)buf;
    *current = ALLZEROS;
    if (flag_fdr == MB_TRUE) *current = *current | MBI_COMM_HEADERBYTE_FDR;
    
    /* move current ptr to beginning of data block */
    current += 1;
    
    /* iterate thru board data and copy into buffer */
    if (board->synced_cursor == 0)
    {
        pl_itr = PL_ITERATOR(board->data);
    }
    else
    {
        rc = pl_getnode_container(board->data, (int)board->synced_cursor, &pl_itr);
        assert(rc == PL_SUCCESS);
    }
    assert(pl_itr != NULL);

    while(pl_itr)
    {
        msg = PL_NODEDATA(pl_itr);
        assert(msg != NULL);
        pl_itr = pl_itr->next;
        
        memcpy((void*)current, msg, (size_t)board->data->elem_size);
        current += board->data->elem_size;
    }
    
    /* set return values and we're done! */
    *bufptr = buf;
    *size   = bufsize;
    return MB_SUCCESS;
}

/*! 
 * \brief Allocate send buffers and populate with messages tagged for a recipient
 * \param[in] board Message Board object
 * \param[out] bufarray Array of buffers
 * \param[in] outcount Outcount array generated by MBI_CommUtil_TagMessages()
 * \return Return Code
 * 
 * This routine should only be called when there are new messages to sync
 * (<tt>board->data->count_current > board->synced_cursor</tt>)
 * 
 * This routine must only be called on boards that have an assigned tag table.
 * It also expects that bufarray has already been allocated with a array of
 * ::MBI_CommSize pointers (all set to \c NULL).
 * 
 * For each reader that expects a message (<tt>outcount[p]>0</tt>), memory is 
 * allocated and assigned to \c bufarray[p]. Non-readers will be assigned with
 * \c NULL in place of buffers. Tagged messages are then copied into the relevant 
 * buffers.
 * 
 * The header for each allocated buffer is set to \c 0x00.
 * 
 * Test cases:
 * 
 * -# All remote procs are readers
 *  - all remote procs get a non \c NULL buffer populated with relevant
 *    messages and a buffer header of 0x00
 *  - local proc gets a \c NULL buffer
 *  - ::MB_SUCCESS returned
 * 
 * -# Some remote procs are non-readers
 *  - all readers get a non \c NULL buffer populated with relevant
 *    messages and a buffer header of 0x00
 *  - non-readers get a \c NULL buffer
 *  - ::MB_SUCCESS returned
 */
int MBI_CommUtil_BuildBuffers_Tagged(MBIt_Board *board, void **bufarray, 
                                     int *outcount) {
    
    int i, j, rc;
    int w, b, p;
    void *msg;
    char *row;
    char **loc;
    char window;
    pl_address_node *pl_itr;
    
#ifdef _EXTRA_CHECKS
    int n_tt, n_outcount;
    int *copied;
#endif
    
    assert(board != NULL);
    assert(board->data != NULL);
    assert(board->tt   != NULL);
    assert(bufarray    != NULL);
    assert(outcount    != NULL);
    assert(board->data->count_current != 0);
    assert(board->data->count_current != board->synced_cursor);
    
#ifdef _EXTRA_CHECKS
    /* check values in outcount[] matches number of bits set in tag table */
    for (i = 0; i < MBI_CommSize; i++)
    {
        /* get num of messages for proc i from outcount[] */
        n_outcount = (outcount[i] == -1) ? 0 : outcount[i];
        /* get num of messages for proc i from outcount[] */
        rc = tt_getcount_col(board->tt, i, &n_tt);
        assert(rc == TT_SUCCESS);
        if (n_tt != n_outcount) printf("*** %d %d %d\n", i, n_tt, n_outcount);
        assert(n_tt == n_outcount);
    }
    
    /* values in board->reader_list must be sorted in ascending order */
    for (i = 1; i < board->reader_count; i++)
    {
        assert(board->reader_list[i] > board->reader_list[i-1]);
    }

    copied = (int*)calloc((size_t)MBI_CommSize, sizeof(int));
    assert(copied != NULL);
    if (copied == NULL) return MB_ERR_MEMALLOC;
#endif
    
    loc = (char**)malloc(sizeof(char*) * MBI_CommSize);
    assert(loc != NULL);
    if (loc == NULL) return MB_ERR_MEMALLOC;

    
    /* allocate memory for buffer where outcount is not 0 or -1 */
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (outcount[i] <= 0)
        {
            loc[i]   = NULL;
            bufarray[i] = NULL;
        }
        else
        {
            bufarray[i] = malloc((size_t)(1 + /* extra byte for buf header */
                                 (outcount[i] * board->data->elem_size))); 
            assert(bufarray[i] != NULL);
            if (bufarray[i] == NULL)
            {
                for (j = 0; j < i; j++) 
                {
                    free(bufarray[j]);
                    bufarray[i] = NULL;
                }
                free(loc);
                return MB_ERR_MEMALLOC;
            }
            loc[i]  = (char*)bufarray[i];
            *loc[i] = ALLZEROS; /* zero header bits */
            loc[i] += 1; /* skip over header. Now points to data block */
        }
    }
   
    /* iterate thru board data and copy into buffer */
    if (board->synced_cursor == 0)
    {
        pl_itr = PL_ITERATOR(board->data);
    }
    else
    {
        rc = pl_getnode_container(board->data, (int)board->synced_cursor, &pl_itr);
        assert(rc == PL_SUCCESS);
    }
    assert(pl_itr != NULL);
    
    i = 0;
    while(pl_itr)
    {
        msg    = PL_NODEDATA(pl_itr);
        assert(msg != NULL);
        pl_itr = pl_itr->next;
        
        /* get ptr to row in tag table */
        rc = tt_getrow(board->tt, i, &row);
        assert(rc == TT_SUCCESS);
        assert(row != NULL);
        
        /* w: window index within row (in units of bytes)
         * b: bit index within window (in units of bits)
         * p: process (mpi task) represented by w&b
         */
        for (w = 0; w < (int)board->tt->row_size; w++)
        {
            b = 0;
            window = *(row + w);
            
            while (window != ALLZEROS)
            {
                if (MSB_IS_SET(window))
                {
                    /* determine which MPI task this refers to */
                    p = (w * 8) + b;
                    
#ifdef _EXTRA_CHECKS
                    assert(p >= 0 && p < MBI_CommSize);
                    assert(outcount[p] > 0);
                    assert(loc[p] != NULL);
                    
                    copied[p]++;
                    assert(copied[p] <= outcount[p]);
#endif
                    
                    /* copy message into buffer then move ptr 
                     * to next available position 
                     */
                    memcpy(loc[p], msg, board->data->elem_size);
                    loc[p] += board->data->elem_size;
                }
                
                window = window << 1;
                b++;
            }
        }
        
        i++;
    }
    assert(i == (int)board->data->count_current - (int)board->synced_cursor);
    
    free(loc);
#ifdef _EXTRA_CHECKS
    free(copied);
#endif
    
    return MB_SUCCESS;
}

/*! 
 * \brief Load a received buffer and populate the local board
 * \param[in] board Message Board object
 * \param[out] buf Address of buffer
 * \param[out] size Size of buffer
 * \return Return Code
 * 
 * This routine loads messages for a received buffer (\c buf) and adds 
 * then to the local board. If the ::MBI_COMM_HEADERBYTE_FDR flag in the
 * buffer head is set, the messages are first filtered before being 
 * added to the local board.
 * 
 * Test cases:
 * -# ::MBI_COMM_HEADERBYTE_FDR is set
 * -# ::MBI_COMM_HEADERBYTE_FDR is not set
 */
int MBI_CommUtil_LoadBuffer(MBIt_Board *board, void *buf, size_t size) {
    
    int i, rc;
    int mcount;
    int filter_required;
    char *ptr;
    void *msg, *ptr_new;
    
    assert(board != NULL);
    assert(buf != NULL);
    assert(size > 1);
    
    ptr = (char*) buf;
    filter_required = BIT_IS_SET(*ptr, MBI_COMM_HEADERBYTE_FDR);
    ptr += 1;
    
#ifdef _EXTRA_CHECKS
    if (filter_required)
    {
        assert(board->filter != NULL);
    }
#endif
    
    mcount = (int)((size - 1) / board->data->elem_size);
    for (i = 0; i < (int)mcount; i++)
    {
        msg = (void *)ptr;
        ptr += board->data->elem_size;
        
        if (filter_required && 0 == (*(board->filter))(msg, MBI_CommRank)) 
            continue; /* discard message */
        
        /* add msg to board */
        rc = pl_newnode(board->data, &ptr_new);
        assert(rc == PL_SUCCESS);
        memcpy(ptr_new, msg, board->data->elem_size);
    }
    
    return MB_SUCCESS;
}

/*! 
 * \brief Updates cursor in board to indicate last point of sync
 * \param[in] board Message Board object
 * \return Return Code
 * 
 * This routine updates \c board->synced_cursor to point to
 * the end of the message list.
 * 
 * It is a very simple function, and arguably should not be a function at all.
 * However, it was included mainly as a subtle reminded to comm routine authors
 * to move the cursor after each sync.
 * 
 * This routine always returns ::MB_SUCCESS. If an error condition occurs, it 
 * will be caught by a run-time assertion.
 * 
 * Test cases:
 * 
 * -# Empty board
 *  - No change. \c synced_cursor remains at \c 0
 *  - ::MB_SUCCESS returned
 * 
 * -# Standard update
 *  - \c synced_cursor updated to \c board->data->count_current
 *  - ::MB_SUCCESS returned
 * 
 * -# Second sync, with no new messages
 *  - \c synced_cursor remains at \c board->data->count_current
 *  - ::MB_SUCCESS returned
 * 
 * -# Second sync, with  new messages
 *  - \c synced_cursor updated to \c board->data->count_current
 *  - ::MB_SUCCESS returned
 * 
 */
int MBI_CommUtil_UpdateCursor(MBIt_Board *board) {
    
    assert(board != NULL);
    assert(board->data != NULL);
    assert(board->synced_cursor <= board->data->count_current);
    
    board->synced_cursor = board->data->count_current;
    return MB_SUCCESS;
}
