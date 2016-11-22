/* $Id: test_mb_s_iterator.c 2925 2012-07-20 14:12:17Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_mb_serial.h"

/* File global var */
MBt_Board    mb  = (MBt_Board)MB_NULL_MBOARD;
MBt_Iterator itr = (MBt_Iterator)MB_NULL_ITERATOR;

/* Test MB_Iterator_Create */
void test_mb_s_iter_create(void) {
    
    int rc;
    MBIt_Board  *board;
    MBIt_Iterator *iterator;
    
    /* Try invalid mboard */
    mb = 99999999;
    rc = MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(itr, MB_NULL_ITERATOR);
    
    /* Try NULL mboard */
    mb = MB_NULL_MBOARD;
    rc = MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(itr, MB_NULL_ITERATOR);
    
    /* Populate mboard. Abort on failure */
    rc = init_mb_with_content(&mb);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* Try locked board */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    board->locked = MB_TRUE; /* artificially lock the board */
    rc = MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    board->locked = MB_FALSE; /* unlock */
    
    /* Try on "unreadable" boards */
    board->is_reader = MB_FALSE;
    rc = MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL(rc, MB_ERR_DISABLED);
    CU_ASSERT_EQUAL(itr, MB_NULL_ITERATOR);
    board->is_reader = MB_TRUE;
    
    /* create standard iterator */
    itr = MB_NULL_ITERATOR;
    rc = MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL(itr, MB_NULL_ITERATOR);
    iterator = (MBIt_Iterator *)MBI_getIteratorRef(itr);
    CU_ASSERT_PTR_NOT_NULL(iterator);
    CU_ASSERT_EQUAL(board->data->elem_size, iterator->msgsize);
    CU_ASSERT_EQUAL(iterator->iterating, 0);
    CU_ASSERT_PTR_NULL(iterator->cursor);
    CU_ASSERT_EQUAL(iterator->mb, mb);
    
}

/* Test MB_Iterator_GetMessage */
void test_mb_s_iter_getmsg(void) {
    
    int rc, errfound, i;
    dummy_msg *msg_ptr;
    MBt_Iterator itr2;
    void *obj;
    MBt_Board mb_empty;
    
    if (mb == MB_NULL_MBOARD || itr == MB_NULL_ITERATOR)
    {
        CU_FAIL("mboard/iterator not yet created");
        return;
    }
    
    /* try invalid iterator */
    itr2 = 99999;
    rc = MB_Iterator_GetMessage(itr2, &obj);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* try null iterator */
    itr2 = MB_NULL_ITERATOR;
    rc = MB_Iterator_GetMessage(itr2, &obj);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Try empty board/iterator */
    rc = MB_Create(&mb_empty, sizeof(dummy_msg));
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    rc = MB_Iterator_Create(mb_empty, &itr2);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_Iterator_GetMessage(itr2, &obj);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(obj);
    
    /* Delete iterator and board */
    rc = MB_Iterator_Delete(&itr2);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(itr2, MB_NULL_ITERATOR);

    rc = MB_Delete(&mb_empty);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb_empty, MB_NULL_MBOARD);
    
    /* try locked board */
    /* WE DON'T HANDLE LOCKED BOARD
     * Too much overhead. Let users be responsible for it
    obj = MBI_objmap_getobj(MBI_OM_mboard, (int)mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(obj);
    board = (MBIt_Board *)obj;
    board->locked = MB_TRUE; 
    rc = MB_Iterator_GetMessage(itr2, (void *)&msg);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    board->locked = MB_FALSE; 
    */
    
    
    /* Try valid iterator */
    errfound = 0;
    obj = NULL;
    for (i = 0; i < SERIAL_TEST_MSG_COUNT; i++)
    {
        rc = MB_Iterator_GetMessage(itr, &obj);
        if (rc != MB_SUCCESS || obj == NULL) 
        {
            errfound++;
            if (obj) free(obj);
            return;
        }
        
        msg_ptr = (dummy_msg*)obj;
        if (msg_ptr->ernet != get_message_value_from_id(i)) errfound++;
        if (msg_ptr->jeopardy != (double)get_message_value_from_id(i)) errfound++;
        
        free(obj);
        
        if (errfound != 0) break;
        
    }
    CU_ASSERT_EQUAL(errfound, 0);

    /* next message should be empty */
    rc = MB_Iterator_GetMessage(itr, &obj);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(obj);
    
}

/* Test MB_Iterator_Delete */
void test_mb_s_iter_delete(void) {
    
    int rc;
    MBt_Iterator itr2;
    
    
    if (mb == MB_NULL_MBOARD || itr == MB_NULL_ITERATOR)
    {
        CU_FAIL("mboard/iterator not yet created");
        return;
    }
    
    /* try invalid iterator */
    itr2 = 999999;
    rc = MB_Iterator_Delete(&itr2);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    
    /* try null iterator */
    itr2 = MB_NULL_ITERATOR;
    rc = MB_Iterator_Delete(&itr2);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Delete actual iterator */
    rc = MB_Iterator_Delete(&itr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(itr, MB_NULL_ITERATOR);
    
    /* Delete mboard */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
    
}
