/* $Id: setup_suite_syncqueue.c 837 2008-04-25 15:57:59Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */
#include "header_syncqueue.h"

/* Define tests within this suite */
CU_TestInfo sq_test_array[] = {
    
    /* test_sq1.c */
    {"SyncQueue initialisation              ", test_sq_init     },
    {"Pushing requests into queue           ", test_sq_push     },
    {"Popping requests from queue           ", test_sq_pop      },
        
    
    CU_TEST_INFO_NULL,
};
    
int init_sq(void) {
    return MBI_SyncQueue_Init();
}

int clean_sq(void) {
    
    /* flush syncqueue */
    if (!MBI_SyncQueue_isEmpty())
    {
        MBI_SyncQueue_Delete();
    }
    
    return 0;
}

CU_ErrorCode testsuite_syncqueue(void) {
    
    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"Sync Request Queue", init_sq, clean_sq, sq_test_array},
        CU_SUITE_INFO_NULL,
    };
    
    return CU_register_suites(suites);
    
}
