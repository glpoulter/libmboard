/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_mb_serial.h"

/* Test MB_Iterator_Rewind */
void test_mb_s_iter_rewind(void) {
    
    int rc, checkval;
    MBt_Board mb;
    MBt_Iterator itr;
    MBIt_Iterator *iter_obj;
    void *obj;
    
    /* testing invalid iterator */
    itr = 999999;
    rc = MB_Iterator_Rewind(itr);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* testing null iterator */
    itr = MB_NULL_ITERATOR;
    rc = MB_Iterator_Rewind(itr);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Create empty iterator/board */
    rc = MB_Create(&mb, sizeof(dummy_msg));
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    rc = MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    /* rewind empty iterator */
    rc = MB_Iterator_Rewind(itr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* check internal values */
    iter_obj = (MBIt_Iterator *)MBI_getIteratorRef(itr);
    CU_ASSERT_PTR_NOT_NULL_FATAL(iter_obj);
    CU_ASSERT_EQUAL(iter_obj->iterating, 0);
    CU_ASSERT_PTR_NULL(iter_obj->cursor);
    
    /* clean up */
    iter_obj = NULL;
    MB_Iterator_Delete(&itr);
    MB_Delete(&mb);
    
    /* create and populate board */
    rc = init_mb_with_content(&mb);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* create iterator */
    rc = MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* check internal values (before starting iteration) */
    iter_obj = (MBIt_Iterator *)MBI_getIteratorRef(itr);
    CU_ASSERT_PTR_NOT_NULL_FATAL(iter_obj);
    CU_ASSERT_EQUAL(iter_obj->iterating, 0);
    CU_ASSERT_PTR_NULL(iter_obj->cursor);
    
    /* get first message */
    rc = MB_Iterator_GetMessage(itr, &obj);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(iter_obj->iterating, 1);
    CU_ASSERT_PTR_NOT_NULL(iter_obj->cursor);
    checkval = ((dummy_msg *)obj)->ernet;

    while(obj) { /* iterate till the end */
        free(obj); /* release mem */
        MB_Iterator_GetMessage(itr, &obj);
    } 
    
    /* rewind and check content */
    rc = MB_Iterator_Rewind(itr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(iter_obj->iterating, 0);
    CU_ASSERT_PTR_NULL(iter_obj->cursor);
    
    /* get another object and compare with obj_first */
    rc = MB_Iterator_GetMessage(itr, &obj);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(checkval, ((dummy_msg *)obj)->ernet);
    
    /* clean up */
    free(obj);
    MB_Iterator_Delete(&itr);
    MB_Delete(&mb);
}
