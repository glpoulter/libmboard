/* $Id: setup_suite_pl.c 2817 2012-05-22 10:15:15Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */
#include "header_pooled_list.h"

/* Define tests within this suite */
CU_TestInfo pl_test_array[] = {

    {"Object creation/deletion                    ", test_pl_create_delete   },
    {"pl_create with invalid input                ", test_pl_create_invalid  },
    {"Different msg sizes                         ", test_pl_multisize       },
    
    {"Adding nodes                                ", test_pl_addnodes        },
    {"pl_addnode with invalid input               ", test_pl_addnode_invalid },
    {"Growth of memory pool                       ", test_pl_memblocks       }, 
    
    {"Single-block list traversal                 ", test_pl_traverse        },
    {"Multi-block list traversal                  ", test_pl_traverse2       },
    {"Getting node by index                       ", test_pl_getnode         },
    {"Randomising entries                         ", test_pl_randomise       },
    
    {"Recycling memory blocks                     ", test_pl_recycle         },
    {"Resetting memory blocks                     ", test_pl_reset           },
    
    
    CU_TEST_INFO_NULL,
};
    
int init_pl(void) {
    return 0;
}

int clean_pl(void) {
    return 0;
}

CU_ErrorCode testsuite_pl(void) {
   
    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"Pooled List ADT", init_pl, clean_pl, pl_test_array},
        CU_SUITE_INFO_NULL,
    };
    
    return CU_register_suites(suites);
    
}

