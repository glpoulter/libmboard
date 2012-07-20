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

#ifndef ACKNOWLEDGED_UNUSED
#define ACKNOWLEDGED_UNUSED(expr) do { (void)(expr); } while (0)
#endif

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
    
    /* CU_ErrorCode rc; */
    
    /* acknowledge+hide "unused parameter" compiler warnings */
    ACKNOWLEDGED_UNUSED(argc);
    ACKNOWLEDGED_UNUSED(argv);
    
    /* seed rng */
    srand((unsigned int)time(NULL));
    
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }
    
    
    /* ================ Add Test Suites to registry ================= */
    
    /* Test suite for Parallel Libmboard */
    if(testsuite_mb_parallel() != CUE_SUCCESS) return clean_quit();
 

    /* -------------------------------------------------------------- */
    
    
    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    /* clean up registry and quit */
    return clean_quit();
}
