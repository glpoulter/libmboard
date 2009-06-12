/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : July 2008
 * 
 */

#include "header_tag_table.h"

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

