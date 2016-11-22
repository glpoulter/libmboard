/* $Id: test_cu_updatecursor.c 2108 2009-09-14 14:14:31Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Aug 2009
 * 
 */
#include "header_commutils.h"

void test_cu_updatecursor(void) {
    
    int rc;
    MBIt_Board *board; /* mock board */
    
    board = mock_board_create();
    assert(board != NULL);
    assert(board->data != NULL);
    assert(board->synced_cursor == 0);
    assert(board->data->count_current == 0);

    /* update cursor of empty board */
    rc = MBI_CommUtil_UpdateCursor(board);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->synced_cursor, 0);
    CU_ASSERT_EQUAL(board->data->count_current, 0);
    
    /* standard update */
    mock_board_populate(board, 100);
    CU_ASSERT_EQUAL(board->data->count_current, 100);
    rc = MBI_CommUtil_UpdateCursor(board);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->synced_cursor, 100);
    CU_ASSERT_EQUAL(board->data->count_current, 100);
    
    /* second update, no new message */
    rc = MBI_CommUtil_UpdateCursor(board);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->synced_cursor, 100);
    CU_ASSERT_EQUAL(board->data->count_current, 100);
    
    /* another update. New messages */
    mock_board_populate(board, 75);
    rc = MBI_CommUtil_UpdateCursor(board);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->synced_cursor, 175);
    CU_ASSERT_EQUAL(board->data->count_current, 175);
    
    /* clean up */
    mock_board_delete(&board);
    assert(board == NULL);
}
