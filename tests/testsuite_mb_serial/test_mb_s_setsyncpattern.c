/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Sept 2009
 * 
 */

#include "header_mb_serial.h"

/* Test MB_SetSyncPattern */
void test_mb_s_setsyncpattern(void) {
    
    int rc;
    unsigned int matrix;
    unsigned int *mptr;
    MBt_Board mb;
    MBIt_Board *board;
    
    /* set matrix to a proper 1x1 array (serial) */
    matrix = 1;
    mptr = &matrix;
    
    /* testing with invalid board */
    mb = 9999999;
    mptr = &matrix;
    rc = MB_SetSyncPattern(mb, mptr);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* testing with null board */
    mb = MB_NULL_MBOARD;
    rc = MB_SetSyncPattern(mb, mptr);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* create a real board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    board = (MBIt_Board*)MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);

    /* testing with null matrix */
    mptr = NULL;
    rc = MB_SetSyncPattern(mb, mptr);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    mptr = &matrix;
    
    /* testing with a locked board */
    matrix = 0;
    board->locked = MB_TRUE;
    rc = MB_SetSyncPattern(mb, mptr);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE); /* default, no change */
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE); /* default, no change */
    board->locked = MB_FALSE;
    
    /* non-empty board */
    rc = MB_AddMessage(mb, &rc);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    rc = MB_SetSyncPattern(mb, mptr);
    CU_ASSERT_EQUAL(rc, MB_ERR_NOTREADY);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE); /* default, no change */
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE); /* default, no change */
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* -- now do it proper -- */
    
    /* 0 val */
    matrix = 0;
    rc = MB_SetSyncPattern(mb, mptr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_FALSE);
    CU_ASSERT_EQUAL(board->is_writer, MB_FALSE);
    
    /* non-0 value */
    matrix = 1;
    rc = MB_SetSyncPattern(mb, mptr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE);
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE);
    
    /* another non-0 value */
    matrix = 99299;
    rc = MB_SetSyncPattern(mb, mptr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE);
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE);
    
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}
