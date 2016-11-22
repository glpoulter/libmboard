/* $Id: setup_suite_commroutines_old.c 2108 2009-09-14 14:14:31Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2008
 * 
 */
#include "header_commroutines_old.h"

/* Define tests within this suite */
CU_TestInfo cr_old_test_array[] = {

    {"TagMessages                           ", test_cr_old_tagmessages         },
    {"SendBufInfo                           ", test_cr_old_sendbufinfo         },
    {"WaitBufInfo                           ", test_cr_old_waitbufinfo         },
    {"InitPropagation                       ", test_cr_old_initpropagation     },
    {"CompletePropagation                   ", test_cr_old_completepropagation },

    CU_TEST_INFO_NULL,
};
    
CU_ErrorCode testsuite_commroutines_old(void) {
    
    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"Communication Routines (OLD)", init_cr_old, clean_cr_old, cr_old_test_array},
        CU_SUITE_INFO_NULL,
    };
    
    return CU_register_suites(suites);
    
}
