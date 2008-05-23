/* $Id$ */
/*!
 * \file parallel/function_free.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Function_Free()
 * 
 */

#include "mb_parallel.h"

/* Deallocate a registered function */
int MB_Function_Free(MBt_Function *fh_ptr) {
    
    void *obj;
    
    /* make sure fh_ptr not null */
    if (*fh_ptr == MB_NULL_FUNCTION) return MB_ERR_INVALID;
    
    /* remove from board */
    obj = MBI_objmap_pop(MBI_OM_function, (OM_key_t)*fh_ptr);
    if (obj == NULL) return MB_ERR_INVALID;
    
    /* free object and return */
    free(obj);
    *fh_ptr = MB_NULL_FUNCTION;
    
    return MB_SUCCESS;
}
