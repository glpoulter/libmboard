/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */

#include "header_mb_parallel.h"

/* test clearing message board */
void test_mb_p_clear(void) {
    
    int i, rc, errfound;
    int count = (int)(MB_CONFIG_PARALLEL_POOLSIZE * 2.5);
    MBt_Board mb;
    dummy_msg msg;
    MBIt_Board  *board;
    
    /* Clearing NULL message board */
    mb = MB_NULL_MBOARD;
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Clearing invalid message board */
    mb = -99;
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Clearing non-existent message board */
    mb = 99999;
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    
    
    /* create message board */
    rc = MB_Create(&mb, sizeof(dummy_msg));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    
    /* Clearing empty message board */
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Add messages */
    errfound = 0;
    for (i = 0; i < count; i++)
    {
        /* assign dummy value */
        msg.ernet = i;
        
        /* add message to board */
        rc = MB_AddMessage(mb, &msg);
        if (rc != MB_SUCCESS) errfound++;
    }
    CU_ASSERT_EQUAL(errfound, 0);
    
    

    /* Lock the message board */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    board->locked = MB_TRUE;
    
    /* Try clearing locked board */
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    CU_ASSERT_EQUAL((int)board->data->count_current, count);
    
    /* unlock the board */
    board->locked = MB_FALSE;
    
    /* Clear board */
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* board should be empty */
    CU_ASSERT_PTR_NOT_NULL_FATAL(board->data);
    CU_ASSERT_EQUAL(board->data->count_current, 0);
    CU_ASSERT_EQUAL((int)board->synced_cursor, 0);
    
    /* Clear empty board */
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* board should still be empty */
    CU_ASSERT_PTR_NOT_NULL_FATAL(board->data);
    CU_ASSERT_EQUAL(board->data->count_current, 0);
   
    /* Delete board */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
}
