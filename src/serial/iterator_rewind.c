/* $Id$ */
/*!
 * \file serial/iterator_rewind.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_Iterator_Rewind()
 * 
 */

#include "mb_serial.h"

/*!
 * \brief Rewinds an Iterator
 * \ingroup MB_API
 * \param[in] itr Iterator Handle
 * 
 * Resets the internal counters such that the next MB_Iterator_GetMessage() 
 * call on the given Iterator will obtain the first message in the list 
 * (or \c NULL if the Iterator is empty).
 * 
 * Specifically, MBIt_Iterator::cursor is set to \c NULL, and 
 * MBIt_Iterator::iterating is set to \c 0.
 * 
 * Rewinding a null Iterator (::MB_NULL_ITERATOR) will result in an 
 * ::MB_ERR_INVALID error.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID
 * 
 */
int MB_Iterator_Rewind(MBt_Iterator itr) {
    
    MBIt_Iterator *iter;

    /* can't rewind a null iterator */
    if (itr == MB_NULL_ITERATOR) return MB_ERR_INVALID;
    
    /* Get reference to iter object */
    iter = (MBIt_Iterator *)MBI_getIteratorRef(itr);
    if (iter == NULL) return MB_ERR_INVALID;
    assert(iter->data != NULL);
    
    /* reset flag and cursor */
    iter->cursor = NULL;
    iter->iterating = 0;
    
    return MB_SUCCESS;
}
