/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : July 2008
 * 
 */

#include "header_tag_table.h"

/* test object creation and deletion */
void test_tt_new_destroy(void) {
    
    int rc;
    int rows = 101;
    int cols = 101;
    tag_table *tt = NULL;
    
    /* test proper creating */
    rc = tt_create(&tt, rows, cols);
    CU_ASSERT_EQUAL_FATAL(rc, TT_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(tt);
    CU_ASSERT_PTR_NOT_NULL_FATAL(tt->table);
    CU_ASSERT_EQUAL(tt->rows, rows);
    CU_ASSERT_EQUAL(tt->cols, cols);
    CU_ASSERT((int)tt->row_size == (int)ceil((double)cols / 8));
    
    
    /* test deletion */
    rc = tt_delete(&tt);
    CU_ASSERT_EQUAL(rc, TT_SUCCESS);
    CU_ASSERT_PTR_NULL(tt);
    
    /* test erronous creations */
    rc = tt_create(&tt, rows, 0);
    CU_ASSERT_EQUAL(rc, TT_ERR_INVALID);
    CU_ASSERT_PTR_NULL(tt);
    
    rc = tt_create(&tt, 0, cols);
    CU_ASSERT_EQUAL(rc, TT_ERR_INVALID);
    CU_ASSERT_PTR_NULL(tt);
    
    /* test erronous deletion */
    tt = NULL;
    rc = tt_delete(&tt);
    CU_ASSERT_EQUAL(rc, TT_ERR_INVALID);
    CU_ASSERT_PTR_NULL(tt);
}


