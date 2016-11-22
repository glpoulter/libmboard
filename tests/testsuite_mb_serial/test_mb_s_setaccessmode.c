/* $Id: test_mb_s_setaccessmode.c 2108 2009-09-14 14:14:31Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Aug 2009
 * 
 */

#include "header_mb_serial.h"

/* Test MB_SetAccessMode */
void test_mb_s_setaccessmode(void) {
    
    int rc;
    MBt_Board mb;
    MBIt_Board *board;
    
    /* testing with invalid board */
    mb = 9999999;
    rc = MB_SetAccessMode(mb, MB_MODE_READWRITE);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* testing with null board */
    mb = MB_NULL_MBOARD;
    rc = MB_SetAccessMode(mb, MB_MODE_READWRITE);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
   
    /* create a real board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    board = (MBIt_Board*)MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    
    /* testing with an invalid mode */
    rc = MB_SetAccessMode(mb, 10000);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* locked board */
    board->locked = MB_TRUE;
    rc = MB_SetAccessMode(mb, MB_MODE_IDLE);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE); /* default */
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE); /* default */
    board->locked = MB_FALSE;
    
    /* non-empty board */
    rc = MB_AddMessage(mb, &rc);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    rc = MB_SetAccessMode(mb, MB_MODE_IDLE);
    CU_ASSERT_EQUAL(rc, MB_ERR_NOTREADY);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE); /* default */
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE); /* default */
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    
    /* --- cycle through different modes --- */
    /* some tests repeated to make sure both flags are toggled */
    
    rc = MB_SetAccessMode(mb, MB_MODE_IDLE);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_FALSE);
    CU_ASSERT_EQUAL(board->is_writer, MB_FALSE);
    
    rc = MB_SetAccessMode(mb, MB_MODE_READWRITE);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE);
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE);
    
    rc = MB_SetAccessMode(mb, MB_MODE_READONLY);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE);
    CU_ASSERT_EQUAL(board->is_writer, MB_FALSE);
    
    rc = MB_SetAccessMode(mb, MB_MODE_WRITEONLY);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_FALSE);
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE);
    
    rc = MB_SetAccessMode(mb, MB_MODE_READONLY); /* repeat */
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE);
    CU_ASSERT_EQUAL(board->is_writer, MB_FALSE);
    
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
}
