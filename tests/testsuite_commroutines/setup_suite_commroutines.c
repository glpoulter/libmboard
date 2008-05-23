/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2008
 * 
 */
#include "header_commroutines.h"

/* Define tests within this suite */
CU_TestInfo cr_test_array[] = {

    /* test_cr1.c */
    {"MBIt_Comm_InitTagging()               ", test_cr_inittagging         },
    {"MBIt_Comm_WaitTagInfo()               ", test_cr_waittaginfo         },
    {"MBIt_Comm_TagMessages()               ", test_cr_tagmessages         },
    {"MBIt_Comm_InitPropagation()           ", test_cr_initpropagation     },
    {"MBIt_Comm_CompletePropagation()       ", test_cr_completepropagation },
    
    CU_TEST_INFO_NULL,
};
    
CU_ErrorCode testsuite_commroutines(void) {
    
    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"Communication Routines", init_cr, clean_cr, cr_test_array},
        CU_SUITE_INFO_NULL,
    };
    
    return CU_register_suites(suites);
    
}
