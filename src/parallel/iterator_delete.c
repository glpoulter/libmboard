/* $Id$ */
/*!
 * \file parallel/iterator_delete.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Iterator_Delete()
 * 
 */

#include "mb_parallel.h"


/*!
 * \brief Deletes an Iterator
 * \ingroup MB_API
 * \param[in,out] itr_ptr Address of Iterator Handle
 * 
 * Upon successful removal of the reference to the Iterator from the 
 * ::MBI_OM_iterator ObjectMap, we first delete the pooled-list associated 
 * with the Iterator and then deallocate the Iterator object.
 * 
 * \note It is valid to delete a null Iterator (::MB_NULL_ITERATOR). The routine
 * will return immediately with ::MB_SUCCESS.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 *  - ::MB_ERR_INVALID (invalid Iterator given) 
 *  - ::MB_ERR_INTERNAL (possible bug. Recompile and run in debug mode for hints)
 */
int MB_Iterator_Delete(MBt_Iterator *itr_ptr) {
	
    int rc;
    MBIt_Iterator *iter;

    /* nothing to do for null iterator */
    if (*itr_ptr == MB_NULL_ITERATOR)
    {
        P_WARNING("Deletion of null iterator (MB_NULL_ITERATOR)");
        return MB_SUCCESS;
    }
    
    /* pop iterator from object map */
    assert(MBI_OM_iterator != NULL);
    assert(MBI_OM_iterator->type == OM_TYPE_ITERATOR);
    iter = (MBIt_Iterator *)MBI_objmap_pop(MBI_OM_iterator, (OM_key_t)*itr_ptr);
    if (iter == NULL) 
    {
        P_FUNCFAIL("Invalid iterator handle (%d)", (int)*itr_ptr);
        return MB_ERR_INVALID;
    }
    
    assert(iter != NULL);
    assert(iter->data != NULL);
    
    /* free memory used by pooled list */
    rc = pl_delete(&(iter->data));
    assert(rc == PL_SUCCESS);
    
    /* deallocate iterator object */
    free(iter);
    
    if (rc != PL_SUCCESS) 
    {
        P_FUNCFAIL("pl_delete() returned with err code %d", rc);
        return MB_ERR_INTERNAL;
    }
    
    P_INFO("Deleted iterator (%d)", (int)*itr_ptr);
    *itr_ptr = MB_NULL_ITERATOR;
    
    return MB_SUCCESS;
}
