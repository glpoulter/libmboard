/* $Id$ */
/*!
 * \file parallel/env_finalise.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Mar 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Env_Finalise()
 * 
 */

#include "mb_parallel.h"

/*!
 * \brief Finalises the libmboard environment
 * \ingroup MB_API
 * 
 * 
 * ObjectMaps are destroyed, and the \c MBI_STATUS_* flags are updated.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_ENV (libmboard environment not yet started or already finalised)
 */
int MB_Env_Finalise(void) {
    
    int rc;
    
    /* Check if environment not initialised */
    if (MBI_STATUS_initialised != MB_TRUE) return MB_ERR_ENV;
    if (MBI_STATUS_finalised == MB_TRUE) return MB_ERR_ENV;
   
    /* finalise Communication Thread */
    rc = MBI_CommThread_Finalise();
    assert(rc == MB_SUCCESS);
    if (rc != MB_SUCCESS) return rc;
    
    /* deallocate Object Maps */
    MBI_objmap_destroy(&MBI_OM_mboard);
    MBI_objmap_destroy(&MBI_OM_iterator);
    MBI_objmap_destroy(&MBI_OM_filter);
    MBI_objmap_destroy(&MBI_OM_indexmap);
    MBI_objmap_destroy(&MBI_OM_searchtree);
    
    /* deallocate string map */
    rc = MBI_stringmap_Delete(&MBI_indexmap_nametable);
    assert(rc == MB_SUCCESS);
    
    /* free our MPI_Communicator */
    MPI_Comm_free(&MBI_CommWorld);
    
    #ifdef _LOG_MEMORY_USAGE
        memlog_milestone("F");
        memlog_finalise();
    #endif
        
    P_INFO("Message Board environment finalised");
    
    /* set initialised status and return */
    MBI_STATUS_finalised   = MB_TRUE;
    MBI_STATUS_initialised = MB_FALSE;
    return MB_SUCCESS;
}
