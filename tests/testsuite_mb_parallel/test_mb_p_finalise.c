/* $Id: test_mb_p_finalise.c 1871 2009-06-12 13:58:18Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */

#include "header_mb_parallel.h"

/* test finalise */
void test_mb_p_finalise(void) {
    
    int rc;
    
    /* finalise the environment */
    rc = MB_Env_Finalise();
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* check env status query routines */
    rc = MB_Env_Initialised();
    CU_ASSERT_EQUAL(rc, MB_ERR_ENV);
    rc = MB_Env_Finalised();
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Flags should be properly set */
    CU_ASSERT_EQUAL(MBI_STATUS_initialised, MB_FALSE);
    CU_ASSERT_EQUAL(MBI_STATUS_finalised,   MB_TRUE);
    
    /* object maps destroyed */
    CU_ASSERT_PTR_NULL(MBI_OM_mboard);
    CU_ASSERT_PTR_NULL(MBI_OM_iterator);
    CU_ASSERT_PTR_NULL(MBI_OM_filter);
    
    /* Make sure MPI communicator has been freed */
    CU_ASSERT_EQUAL(MBI_CommWorld, MPI_COMM_NULL);
    
}
