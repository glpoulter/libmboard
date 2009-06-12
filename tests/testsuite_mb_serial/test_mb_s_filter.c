/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */
#include "header_mb_serial.h"

void test_mb_s_filter_create(void) {
    
    int rc;
    MBt_Filter fh;
    
    rc = MB_Filter_Create(&fh, NULL);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(fh, MB_NULL_FILTER);
}


void test_mb_s_filter_assign(void) {
    
    int rc;
    MBt_Board mb;
    MBt_Filter fh;
    MBIt_Board *board;
    
    /* does not matter what fh we use */
    fh = MB_NULL_FILTER;
    
    /* try assigning fh to null board */
    mb = MB_NULL_MBOARD;
    rc = MB_Filter_Assign(mb, fh);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* try assigning fh to non-existant board */
    mb = 9999999;
    rc = MB_Filter_Assign(mb, fh);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* create a board */
    rc = MB_Create(&mb, sizeof(int));
    if (rc != MB_SUCCESS) CU_FAIL("Unable to create test board");
    
    /* get reference to board object */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    /* try assigning to locked board */
    board->locked = MB_TRUE;
    rc = MB_Filter_Assign(mb, fh);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    
    /* unlock board */
    board->locked = MB_FALSE;
    
    /* a proper call */
    rc = MB_Filter_Assign(mb, fh);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    rc = MB_Delete(&mb);
    if (rc != MB_SUCCESS) CU_FAIL("Unable to delete test board");
    
}

void test_mb_s_filter_delete(void) {
    
    int rc;
    MBt_Filter fh;
    
    /* this routine does not do much */
    rc = MB_Filter_Delete(&fh);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(fh, MB_NULL_FILTER);
}
