/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */
/*@-redef@*/

#include "CUnit/Basic.h"
#include "testing.h"
#include <mpi.h>

static int clean_quit(void) {
    
    int failures = (int)CU_get_number_of_tests_failed();
    
    CU_cleanup_registry();
    /* return CU_get_error(); */
    
    /* return number of failures so runs launched by Makefile
     * will stop when error is encountered 
     */
    return failures;
}

int main(int argc, char ** argv) {
    
    CU_ErrorCode rc;
    
#ifdef _LOG_MEMORY_USAGE
    /* Don't proceed with this test if compiled with --enable-memlog
     * memlog_init() would not have been called and all alloc/free calls
     * will fail
     */
    fprintf(stderr, "ERROR: This test cannot be run when compiled with --enable-memlog\n");
    fprintf(stderr, "       However, you should still be able to run 'run_test_serial'\n");
    fprintf(stderr, "       and 'run_test_parallel'.\n");
    return 1;
#endif
    
    /* initialise MPI environment */
    MPI_Init(&argc, &argv);
    
    /* seed rng */
    srand((unsigned int)time(NULL));
    
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }
    
    
    /* ================ Add Test Suites to registry ================= */
    
    /* Test CommQueue used in parallel lib */
    if(testsuite_commqueue() != CUE_SUCCESS) return clean_quit();
    
    /* Test SyncQueue used in parallel lib */
    if(testsuite_syncqueue() != CUE_SUCCESS) return clean_quit();
    
    /* Test communication routines */
    if(testsuite_commroutines() != CUE_SUCCESS) return clean_quit();
    
    /* -------------------------------------------------------------- */
    
    
    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    rc = CU_basic_run_tests();
    
    /* finalise MPI environment */
    MPI_Finalize();
    
    /* clean up registry and quit */
    return clean_quit();
}
