/* $Id: iterator_create.c 2108 2009-09-14 14:14:31Z lsc $ */
/*!
 * \file parallel/iterator_create.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Iterator_Create()
 * 
 */

#include "mb_parallel.h"
#include <string.h> /* for memcpy() */

/*!
 * \brief Creates a new Iterator for accessing a MessageBoard
 * \ingroup MB_API
 * \param[in] mb MessageBoard handle
 * \param[out] itr_ptr Address of Iterator Handle
 * 
 * The Iterator object is allocated and populated with references to 
 * messages in the target MessageBoard. The Iterator is then registered 
 * with the ::MBI_OM_iterator and the reference ID returned by ObjectMap 
 * is then written to \c itr_ptr as the handle.
 * 
 * References to messages are stored within a pooled_list and traversed as 
 * a linked list. The memory block size for the pooled_list is chosen to
 * be slightly larger than the number of message so that only one block
 * is allocated.
 * 
 * Message references are stored in the same order they appear in the board.
 * 
 * \note We only store pointers to message objects within the Iterator, and
 *       not the actual messages. These pointers will be invalid if the 
 *       MessageBoard is deleted of modified. Checking the validity of 
 *       messages each time it is accessed would be too great an overhead.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 *  - ::MB_ERR_INVALID (invalid or null board given) 
 *  - ::MB_ERR_MEMALLOC (error allocating memory for Iterator object or pooled_list)
 *  - ::MB_ERR_LOCKED (\c mb is locked)
 *  - ::MB_ERR_INTERNAL (possible bug. Recompile and run in debug mode for hints)
 *  - ::MB_ERR_OVERFLOW (Too many Iterators created.)
 *  - ::MB_ERR_DISABLED (access mode of board set to non-readable)
 */
int MB_Iterator_Create(MBt_Board mb, MBt_Iterator *itr_ptr) {
    
    int rc, mcount;
    OM_key_t rc_om;
    void *obj;
    void *new;
    MBIt_Board *board;
    MBIt_Iterator *iter;
    pl_address_node *pl_itr;
    
	/* Check for NULL message board */
    if (mb == MB_NULL_MBOARD) 
    {
        P_FUNCFAIL("Cannot create iterator for null board (MB_NULL_MBOARD)");
        return MB_ERR_INVALID;
    }
    
    /* get ptr to board */
    board = (MBIt_Board*)MBI_getMBoardRef(mb);
    if (board == NULL) 
    {
        P_FUNCFAIL("Invalid board handle (%d)", (int)mb);
        return MB_ERR_INVALID;
    }
    
    /* check if board is locked */
    if (board->locked == MB_TRUE) 
    {
        P_FUNCFAIL("Board (%d) is locked", (int)mb);
        return MB_ERR_LOCKED;
    }
    
    /* check if board is "unreadable" */
    if (board->is_reader == MB_FALSE)
    {
        P_FUNCFAIL("Board access mode was set to non-readable");
        return MB_ERR_DISABLED;
    }
    
    /* get message count */
    mcount = (int)board->data->count_current;
    
    /* Allocate Iterator object */
    iter = (MBIt_Iterator*)malloc(sizeof(MBIt_Iterator));
    assert(iter != NULL);
    if (iter == NULL) 
    {
        P_FUNCFAIL("Could not allocate required memory");
        return MB_ERR_MEMALLOC;
    }
    
    /* assign mb handle to iterator */
    iter->mb        = mb;
    iter->msgsize   = board->data->elem_size;
    iter->cursor    = NULL;
    iter->iterating = 0;
    
    /* allocate memory for address list */
    rc = pl_create(&(iter->data), sizeof(void *), mcount + 1);
    if (rc != PL_SUCCESS)
    {
        free(iter);
        if (rc == PL_ERR_MALLOC) 
        {
            P_FUNCFAIL("Could not allocate required memory");
            return MB_ERR_MEMALLOC;
        }
        else 
        {
            P_FUNCFAIL("pl_create() returned with err code %d", rc);
            return MB_ERR_INTERNAL;
        }
    }
    
    /* populate iterator */
    assert(board->data != NULL);
    for (pl_itr = PL_ITERATOR(board->data); pl_itr; pl_itr = pl_itr->next)
    {
        /* get reference to message object within board */
        obj = PL_NODEDATA(pl_itr);
        assert(obj != NULL);
        
        /* copy object address to as new data entry in Iterator */
        rc = pl_newnode(iter->data, &new);
        assert(new != NULL);
        memcpy(new, &obj, sizeof(void *));
    }
    
    /* register iter object */
    rc_om = MBI_objmap_push(MBI_OM_iterator, (void*)iter);
    if (rc_om > OM_MAX_INDEX)
    {
        if (rc_om == OM_ERR_MEMALLOC)
        {
            P_FUNCFAIL("Could not allocate required memory");
            return MB_ERR_MEMALLOC;
        }
        else if (rc_om == OM_ERR_OVERFLOW)
        {
            P_FUNCFAIL("Too many iterators created. Objmap key overflow");
        	return MB_ERR_OVERFLOW;
        }
        else
        {
            P_FUNCFAIL("MBI_objmap_push() returned with err code %d", rc);
            return MB_ERR_INTERNAL;
        }
    }
    
    /* assign return pointer */
    *itr_ptr  = (MBt_Iterator)rc_om;
    
    P_INFO("Iterator created (iter:%d, board:%d, mcount:%d)", 
            (int)rc_om, (int)mb, mcount);
    
    return MB_SUCCESS;
}
