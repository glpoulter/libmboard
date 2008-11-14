/* $Id$ */
/*!
 * \file parallel/iterator_randomise.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Iterator_Randomise()
 * 
 */
#include "mb_parallel.h"
/*!
 * \brief Randomises the order of entries in an Iterator
 * \ingroup MB_API
 * \param[in] itr Iterator Handle
 * 
 * Apart from randomising the order of entries in the Iterator, this routine 
 * will also reset the internal counters leading to an effect similar to that
 * of MB_Iterator_Rewind(). This decision was made based on the assumption 
 * that once the messages have been reshuffled, the positional cursor (either
 * tied to the index/offset or specific message) will no longer have any 
 * meaning.
 * 
 * Randomising a null Iterator (::MB_NULL_ITERATOR) will result in an 
 * ::MB_ERR_INVALID error.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (the Iterator is either null or invalid)
 * 
 */

int MB_Iterator_Randomise(MBt_Iterator itr) {
    
    int rc;
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
    
    /* randomise pooled list */
    rc = pl_randomise(iter->data);
    
    if (rc != PL_SUCCESS) return MB_ERR_INTERNAL;
   
    return MB_SUCCESS;
}

