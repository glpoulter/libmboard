/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */
#include "header_objmap.h"

/* Define tests within this suite */
CU_TestInfo om_test_array[] = {
    
    {"Object creation and deletion                ", test_om_new_destroy   },
    {"Pushing object into Map                     ", test_om_push          },
    {"Popping object from Map                     ", test_om_pop           },
    {"Mapping handles to objects                  ", test_om_getobj        },
    CU_TEST_INFO_NULL,
};
    
int init_om(void) {
    return 0;
}

int clean_om(void) {
    return 0;
}

CU_ErrorCode testsuite_om(void) {
    
    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"Object Mapper ADT", init_om, clean_om, om_test_array},
        CU_SUITE_INFO_NULL,
    };
    
    return CU_register_suites(suites);
    
}
