/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : July 2008
 * 
 */

#include "header_tag_table.h"

/* test test_tt_getcount_col() */
void test_tt_getcount_col(void) {
    
    int i, j, rc;
    int count = 999;
    int rows = 10;
    int cols = rows * 8;
    tag_table *tt = NULL;
    char octet1 = (char)0x00;
    char octet2 = (char)0xff;
    
    /* create the tt object */
    rc = tt_create(&tt, rows, cols);
    CU_ASSERT_EQUAL_FATAL(rc, TT_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(tt);
    
    /* check count for newly created table */
    for (i = 0; i < cols; i++)
    {
        rc = tt_getcount_col(tt, i, &count);
        if (rc != TT_SUCCESS) CU_FAIL("tt_getcount() returned with an error");
        if (count != 0) CU_FAIL("count != 0");
    }
    
    /* systematically set various bits within the table */
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < (int)tt->row_size; j++)
        {
            if (i % 2 == 0)
            {
                rc = tt_setbyte(tt, i, j, octet1);
                if (rc != TT_SUCCESS) CU_FAIL("tt_setbits() returned an error code");
            }
            else
            {
                rc = tt_setbyte(tt, i, j, octet2);
                if (rc != TT_SUCCESS) CU_FAIL("tt_setbits() returned an error code");
            }
        }
        
    }
    
    /* recheck count for table */
    for (i = 0; i < cols; i++)
    {
        rc = tt_getcount_col(tt, i, &count);
        if (rc != TT_SUCCESS) CU_FAIL("tt_getcount() returned with an error");
        
        if (count != (rows / 2)) CU_FAIL("count != rows/2");

    }
    
    /* destroy tt object */
    rc = tt_delete(&tt);
    CU_ASSERT_EQUAL(rc, TT_SUCCESS);
    CU_ASSERT_PTR_NULL(tt);
}
