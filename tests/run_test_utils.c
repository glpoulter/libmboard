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
    
    /* -------------------------------------------------------------- */
    
    
    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    rc = CU_basic_run_tests();
    
    /* clean up registry and quit */
    return clean_quit();
}
