/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_mb_serial.h"

#define TEST_FILTER_UB (int)(SERIAL_TEST_MSG_COUNT * 0.75)
#define TEST_FILTER_LB (int)(SERIAL_TEST_MSG_COUNT * 0.25)

/* File global var */
MBt_Board    mb_fs  = (MBt_Board)MB_NULL_MBOARD;
MBt_Iterator itr_fs = (MBt_Iterator)MB_NULL_ITERATOR;

/* struct used as input params to filter func */
typedef struct {
    int lb;
    int ub;
} fparams;

/* filter function */
static int my_filter(const void *msg, const void *params) {
    /* return 1 if accepted, 0 if rejected */
    
    dummy_msg *m;
    fparams *p;
    m = (dummy_msg *)msg;
    p = (fparams *)params;
    
    if (m->ernet >= p->lb && m->ernet <= p->ub) return 1;
    else return 0;
}

/* sort function */
static int my_sort(const void *msg1, const void *msg2) {
    /* returns 0 for equal, 1 for more-than, -1 for less than */
    dummy_msg *m1, *m2;
    m1 = (dummy_msg *)msg1;
    m2 = (dummy_msg *)msg2;
    
    if (m1->ernet > m2->ernet) return 1;
    else if (m1->ernet < m2->ernet) return -1;
    else return 0;
}

/* Test MB_Iterator_CreateFilteredSorted */
void test_mb_s_iter_create_filteredsorted(void) {
    
    int rc;
    MBIt_Board  *board;
    MBIt_Iterator  *iterator;
    fparams fp;
    
    fp.lb = TEST_FILTER_LB;
    fp.ub = TEST_FILTER_UB;
        
    /* Try invalid mboard */
    mb_fs = 99999999;
    rc = MB_Iterator_CreateFilteredSorted(mb_fs, &itr_fs, &my_filter, &fp, &my_sort);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(itr_fs, MB_NULL_ITERATOR);
    
    /* Try NULL mboard */
    mb_fs = MB_NULL_MBOARD;
    rc = MB_Iterator_CreateFilteredSorted(mb_fs, &itr_fs, &my_filter, &fp, &my_sort);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(itr_fs, MB_NULL_ITERATOR);
    
    /* Populate mboard. Abort on failure */
    rc = init_mb_with_content(&mb_fs);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* Try locked board */
    board = (MBIt_Board *)MBI_getMBoardRef(mb_fs);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    board->locked = MB_TRUE;
    rc = MB_Iterator_CreateSorted(mb_fs, &itr_fs, &my_sort);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    board->locked = MB_FALSE; 
    
    /* Create filtered+sorted Iterator */
    itr_fs = MB_NULL_ITERATOR;
    rc = MB_Iterator_CreateFilteredSorted(mb_fs, &itr_fs, &my_filter, &fp, &my_sort);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL(itr_fs, MB_NULL_ITERATOR);
    
    board = (MBIt_Board *)MBI_getMBoardRef(mb_fs);
    iterator = (MBIt_Iterator *)MBI_getIteratorRef(itr_fs);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(iterator);
    CU_ASSERT_EQUAL(board->data->elem_size, iterator->msgsize);
    CU_ASSERT_EQUAL(iterator->iterating, 0);
    CU_ASSERT_PTR_NULL(iterator->cursor);
    CU_ASSERT_EQUAL(iterator->mb, mb_fs);
    
}

/* Test MB_Iterator_GetMessage on Filtered+Sorted Iterator */
void test_mb_s_iter_filteredsorted_getmsg(void) {
    
    int rc, errfound, i, count;
    dummy_msg *msg_ptr;
    MBIt_Board *board;
    MBIt_Iterator *iter;
    void *obj;
    
    if (mb_fs == MB_NULL_MBOARD || itr_fs == MB_NULL_ITERATOR)
    {
        CU_FAIL("mboard/iterator not yet created");
        return;
    }
    
    /* manually check all messages to see how many messages fit the bill */
    board = (MBIt_Board *)MBI_getMBoardRef(mb_fs);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    count = 0;
    for (i = 0; i < (int)board->data->count_current; i++)
    {
        rc = pl_getnode(board->data, i, &obj);
        if (rc != PL_SUCCESS) CU_FAIL("corrupted board?");
        msg_ptr = (dummy_msg *)obj;
        if (msg_ptr->ernet >= TEST_FILTER_LB && \
                msg_ptr->ernet <= TEST_FILTER_UB) count++;
    }
    
    /* check number of elems in iterator */
    iter = (MBIt_Iterator *)MBI_getIteratorRef(itr_fs);
    CU_ASSERT_PTR_NOT_NULL_FATAL(iter);
    CU_ASSERT_EQUAL(count, (int)iter->data->count_current);
   
    /* check values in iterator */
    errfound = 0;
    for (i = 0; i < count; i++)
    {
        rc = MB_Iterator_GetMessage(itr_fs, &obj);
        if (rc != MB_SUCCESS || obj == NULL) errfound++;
        
        msg_ptr = (dummy_msg*)obj;
        
        if (msg_ptr->ernet != i + TEST_FILTER_LB) errfound++;
        
        free(obj);
        
        if (errfound != 0) break;
        
    }
    CU_ASSERT_EQUAL(errfound, 0);
    
    /* next message should be empty */
    rc = MB_Iterator_GetMessage(itr_fs, &obj);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(obj);
    
    /* We're done. Delete iterator */
    rc = MB_Iterator_Delete(&itr_fs);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(itr_fs, MB_NULL_ITERATOR);
    
    /* Delete mboard */
    rc = MB_Delete(&mb_fs);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb_fs, MB_NULL_MBOARD);
}
