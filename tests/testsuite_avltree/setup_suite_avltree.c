/* $Id: setup_suite_avltree.c 1871 2009-06-12 13:58:18Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2009
 * 
 */
#include "header_avltree.h"

/* Define tests within this suite */
CU_TestInfo avl_test_array[] = {
    
    {"Creating and deleting tree                  ", test_avl_create_delete   },
    {"Inserting new nodes                         ", test_avl_insert_simple   },
    {"Inserting nodes on alternate branches       ", test_avl_insert_zigzag   },
    {"Inserting duplicate nodes                   ", test_avl_insert_duplicate},
    {"Retrieving min/max key value                ", test_avl_getmin_getmax   },
    {"Retrieving data pointer                     ", test_avl_getdata         },
    {"Retrieving node pointer                     ", test_avl_getnode         },
    {"Retrieving non-existant nodes               ", test_avl_getnode_invalid },
    {"Inserting new nodes with attached ptr       ", test_avl_insert_withptr  },
    {"Modifying node value using getnode()        ", test_avl_getnode_modvalue},
    {"Dumping tree content                        ", test_avl_dump            },
    {"Walking the tree                            ", test_avl_walk            },
    {"Quering for specific node values            ", test_avl_memberof        },
    CU_TEST_INFO_NULL,
};
    
int init_avl(void) {
    
    /* seed RNG */
    srand((unsigned)time(NULL));
    
    return 0;
}

int clean_avl(void) {
    return 0;
}

CU_ErrorCode testsuite_avl(void) {
    
    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"AVL balanced binary search tree", init_avl, clean_avl, avl_test_array},
        CU_SUITE_INFO_NULL,
    };
    
    return CU_register_suites(suites);
    
}
