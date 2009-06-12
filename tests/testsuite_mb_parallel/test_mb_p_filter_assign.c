/* $Id $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_mb_parallel.h"

/* filter function to be used for testing */
static int filter_even(const void *msg, int pid);

void test_mb_p_filter_assign(void) {

    MBIt_Board  *board;
    MBt_Board mb;
    MBt_Filter fh;
    int rc;
    
    /* Create filter */
    rc = MB_Filter_Create(&fh, &filter_even);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL(fh, MB_NULL_FILTER);
    
    /* assigning filter to null board */
    mb = MB_NULL_MBOARD;
    rc = MB_Filter_Assign(mb, fh);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* assigning to a invalid board */
    mb = 999999;
    rc = MB_Filter_Assign(mb, fh);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    
    /* create a board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    CU_ASSERT_PTR_NOT_NULL(board);
    
    /* assign invalid filter to board */
    rc = MB_Filter_Assign(mb, 999999);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* assign to locked board */
    board->locked = MB_TRUE;
    rc = MB_Filter_Assign(mb, fh);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    
    /* unlock the board. should succeed now */
    board->locked = MB_FALSE;
    rc = MB_Filter_Assign(mb, fh);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->filter, (MBIt_filterfunc)&filter_even);
    
    /* unassigning by replacing with MB_NULL_FILTER */
    rc = MB_Filter_Assign(mb, MB_NULL_FILTER);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->filter, (MBIt_filterfunc)NULL);
    
    /* delete objects */
    rc = MB_Filter_Delete(&fh);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(fh, MB_NULL_FILTER);
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
}


static int filter_even(const void *msg, int pid) {
    dummy_msg *m;
    
    assert(msg != NULL);
    
    m = (dummy_msg*)msg;
    
    if (m->ernet % 2 == 0) return 1;
    else return 0;
}
