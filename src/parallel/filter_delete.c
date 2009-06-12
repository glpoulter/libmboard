/* $Id$ */
/*!
 * \file parallel/filter_delete.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Filter_Delete()
 * 
 */

#include "mb_parallel.h"

/*!
 * \brief Deletes a filter object
 * \ingroup MB_API
 * \param[in,out] fh_ptr Address of Filter handle
 * 
 * The associated filter object is removed from the map and deallocated.
 * 
 * On success, \c fh_ptr is set to MB_NULL_FILTER. On error, \c fh_ptr
 * will remain unchanged, and an appropriate error code is returned.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 *  - ::MB_ERR_INVALID (invalid or null function handle) 
 */
int MB_Filter_Delete(MBt_Filter *fh_ptr) {
    
    void *obj;
    
    if (fh_ptr == NULL) return MB_ERR_INVALID;
    
    /* make sure fh_ptr not null */
    if (*fh_ptr == MB_NULL_FILTER) return MB_SUCCESS;
    
    /* remove from board */
    obj = MBI_objmap_pop(MBI_OM_filter, (OM_key_t)*fh_ptr);
    if (obj == NULL) return MB_ERR_INVALID;
    
    /* free object and return */
    free(obj);
    *fh_ptr = MB_NULL_FUNCTION;
    
    return MB_SUCCESS;
}
