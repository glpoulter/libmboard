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
    

    {"MB Environment initialisation                 ", test_mb_s_init            },
    {"Board Creation / Deletion                     ", test_mb_s_create_delete   },
    {"Multiple board creation                       ", test_mb_s_create_multiple },
    
    {"Setting board access modes                    ", test_mb_s_setaccessmode   },

    {"Message Add                                   ", test_mb_s_addmessage      },
    {"Message Add beyond single block               ", test_mb_s_addmessage_many },

    {"Board Clearance                               ", test_mb_s_clear           },

    {"Board Synchronisation (start)                 ", test_mb_s_sync_start      },
    {"Sync Completion test                          ", test_mb_s_sync_test       },
    {"Board Synchronisation (complete)              ", test_mb_s_sync_complete   },

    {"Creating Iterator                             ", test_mb_s_iter_create     },
    {"Reading messages from Iterator                ", test_mb_s_iter_getmsg     },
    {"Deleting Iterator                             ", test_mb_s_iter_delete     },

    {"Creating Sorted Iterator                      ", test_mb_s_iter_create_sorted   },
    {"Reading messages from Sorted Iterator         ", test_mb_s_iter_sorted_getmsg   },

    {"Creating Filtered Iterator                    ", test_mb_s_iter_create_filtered },
    {"Reading messages from Filtered Iterator       ", test_mb_s_iter_filtered_getmsg },

    {"Creating Filtered+Sorted Iterator             ", test_mb_s_iter_create_filteredsorted },
    {"Reading messages from Filtered+Sorted Iterator", test_mb_s_iter_filteredsorted_getmsg },

    {"Rewinding Iterator                            ", test_mb_s_iter_rewind       },

    {"Randomising Iterator                          ", test_mb_s_iter_randomise    },

    {"Creating filter                               ", test_mb_s_filter_create     },
    {"Assigning filter to board                     ", test_mb_s_filter_assign     },
    {"Deleting filter                               ", test_mb_s_filter_delete     },

    /* testing indexmap */
    {"Creating Index Maps                           ", test_mb_s_indexmap_create   },
    {"Deleting Index Maps                           ", test_mb_s_indexmap_delete   },
    {"Adding entries to Index Map                   ", test_mb_s_indexmap_addentry },
    {"Adding entries to Index Map with duplicates   ", test_mb_s_indexmap_addentry_withdups },
    {"Adding random entries to Index Map (with dups)", test_mb_s_indexmap_addentry_randomvals },
    {"Querying an Index Map                         ", test_mb_s_indexmap_memberof },
    {"Querying an Index Map with duplicates         ", test_mb_s_indexmap_memberof_withdups },
    {"Querying an Index Map with large spread + dups", test_mb_s_indexmap_memberof_randomvals },
    {"Synchronising Index Maps                      ", test_mb_s_indexmap_sync     },
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
