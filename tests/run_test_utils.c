/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
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

#ifdef _LOG_MEMORY_USAGE
int MBI_CommSize = 1;
int MBI_CommRank = 0;
#endif

int main(int argc, char ** argv) {
    
    /* acknowledge+hide "unused parameter" compiler warnings */
    ACKNOWLEDGED_UNUSED(argc);
    ACKNOWLEDGED_UNUSED(argv);
    
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

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }
    
    
    /* ================ Add Test Suites to registry ================= */
    
    /* Test pooled-list ADT */
    if(testsuite_pl() != CUE_SUCCESS) return clean_quit();
    
    /* Test Object Map ADT */
    if(testsuite_om() != CUE_SUCCESS) return clean_quit();
    
    /* Test Tag Table ADT */
    if(testsuite_tt() != CUE_SUCCESS) return clean_quit();
    
    /* Test AVL Tree ADT */
    if(testsuite_avl() != CUE_SUCCESS) return clean_quit();
    
    /* Test AVL Tree ADT */
    if(testsuite_kd() != CUE_SUCCESS) return clean_quit();

    /* Test String Map */
    if(testsuite_sm() != CUE_SUCCESS) return clean_quit();
    
    /* -------------------------------------------------------------- */
    
    
    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    /* clean up registry and quit */
    return clean_quit();
}
