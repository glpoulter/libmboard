/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_mb_serial.h"

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


/* Test MB_Iterator_Randomise */
void test_mb_s_iter_randomise(void) {
    
    int rc, count, match;
    void *obj;
    MBt_Board mb;
    pooled_list *pl;
    MBt_Iterator itr;
    MBIt_Iterator *iter_obj;
    pl_address_node *addrnode;
    dummy_msg *msg;
    
    /* testing invalid iterator */
    itr = 999999;
    rc = MB_Iterator_Randomise(itr);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* testing null iterator */
    itr = MB_NULL_ITERATOR;
    rc = MB_Iterator_Randomise(itr);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Create empty iterator/board */
    rc = MB_Create(&mb, sizeof(dummy_msg));
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    rc = MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    /* randomise empty iterator */
    rc = MB_Iterator_Randomise(itr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* clean up */
    MB_Iterator_Delete(&itr);
    MB_Delete(&mb);
    
    /* Populate mboard. Abort on failure */
    rc = init_mb_with_content(&mb);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* create sorted iterator */
    itr = MB_NULL_ITERATOR;
    rc = MB_Iterator_CreateSorted(mb, &itr, &my_sort);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL_FATAL(itr, MB_NULL_ITERATOR);
    
    /* get a few messages so iterator is not in initial position */
    rc = MB_Iterator_GetMessage(itr, &obj);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(obj);
    free(obj);
    rc = MB_Iterator_GetMessage(itr, &obj);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(obj);
    free(obj);
    
    /* randomise it.. check order */
    rc = MB_Iterator_Randomise(itr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* internal pointers should have been reset */
    iter_obj = (MBIt_Iterator *)MBI_getIteratorRef(itr);
    CU_ASSERT_PTR_NOT_NULL_FATAL(iter_obj);
    CU_ASSERT_EQUAL(iter_obj->iterating, 0);
    CU_ASSERT_PTR_NULL(iter_obj->cursor);
    
    /* Traverse iterator.
     * - check values to make sure order has been randomised
     * - also confirms that linked list is still intact
     */
    count = 0;
    match = 0;
    pl = iter_obj->data;
    for (addrnode = PL_ITERATOR(pl); addrnode; addrnode = addrnode->next)
    {
        count++;
        msg = (dummy_msg *)(PL_NODEDATA(addrnode));
        if (msg->ernet == count) match++; 
    }
    CU_ASSERT_EQUAL(count, SERIAL_TEST_MSG_COUNT);
    CU_ASSERT_NOT_EQUAL(match, count);
    if ((int)((float)match / count * 100) > 20)
    {
        CU_FAIL("More than 20% of messages remain in place");
    }
    
    /* Clean up */
    rc = MB_Iterator_Delete(&itr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
}
