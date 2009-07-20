/* $Id$ */
/*!
 * \file parallel/filter_create.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Filter_Create()
 * 
 */

#include "mb_parallel.h"

#ifdef _EXTRA_CHECKS
#ifdef _DEVELOPER
    static void check_all_fh_equal(OM_key_t key, MBIt_filterfunc fptr);
    
    struct fdata_t {
        OM_key_t handle;
        MBIt_filterfunc fptr;
    };
#endif /* _DEVELOPER */
#endif /*_EXTRA_CHECKS*/
    
/*!
 * \brief Creates a Filter object based on a function
 * \ingroup MB_API
 * \param[out] fh_ptr Address of Filter handle
 * \param[in] filterFunc Pointer to user-defined filter function
 * 
 * The \c filterFunc pointer is placed in a newly allocated ::MBIt_filterfunc_wrapper
 * object and registered with the ::MBI_OM_filter map. The associated 
 * function handle is then returned through \c fh_ptr.
 * 
 * If the function returns with an error code, \c fh_ptr would be set to
 * ::MB_NULL_FILTER
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 *  - ::MB_ERR_INVALID (\c filterFunc is \c NULL)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_OVERFLOW (ObjectMap overflow. Too many functions registered.)
 *  - ::MB_ERR_INTERNAL (Internal error. Possibly a bug.)
 */
int MB_Filter_Create(MBt_Filter *fh_ptr, 
        int (*filterFunc)(const void *msg, int pid) ) {
    
    OM_key_t rc_fh;
    MBIt_filterfunc_wrapper *fwrap;
    
    /* first, some quick checks */
    if (fh_ptr == NULL)
    {
    	P_FUNCFAIL("NULL pointer given in first argument");
    	return MB_ERR_INVALID;
    }
    
    /* set NULL return value first, in case of error conditions */
    *fh_ptr = MB_NULL_FILTER;
    
    /* sorry sir, me no deal with null function pointers */
    if (filterFunc == NULL)
    {
    	P_FUNCFAIL("NULL function pointer given in seconds argument");
    	return MB_ERR_INVALID;
    }
    
    /* allocate memory for your function ptr wrapper */
    fwrap = (MBIt_filterfunc_wrapper *)malloc(sizeof(MBIt_filterfunc_wrapper));
    assert(fwrap != NULL);
    if (fwrap == NULL)
    {
    	P_FUNCFAIL("Could not allocate required memory");
    	return MB_ERR_MEMALLOC;
    }
    
    /* embed func pointer into wrapper */
    fwrap->func = (MBIt_filterfunc)filterFunc;
    
    /* register wrapper object in objmap */
    assert(MBI_OM_filter != NULL);
    assert(MBI_OM_filter->type == OM_TYPE_FILTER);
    rc_fh = MBI_objmap_push(MBI_OM_filter, (void*)fwrap);
    if (rc_fh > OM_MAX_INDEX)
    {
        if (rc_fh == OM_ERR_MEMALLOC)
        {
        	P_FUNCFAIL("Could not allocate required memory");
            return MB_ERR_MEMALLOC;
        }
        else if (rc_fh == OM_ERR_OVERFLOW)
        {
        	P_FUNCFAIL("Too many filters created. ObjMap keys overflowed");
            return MB_ERR_OVERFLOW;
        }
        else
        {
        	P_FUNCFAIL("ObjectMap error. MBI_objmap_push() returned %d", (int)rc_fh);
            return MB_ERR_INTERNAL;
        }
    }
    
    /* debug: make sure same fh on all procs */
#ifdef _EXTRA_CHECKS
#ifdef _DEVELOPER
    check_all_fh_equal((OM_key_t)rc_fh, (MBIt_filterfunc)filterFunc);
#endif /* _DEVELOPER */
#endif /*_EXTRA_CHECKS*/
    
    /* assign fh */
    *fh_ptr = (MBt_Function)rc_fh;
    
    P_INFO("Filter function registered. Handle = %d", (int)rc_fh);
    
    /* victory is mine! */
    return MB_SUCCESS;
}

#ifdef _EXTRA_CHECKS
#ifdef _DEVELOPER
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
#endif /* _DEVELOPER */
#endif /*_EXTRA_CHECKS*/

