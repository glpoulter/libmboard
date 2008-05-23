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
#include <string.h>


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
 * We use the \c swapIteratorNode() static function in iterator_randomise.c to
 * swap nodes. This routine swaps data within the pooled_list memory block and
 * relinks the 'next' pointers for the linked list.
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
    
    int count, i, rnd, rc;
    MBIt_Iterator *iter;
    pooled_list *pl;
    void *current, *target, *temp;
    temp = NULL;
    
    /* can't rewind a null iterator */
    if (itr == MB_NULL_ITERATOR) return MB_ERR_INVALID;
    
    /* Get reference to iter object */
    iter = (MBIt_Iterator *)MBI_getIteratorRef(itr);
    if (iter == NULL) return MB_ERR_INVALID;
    assert(iter->data != NULL);

    /* reset flag and cursor */
    iter->cursor = NULL;
    iter->iterating = 0;
    
    /* randomise data in pooled list */
    pl = iter->data;
    count = (int)pl->count_current;
    
    /* nothing to do for single or no message */
    if (count <= 1) return MB_SUCCESS;
    
    /* iterate thru pl data. randomise while resetting next pointer */
    temp = malloc((size_t)pl->elem_size);
    for (i = count - 1; i > 0; i--)
    {
        /* get a random number */
        rnd = (int)((double)i * rand() / (RAND_MAX + 1.0));
        
        if (rnd == i) continue; /* no point swapping with self */

        /* get reference to selected nodes */
        rc = pl_getnode(pl, i, &current);
        assert(rc == PL_SUCCESS);
        assert(current != NULL);
        
        rc = pl_getnode(pl, rnd, &target);
        assert(rc == PL_SUCCESS);
        assert(target != NULL);
        
        /* do the swap */
        memcpy(temp, target, (size_t)pl->elem_size);
        memcpy(target, current, (size_t)pl->elem_size);
        memcpy(current, temp, (size_t)pl->elem_size);
    }
    free(temp);
    
    return MB_SUCCESS;
}
