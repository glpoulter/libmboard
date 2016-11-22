/* $Id: iterator_getmessage.c 1930 2009-07-02 13:47:53Z lsc $ */
/*!
 * \file parallel/iterator_getmessage.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Iterator_GetMessage()
 * 
 */

#include "mb_parallel.h"
#include <string.h>

/* Get next message from iterator */
/*!
 * \fn MB_Iterator_GetMessage(MBt_Iterator itr, void **msg_ptr)
 * \ingroup MB_API
 * \brief Returns next available message from Iterator
 * \param[in] itr Iterator Handle
 * \param[out] msg_ptr Address where reference to message will be written to
 * 
 * 
 * If there is a message to return, we allocate new memory for it, copy in the
 * message data, and return the memory address via \c msg_ptr. This prevents the 
 * user from accidentally modifying or deallocating memory within the 
 * MessageBoard.
 * 
 * It is the user's responsibility to free the memory associated with the 
 * returned msg.
 *
 * The MBIt_Iterator::cursor pointer is used to keep track of which message 
 * should be returned in during each call. This pointer is updated at the end 
 * of each call. It will contain \c NULL if the Iterator is empty, of if iteration
 * has reached the end or not yet begun. The MBIt_Iterator::iterating flag is used
 * to indicate if iteration has started.
 * 
 * When there are no more messages to return, \c msg_ptr will be assigned with
 * \c NULL and the routine shall complete with the ::MB_SUCCESS return code.
 *
 * Any attempts to get a message from a null Iterator (::MB_NULL_ITERATOR) will 
 * result in an ::MB_ERR_INVALID error.
 * 
 * In the event of an error, msg will be assigned \c NULL and the routine shall
 * return with an appropriate error code.
 * 
 * \note If the given Iterator is invalidated due to a deleted/cleared board, the
 * pointer returned by this routine will not be valid. 
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (Iterator is invalid)
 *  - ::MB_ERR_MEMALLOC (error allocating requied memory)
 * 
 */
int MB_Iterator_GetMessage(MBt_Iterator itr, void **msg_ptr) {
    
    MBIt_Iterator *iter;
    void *data_addr;
    
    /* nothing to do for null iterator */
    if (itr == MB_NULL_ITERATOR) 
    {
        P_FUNCFAIL("Cannot iterator null iterator (MB_NULL_ITERATOR)");
        return MB_ERR_INVALID;
    }
    
    /* Get reference to iter object */
    iter = (MBIt_Iterator *)MBI_getIteratorRef(itr);
    if (iter == NULL) 
    {
        P_FUNCFAIL("Invalid iterator handle (%d)", (int)itr);
        return MB_ERR_INVALID;
    }
    assert(iter->data != NULL);
    
    if (iter->iterating == 0) /* first time iterator is called */
    { 
        assert(iter->cursor == NULL);
        
        iter->iterating = 1;
        iter->cursor = PL_ITERATOR(iter->data);
    }
    
    if (iter->cursor == NULL) /* we've reached the end (or empty iterator) */
    { 
        *msg_ptr = NULL;
        return MB_SUCCESS;
    }
    
    /* allocate memory for output message */
    *msg_ptr = malloc(iter->msgsize);
    assert(*msg_ptr != NULL);
    if ((*msg_ptr) == NULL) 
    {
        P_FUNCFAIL("Could not allocate required memory");
        return MB_ERR_MEMALLOC;
    }
    
    /* copy message to allocated buffer */
    data_addr = *((void **)PL_NODEDATA(iter->cursor));
    memcpy(*msg_ptr, data_addr, (size_t)iter->msgsize);

    /* move cursor */
    iter->cursor = iter->cursor->next;
    
    return MB_SUCCESS;
}
