/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : July 2008
 * 
 */
#include "header_tag_table.h"

/* Define tests within this suite */
CU_TestInfo tt_test_array[] = {
    
    /* test_tt1.c */
    {"Object creation and deletion                ", test_tt_new_destroy   },
    {"Retrieving row pointer from tag table       ", test_tt_getrow        },
    {"Testing tt_setbits()                        ", test_tt_setbits       },
    {"Testing tt_setbyte()                        ", test_tt_setbyte       },
    {"Testing tt_getcount_row()                   ", test_tt_getcount_row  },
    {"Testing tt_getcount_col()                   ", test_tt_getcount_col  },

    CU_TEST_INFO_NULL,
};
    
int init_tt(void) {
    return 0;
}

int clean_tt(void) {
    return 0;
}

CU_ErrorCode testsuite_tt(void) {
    
    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"Tag Table ADT", init_tt, clean_tt, tt_test_array},
        CU_SUITE_INFO_NULL,
    };
    
    return CU_register_suites(suites);
    
}
