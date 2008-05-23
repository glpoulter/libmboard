/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */
#include "header_mb_serial.h"

/* Define tests within this suite */
CU_TestInfo mbs_test_array[] = {
    
    /* test_mb_s1.c */
    {"MB Environment initialisation                 ", test_mb_s_init            },
    {"Board Creation / Deletion                     ", test_mb_s_create_delete   },
    {"Multiple board creation                       ", test_mb_s_create_multiple },
    /* test_mb_s2.c */
    {"Message Add                                   ", test_mb_s_addmessage      },
    {"Message Add beyond single block               ", test_mb_s_addmessage_many },
    /* test_mb_s3.c */
    {"Board Clearance                               ", test_mb_s_clear           },
    /* test_mb_s4.c */
    {"Board Synchronisation (start)                 ", test_mb_s_sync_start      },
    {"Sync Completion test                          ", test_mb_s_sync_test       },
    {"Board Synchronisation (complete)              ", test_mb_s_sync_complete   },
    /* test_mb_s5.c */
    {"Creating Iterator                             ", test_mb_s_iter_create     },
    {"Reading messages from Iterator                ", test_mb_s_iter_getmsg     },
    {"Deleting Iterator                             ", test_mb_s_iter_delete     },
    /* test_mb_s6.c */
    {"Creating Sorted Iterator                      ", test_mb_s_iter_create_sorted   },
    {"Reading messages from Sorted Iterator         ", test_mb_s_iter_sorted_getmsg   },
    /* test_mb_s7.c */
    {"Creating Filtered Iterator                    ", test_mb_s_iter_create_filtered },
    {"Reading messages from Filtered Iterator       ", test_mb_s_iter_filtered_getmsg },
    /* test_mb_s10.c */
    {"Creating Filtered+Sorted Iterator             ", test_mb_s_iter_create_filteredsorted },
    {"Reading messages from Filtered+Sorted Iterator", test_mb_s_iter_filteredsorted_getmsg },
    /* test_mb_s8.c */
    {"Rewinding Iterator                            ", test_mb_s_iter_rewind       },
    /* test_mb_s9.c */
    {"Randomising Iterator                          ", test_mb_s_iter_randomise    },
    /* test_mb_s12.c */
    {"Registering a function                        ", test_mb_s_function_register },
    {"Assigning a function to board                 ", test_mb_s_function_assign   },
    {"Freeing a function                            ", test_mb_s_function_free     },

    CU_TEST_INFO_NULL,
};
    
int init_mb_serial(void) {
    int rc;
    
    rc = MB_Env_Init();
    if (rc != MB_SUCCESS) return rc;
    
    return 0;
}

int clean_mb_serial(void) {
    int rc;
    
    rc = MB_Env_Finalise();
    if (rc != MB_SUCCESS) return rc;
    
    return 0;
}

CU_ErrorCode testsuite_mb_serial(void) {
   
    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"MBoard Library (serial)", init_mb_serial, clean_mb_serial, mbs_test_array},
        CU_SUITE_INFO_NULL,
    };
    
    return CU_register_suites(suites);
    
}
