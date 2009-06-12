/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_mb_serial.h"

/* Test MB_SyncStart */
void test_mb_s_sync_start(void) {
    
    int rc;
    MBt_Board mb;
    
    /* Testing with invalid board */
    mb = 999999;
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Testing null board */
    mb = MB_NULL_MBOARD;
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    rc = MB_Create(&mb, sizeof(dummy_msg));
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* Start synchronisation */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* We should not be able to clear this board (locked) */
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    
    /* Clean up and quit */
    MB_SyncComplete(mb);
    rc = MB_Delete(&mb);
}


/* Test MB_SyncTest */
void test_mb_s_sync_test(void) {
    
    int rc;
    int flag;
    MBt_Board mb;
    MBIt_Board  *board;
    
    /* Testing with invalid board */
    mb = 999999;
    rc = MB_SyncTest(mb, &flag);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(flag, MB_FALSE);
    
    mb = -9;
    rc = MB_SyncTest(mb, &flag);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(flag, MB_FALSE);
    
    /* Testing null board */
    mb = MB_NULL_MBOARD;
    rc = MB_SyncTest(mb, &flag);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(flag, MB_TRUE);
    
    rc = MB_Create(&mb, sizeof(dummy_msg));
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* get reference to board object */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    
    /* testing a board that has not been synched is invalid */
    rc = MB_SyncTest(mb, &flag);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(flag, MB_FALSE);
    
    /* Start synchronisation */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* synchronisation in serial completes immediately after starting */
    rc = MB_SyncTest(mb, &flag);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(flag, MB_TRUE);
    
    /* board should be unlocked */
    CU_ASSERT_EQUAL(board->locked, MB_FALSE);
    
    /* Clean up and quit */
    rc = MB_Delete(&mb);
}



/* Test MB_SyncComplete */
void test_mb_s_sync_complete(void) {
    
    int rc;
    MBt_Board mb;
    
    /* Testing with invalid board */
    mb = 999999;
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    mb = -9;
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Testing null board */
    mb = MB_NULL_MBOARD;
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    rc = MB_Create(&mb, sizeof(dummy_msg));
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* try completeing sync that has not yet started */
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Start synchronisation */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* We should not be able to clear this board (locked) */
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    
    /* Complete sync */
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* We should now be able to clear this board */
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Should not have anything to complete */
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Clean up and quit */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);

}

