/* $Id: setup_suite_sm.c 2925 2012-07-20 14:12:17Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */
#include "header_string_map.h"

/* Define tests within this suite */
CU_TestInfo sm_test_array[] = {
    
    {"Instantiating string maps                   ", test_sm_create           }, 
    {"Deleting string maps                        ", test_sm_delete           }, 
    {"Adding strings to the map                   ", test_sm_addstring        },
    {"Removing strings from the map               ", test_sm_removestring     },
    {"Querying the map                            ", test_sm_contains         },
    
    CU_TEST_INFO_NULL,
};
    
int init_sm(void) {
    return 0;
}

int clean_sm(void) {
    return 0;
}

CU_ErrorCode testsuite_sm(void) {
    
    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"String Map", init_sm, clean_sm, sm_test_array},
        CU_SUITE_INFO_NULL,
    };
    
    return CU_register_suites(suites);
    
}
