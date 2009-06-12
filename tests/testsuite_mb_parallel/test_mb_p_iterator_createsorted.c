/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */

#include "header_mb_parallel.h"


/* File global var */
MBt_Board    mb_s  = (MBt_Board)MB_NULL_MBOARD;
MBt_Iterator itr_s = (MBt_Iterator)MB_NULL_ITERATOR;


/* sort function used to create Sorted Iterator */
static int my_sort(const void *msg1, const void *msg2) {
    /* returns 0 for equal, 1 for more-than, -1 for less than */
    dummy_msg *m1, *m2;
    m1 = (dummy_msg *)msg1;
    m2 = (dummy_msg *)msg2;
    
    if (m1->ernet > m2->ernet) return 1;
    else if (m1->ernet < m2->ernet) return -1;
    else return 0;
}

/* Test MB_Iterator_CreateSorted */
void test_mb_p_iter_create_sorted(void) {
    
    int rc;
    MBIt_Board  *board;
    MBIt_Iterator  *iterator;
    
    /* Try invalid mboard */
    mb_s = 99999999;
    rc = MB_Iterator_CreateSorted(mb_s, &itr_s, &my_sort);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(itr_s, MB_NULL_ITERATOR);
    
    /* Try NULL mboard */
    mb_s = MB_NULL_MBOARD;
    rc = MB_Iterator_CreateSorted(mb_s, &itr_s, &my_sort);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(itr_s, MB_NULL_ITERATOR);
    
    /* Populate mboard. Abort on failure */
    rc = init_mb_with_content(&mb_s);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* Try locked board */
    board = (MBIt_Board *)MBI_getMBoardRef(mb_s);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    board->locked = MB_TRUE;
    rc = MB_Iterator_CreateSorted(mb_s, &itr_s, &my_sort);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    board->locked = MB_FALSE; 
    
    
    /* Create sorted Iterator */
    itr_s = MB_NULL_ITERATOR;
    rc = MB_Iterator_CreateSorted(mb_s, &itr_s, &my_sort);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL(itr_s, MB_NULL_ITERATOR);
    
    iterator = (MBIt_Iterator *)MBI_getIteratorRef(itr_s);
    board = (MBIt_Board *)MBI_getMBoardRef(mb_s);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board->data);
    CU_ASSERT_PTR_NOT_NULL_FATAL(iterator);
    CU_ASSERT_EQUAL((int)board->data->elem_size, iterator->msgsize);
    CU_ASSERT_EQUAL(iterator->iterating, 0);
    CU_ASSERT_PTR_NULL(iterator->cursor);
    CU_ASSERT_EQUAL(iterator->mb, mb_s);
}

/* Test MB_Iterator_GetMessage on Sorted Iterator */
void test_mb_p_iter_sorted_getmsg(void) {
    int rc, errfound, i, prev;
    dummy_msg *msg_ptr;
    void *obj;
    
    if (mb_s == MB_NULL_MBOARD || itr_s == MB_NULL_ITERATOR)
    {
        CU_FAIL("mboard/iterator not yet created");
        return;
    }
    
    /* try getting message from  sorted iterator */
    errfound = 0;
    prev = -1;
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        rc = MB_Iterator_GetMessage(itr_s, &obj);
        if (rc != MB_SUCCESS || obj == NULL) errfound++;
        
        msg_ptr = (dummy_msg*)obj;

        if (msg_ptr->ernet != i + 1) errfound++;
        
        free(obj);
        
        if (errfound != 0) break;
        
    }
    CU_ASSERT_EQUAL(errfound, 0);
    
    
    /* next message should be empty */
    rc = MB_Iterator_GetMessage(itr_s, &obj);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(obj);
    
    
    /* We're done. Delete iterator */
    rc = MB_Iterator_Delete(&itr_s);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(itr_s, MB_NULL_ITERATOR);
    
    /* Delete mboard */
    rc = MB_Delete(&mb_s);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb_s, MB_NULL_MBOARD);
}
