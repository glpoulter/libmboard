/* $Id: test_tt_getrow.c 1871 2009-06-12 13:58:18Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : July 2008
 * 
 */

#include "header_tag_table.h"


/* test tt_getrow() */
void test_tt_getrow(void) {
    
    int i, j;
    int rc;
    int rows = 101;
    int cols = 101;
    tag_table *tt = NULL;
    char *table_ptr, *expected_row_ptr;
    char *row_ptr, *octet_ptr, octet;
    
    /* create the tt object */
    rc = tt_create(&tt, rows, cols);
    CU_ASSERT_EQUAL_FATAL(rc, TT_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(tt);
    
    /* Get base address of table data */
    table_ptr = tt->table;
    CU_ASSERT_PTR_NOT_NULL_FATAL(table_ptr);
    
    for (i = 0; i < rows; i++)
    {
        expected_row_ptr = table_ptr + (tt->row_size * i);
        rc = tt_getrow(tt, i, &row_ptr);
        
        /* check return values are reasonable */
        if (rc != TT_SUCCESS || row_ptr == NULL) CU_FAIL("tt_getrow() failed");
        
        /* check returned address */
        if (row_ptr != expected_row_ptr) CU_FAIL("tt_getrow() failed");
        
        /* extra check which should have been done previously, but we need to use
         * tt_getrow() which only now has been tested.
         */
        /* check that all bits in the table have been set to zeros */
        for (j = 0; j < (int)tt->row_size; j++)
        {
            octet_ptr = row_ptr + j;
            octet = *octet_ptr;
            
            if (octet != 0x00) CU_FAIL("Not all bits were initialised to Zero");
        }
    }
    
    /* destroy tt object */
    rc = tt_delete(&tt);
    CU_ASSERT_EQUAL(rc, TT_SUCCESS);
    CU_ASSERT_PTR_NULL(tt);
}

