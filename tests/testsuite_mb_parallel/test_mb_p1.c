/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */

#include "header_mb_parallel.h"
#include "syncqueue.h"

/* test init */
void test_mb_p_init(void) {
    
    int rc, flag, size, rank;
    
    /* Flags should be properly set */
    CU_ASSERT_EQUAL(MBI_STATUS_initialised, MB_TRUE);
    CU_ASSERT_EQUAL(MBI_STATUS_finalised,   MB_FALSE);
    
    /* object maps properly defined */
    CU_ASSERT_PTR_NOT_NULL_FATAL(MBI_OM_mboard);
    CU_ASSERT_EQUAL(MBI_OM_mboard->type, OM_TYPE_MBOARD);
    
    CU_ASSERT_PTR_NOT_NULL_FATAL(MBI_OM_iterator);
    CU_ASSERT_EQUAL(MBI_OM_iterator->type, OM_TYPE_ITERATOR);
    
    CU_ASSERT_PTR_NOT_NULL_FATAL(MBI_OM_function);
    CU_ASSERT_EQUAL(MBI_OM_function->type, OM_TYPE_FUNCTION);
    
    /* make sure MPI env initialised */
    rc = MPI_Initialized(&flag);
    CU_ASSERT_EQUAL(rc, MPI_SUCCESS);
    CU_ASSERT(flag);
    
    /* make sure MPI related vars assigned */
    rc = MPI_Comm_size(MBI_CommWorld, &size);
    CU_ASSERT_EQUAL(rc, MPI_SUCCESS);
    CU_ASSERT_EQUAL(size, MBI_CommSize);
    
    rc = MPI_Comm_rank(MBI_CommWorld, &rank);
    CU_ASSERT_EQUAL(rc, MPI_SUCCESS);
    CU_ASSERT_EQUAL(rank, MBI_CommRank);
    
    /* we should use our own communicator */
    CU_ASSERT_NOT_EQUAL(MBI_CommWorld, MPI_COMM_WORLD);
    
    /* check env status query routines */
    rc = MB_Env_Initialised();
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_Env_Finalised();
    CU_ASSERT_EQUAL(rc, MB_ERR_ENV);
    
}

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
    CU_ASSERT_PTR_NULL(MBI_OM_function);
    
    /* Make sure MPI communicator has been freed */
    CU_ASSERT_EQUAL(MBI_CommWorld, MPI_COMM_NULL);
    
}
