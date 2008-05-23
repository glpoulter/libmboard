/* $Id$ */
/*!
 * \file parallel/function_register.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Function_Register()
 * 
 */

#include "mb_parallel.h"

#ifdef _EXTRA_CHECKS
    static void check_all_fh_equal(OM_key_t key, MBIt_filterfunc fptr);
    
    struct fdata_t {
        MBt_Function handle;
        MBIt_filterfunc fptr;
    };
    
#endif /*_EXTRA_CHECKS*/
    
/* Register a function */
int MB_Function_Register(MBt_Function *fh_ptr, 
        int (*filterFunc)(const void *msg, const void *params) ) {
    
    OM_key_t rc_fh;
    MBIt_filterfunc_wrapper *fwrap;
    
    /* first, assign fh_ptr to NULL FUNC */
    *fh_ptr = MB_NULL_FUNCTION;
    
    /* Check if func ptr is null */
    if (filterFunc == NULL) return MB_ERR_INVALID;
    
    /* create and populate func ptr wrapper so it can be placed in the map */
    fwrap = (MBIt_filterfunc_wrapper *)malloc(sizeof(MBIt_filterfunc_wrapper));
    assert(fwrap != NULL);
    if (fwrap == NULL || errno != 0) return MB_ERR_MEMALLOC;
    fwrap->func = filterFunc; /* set func ptr in wrapper */
    
    /* add func ptr to map. Get back fh. Make sure it is valid */
    assert(MBI_OM_function != NULL);
    assert(MBI_OM_function->type == OM_TYPE_FUNCTION);
    rc_fh = MBI_objmap_push(MBI_OM_function, (void*)fwrap);
    if (rc_fh > OM_MAX_INDEX)
    {
        if (rc_fh == OM_ERR_MEMALLOC)
        {
            return MB_ERR_MEMALLOC;
        }
        else
        {
            return MB_ERR_INTERNAL;
        }
    }
    
    /* debug: make sure same fh on all procs */
#ifdef _EXTRA_CHECKS
    check_all_fh_equal(rc_fh, filterFunc);
#endif /*_EXTRA_CHECKS*/
    
    /* assign fh */
    *fh_ptr = (MBt_Function)rc_fh;
    
    /* return success */
    return MB_SUCCESS;
}


#ifdef _EXTRA_CHECKS


static void check_all_fh_equal(OM_key_t key, MBIt_filterfunc fptr) {
    
    int rc;
    struct fdata_t fdata; 
    
    if (MASTERNODE)
    {
        fdata.handle = key;
        fdata.fptr   = fptr;
    }
    
    rc = MPI_Bcast(&fdata, (int)sizeof(struct fdata_t), MPI_BYTE, 0, MBI_CommWorld);
    assert(rc == MPI_SUCCESS);
    assert(fdata.handle == key);
    assert(fdata.fptr   == fptr);

}

#endif /*_EXTRA_CHECKS*/
