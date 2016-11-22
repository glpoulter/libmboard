/* $Id: setup_suite_kdtree.c 2925 2012-07-20 14:12:17Z lsc $ */
/*
 * Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * Author: Shawn Chin
 * Date  : July 2012
 *
 */
#include "header_kdtree.h"

/* Define tests within this suite */
CU_TestInfo kd_test_array[] = {
	{"Creating and deleting an empty tree         ", test_kd_create_delete_empty },
	{"Creating and deleting a tree                ", test_kd_create_delete },
	{"Search empty tree (1D)                      ", test_kd_search_1d_empty },
	{"Search tree with single entry (1D)          ", test_kd_search_1d_single_item },
	{"Various search patterns (1D)                ", test_kd_search_1d },
	{"Search empty tree (2D)                      ", test_kd_search_2d_empty },
	{"Search tree with single entry (2D)          ", test_kd_search_2d_single_item },
	{"Various search patterns (2D)                ", test_kd_search_2d },
	{"Search empty tree (3D)                      ", test_kd_search_3d_empty },
	{"Search tree with single entry (3D)          ", test_kd_search_3d_single_item },
	{"Various search patterns (3D)                ", test_kd_search_3d },
    CU_TEST_INFO_NULL,
};

int init_kd(void) {
    /* seed RNG */
    srand((unsigned)time(NULL));
    return 0;
}

int clean_kd(void) {
    return 0;
}

CU_ErrorCode testsuite_kd(void) {

    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"Multi-dimensional k-d search tree", init_kd, clean_kd, kd_test_array},
        CU_SUITE_INFO_NULL,
    };

    return CU_register_suites(suites);

}
