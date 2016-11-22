/* $Id: setup_suite_commqueue.c 869 2008-05-19 12:26:29Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */
#include "header_commqueue.h"

/* Define tests within this suite */
CU_TestInfo cq_test_array[] = {
    
    /* test_cq1.c */
    {"CommQueue initialisation              ", test_cq_init        },
    {"Adding new node to queue              ", test_cq_newnode     },
    {"Deleting nodes from queue             ", test_cq_deletenode  },
        
    
    CU_TEST_INFO_NULL,
};
    
int init_cq(void) {
    return MBI_CommQueue_Init();
}

int clean_cq(void) {
    
    /* flush commqueue */
    MBI_CommQueue_Delete();
    
    return 0;
}

CU_ErrorCode testsuite_commqueue(void) {
    
    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"Communication Queue", init_cq, clean_cq, cq_test_array},
        CU_SUITE_INFO_NULL,
    };
    
    return CU_register_suites(suites);
    
}
