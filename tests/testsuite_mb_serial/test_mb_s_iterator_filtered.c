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
MBt_Board    mb_f  = (MBt_Board)MB_NULL_MBOARD;
MBt_Iterator itr_f = (MBt_Iterator)MB_NULL_ITERATOR;

/* struct used as input params to filter func */
typedef struct {
    int lb;
    int ub;
} filter_params;

/* filter function */
static int my_filter(const void *msg, const void *params) {
    /* return 1 if accepted, 0 if rejected */
    
    dummy_msg *m;
    filter_params *p;
    m = (dummy_msg *)msg;
    p = (filter_params *)params;
    
    if (m->ernet >= p->lb && m->ernet <= p->ub) return 1;
    else return 0;
}

/* Test MB_Iterator_CreateFiltered */
void test_mb_s_iter_create_filtered(void) {
    
    int rc;
    MBIt_Board  *board;
    MBIt_Iterator  *iterator;
    filter_params fp;
    
    fp.lb = TEST_FILTER_LB;
    fp.ub = TEST_FILTER_UB;
        
    /* Try invalid mboard */
    mb_f = 99999999;
    rc = MB_Iterator_CreateFiltered(mb_f, &itr_f, &my_filter, &fp);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(itr_f, MB_NULL_ITERATOR);
    
    /* Try NULL mboard */
    mb_f = MB_NULL_MBOARD;
    rc = MB_Iterator_CreateFiltered(mb_f, &itr_f, &my_filter, &fp);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(itr_f, MB_NULL_ITERATOR);
    
    /* Populate mboard. Abort on failure */
    rc = init_mb_with_content(&mb_f);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* Try locked board */
    board = (MBIt_Board *)MBI_getMBoardRef(mb_f);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    board->locked = MB_TRUE; 
    rc = MB_Iterator_CreateFiltered(mb_f, &itr_f, &my_filter, &fp);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    board->locked = MB_FALSE; 

    
    /* Create sorted Iterator */
    itr_f = MB_NULL_ITERATOR;
    rc = MB_Iterator_CreateFiltered(mb_f, &itr_f, &my_filter, &fp);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL(itr_f, MB_NULL_ITERATOR);
    
    board = (MBIt_Board *)MBI_getMBoardRef(mb_f);
    iterator = (MBIt_Iterator *)MBI_getIteratorRef(itr_f);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(iterator);
    CU_ASSERT_EQUAL(board->data->elem_size, iterator->msgsize);
    CU_ASSERT_EQUAL(iterator->iterating, 0);
    CU_ASSERT_PTR_NULL(iterator->cursor);
    CU_ASSERT_EQUAL(iterator->mb, mb_f);
}

/* Test MB_Iterator_GetMessage on Filtered Iterator */
void test_mb_s_iter_filtered_getmsg(void) {
    int rc, errfound, i, count;
    dummy_msg *msg_ptr;
    MBIt_Board *board;
    MBIt_Iterator *iter;
    void *obj;
    
    if (mb_f == MB_NULL_MBOARD || itr_f == MB_NULL_ITERATOR)
    {
        CU_FAIL("mboard/iterator not yet created");
        return;
    }
    
    /* manually check all messages to see how many messages fit the bill */
    board = (MBIt_Board *)MBI_getMBoardRef(mb_f);
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
    iter = (MBIt_Iterator *)MBI_getIteratorRef(itr_f);
    CU_ASSERT_PTR_NOT_NULL_FATAL(iter);
    CU_ASSERT_EQUAL(count, (int)iter->data->count_current);
   
    /* try getting message from  sorted iterator */
    errfound = 0;
    for (i = 0; i < count; i++)
    {
        rc = MB_Iterator_GetMessage(itr_f, &obj);
        if (rc != MB_SUCCESS || obj == NULL) errfound++;
        
        msg_ptr = (dummy_msg*)obj;
        if (msg_ptr->ernet < TEST_FILTER_LB || \
                msg_ptr->ernet > TEST_FILTER_UB) errfound++;
        
        free(obj);
        
        if (errfound != 0) break;
        
    }
    CU_ASSERT_EQUAL(errfound, 0);
    
    /* next message should be empty */
    rc = MB_Iterator_GetMessage(itr_f, &obj);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(obj);
    
    /* We're done. Delete iterator */
    rc = MB_Iterator_Delete(&itr_f);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(itr_f, MB_NULL_ITERATOR);
    
    /* Delete mboard */
    rc = MB_Delete(&mb_f);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb_f, MB_NULL_MBOARD);
}
