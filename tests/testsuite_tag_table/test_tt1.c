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

/* test test_tt_setbits() */
void test_tt_setbits(void) {
	
	int i, rc;
    int rows = 10;
    int cols = rows * 8;
    tag_table *tt = NULL;
    char *row_ptr, *octet_ptr;
    char octet1 = (char)0x82;
    char octet2 = (char)0x91;
    char octet;
    
    /* create the tt object */
    rc = tt_create(&tt, rows, cols);
    CU_ASSERT_EQUAL_FATAL(rc, TT_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(tt);
    
    /* systematically set various bits within the table */
    for (i = 0; i < rows; i++)
    {
    	rc = tt_setbits(tt, i, i, octet1);
    	if (rc != TT_SUCCESS) CU_FAIL("tt_setbits() returned an error code");
    	
    	rc = tt_setbits(tt, i, (i + 1) % 8, octet1);
    	if (rc != TT_SUCCESS) CU_FAIL("tt_setbits() returned an error code");
    	
    	rc = tt_setbits(tt, i, (i + 1) % 8, octet2);
    	if (rc != TT_SUCCESS) CU_FAIL("tt_setbits() returned an error code");
    	
    	rc = tt_setbits(tt, i, (i + 2) % 8, octet2);
    	if (rc != TT_SUCCESS) CU_FAIL("tt_setbits() returned an error code");
    }
    
    /* check those bits */
    for (i = 0; i < rows; i++)
    {
    	rc = tt_getrow(tt, i, &row_ptr);
    	if (rc != TT_SUCCESS || row_ptr == NULL) CU_FAIL("tt_getrow() failed");
    	
    	octet_ptr = row_ptr + i;
    	octet = *octet_ptr;
    	if (octet != octet1) CU_FAIL("matching of first octet failed");
    	
    	octet_ptr = row_ptr + ((i + 1) % 8);
    	octet = *octet_ptr;
    	if (octet != (octet1 | octet2)) CU_FAIL("matching of second octet failed");
    	
    	octet_ptr = row_ptr + ((i + 2) % 8);
    	octet = *octet_ptr;
    	if (octet != octet2) CU_FAIL("matching of third octet failed");
    }
    
    
    /* destroy tt object */
    rc = tt_delete(&tt);
    CU_ASSERT_EQUAL(rc, TT_SUCCESS);
    CU_ASSERT_PTR_NULL(tt);
}


/* test test_tt_setbyte() */
void test_tt_setbyte(void) {
	
	int i, rc;
    int rows = 10;
    int cols = rows * 8;
    tag_table *tt = NULL;
    char *row_ptr, *octet_ptr;
    char octet1 = (char)0x82;
    char octet2 = (char)0x91;
    char octet;
    
    /* create the tt object */
    rc = tt_create(&tt, rows, cols);
    CU_ASSERT_EQUAL_FATAL(rc, TT_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(tt);
    
    /* systematically set various bits within the table */
    for (i = 0; i < rows; i++)
    {
    	rc = tt_setbyte(tt, i, i, octet1);
    	if (rc != TT_SUCCESS) CU_FAIL("tt_setbits() returned an error code");
    	
    	rc = tt_setbyte(tt, i, (i + 1) % 8, octet1);
    	if (rc != TT_SUCCESS) CU_FAIL("tt_setbits() returned an error code");
    	
    	rc = tt_setbyte(tt, i, (i + 1) % 8, octet2);
    	if (rc != TT_SUCCESS) CU_FAIL("tt_setbits() returned an error code");
    	
    	rc = tt_setbyte(tt, i, (i + 2) % 8, octet2);
    	if (rc != TT_SUCCESS) CU_FAIL("tt_setbits() returned an error code");
    }
    
    /* check those bits */
    for (i = 0; i < rows; i++)
    {
    	rc = tt_getrow(tt, i, &row_ptr);
    	if (rc != TT_SUCCESS || row_ptr == NULL) CU_FAIL("tt_getrow() failed");
    	
    	octet_ptr = row_ptr + i;
    	octet = *octet_ptr;
    	if (octet != octet1) CU_FAIL("matching of first octet failed");
    	
    	octet_ptr = row_ptr + ((i + 1) % 8);
    	octet = *octet_ptr;
    	if (octet != octet2) CU_FAIL("matching of second octet failed");
    	
    	octet_ptr = row_ptr + ((i + 2) % 8);
    	octet = *octet_ptr;
    	if (octet != octet2) CU_FAIL("matching of third octet failed");
    }
    
    
    /* destroy tt object */
    rc = tt_delete(&tt);
    CU_ASSERT_EQUAL(rc, TT_SUCCESS);
    CU_ASSERT_PTR_NULL(tt);
}

/* test test_tt_getcount_row() */
void test_tt_getcount_row(void) {
	
	int i, rc;
	int count = 999;
    int rows = 10;
    int cols = rows * 8;
    tag_table *tt = NULL;
    char octet1 = (char)0x09;
    char octet2 = (char)0x04;
    
    /* create the tt object */
    rc = tt_create(&tt, rows, cols);
    CU_ASSERT_EQUAL_FATAL(rc, TT_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(tt);
    
    /* check count for newly created table */
    for (i = 0; i < rows; i++)
    {
    	rc = tt_getcount_row(tt, i, &count);
    	if (rc != TT_SUCCESS) CU_FAIL("tt_getcount() returned with an error");
    	if (count != 0) CU_FAIL("count != 0");
    }
    
    /* systematically set various bits within the table */
    for (i = 0; i < rows; i++)
    {
    	rc = tt_setbyte(tt, i, i, octet1);
    	if (rc != TT_SUCCESS) CU_FAIL("tt_setbits() returned an error code");
    	
    	if (i % 2 == 0)
    	{
    		rc = tt_setbyte(tt, i, (i + 2) % 8, octet2);
    		if (rc != TT_SUCCESS) CU_FAIL("tt_setbits() returned an error code");
    	}
    	
    }
    
    /* recheck count for table */
    for (i = 0; i < rows; i++)
    {
    	rc = tt_getcount_row(tt, i, &count);
    	if (rc != TT_SUCCESS) CU_FAIL("tt_getcount() returned with an error");
    	
    	if (i % 2 == 0)
    	{
    		if (count != 3) CU_FAIL("count != 3");
    	}
    	else
    	{
    		if (count != 2) CU_FAIL("count != 2");
    	}
    }
    
    /* destroy tt object */
    rc = tt_delete(&tt);
    CU_ASSERT_EQUAL(rc, TT_SUCCESS);
    CU_ASSERT_PTR_NULL(tt);
}

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
