/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */

#include "header_mb_parallel.h"

/* filter functions to be used for testing */
int filter_even(const void *msg, const void *params);
int filter_minimum(const void *msg, const void *params);

/* test registering a function */
void test_mb_p_function_register(void) {
    
    int rc, min; 
    MBt_Function fh, fh2;
    MBIt_filterfunc_wrapper *fwrap;
    dummy_msg msg;
    
    /* init values */
    fh = fh2 = MB_NULL_FUNCTION;
    
    /* add first function */
    rc = MB_Function_Register(&fh, &filter_even);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL(fh, MB_NULL_FUNCTION);
    
    /* add second function */
    rc = MB_Function_Register(&fh2, &filter_minimum);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL(fh2, MB_NULL_FUNCTION);
    
    /* get access to first function */
    fwrap = (MBIt_filterfunc_wrapper*)MBI_getFunctionRef(fh);
    CU_ASSERT_PTR_NOT_NULL_FATAL(fwrap);
    
    /* try running function on dummy msg */
    msg.ernet = 111;
    rc = (fwrap->func)(&msg, NULL);
    CU_ASSERT_EQUAL(rc, 0);
    msg.ernet = 222;
    rc = (fwrap->func)(&msg, NULL);
    CU_ASSERT_EQUAL(rc, 1);
    
    /* get access to second function */
    fwrap = (MBIt_filterfunc_wrapper*)MBI_getFunctionRef(fh2);
    CU_ASSERT_PTR_NOT_NULL_FATAL(fwrap);
    
    /* try running function on dummy msg */
    min = 200;
    msg.ernet = 201;
    rc = (fwrap->func)(&msg, &min);
    CU_ASSERT_EQUAL(rc, 1);
    min = 200;
    msg.ernet = 100;
    rc = (fwrap->func)(&msg, &min);
    CU_ASSERT_EQUAL(rc, 0);

    /* free functions */
    rc = MB_Function_Free(&fh);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(fh, MB_NULL_FUNCTION);
    fwrap = (MBIt_filterfunc_wrapper*)MBI_getFunctionRef(fh);
    CU_ASSERT_PTR_NULL(fwrap);
    
    rc = MB_Function_Free(&fh2);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(fh2, MB_NULL_FUNCTION);
    fwrap = (MBIt_filterfunc_wrapper*)MBI_getFunctionRef(fh2);
    CU_ASSERT_PTR_NULL(fwrap);
    
}


/* test assigning a function to a board */
void test_mb_p_function_assign(void) {
    
    int e, input;
    int i, rc, count;
    MBIt_Board *board;
    MBt_Board mb, mb2;
    MBt_Iterator itr;
    MBt_Function fh_even, fh_min;
    dummy_msg msg, *m;
    void *ptr;
    
    if (MBI_CommSize == 1) 
    {
        CU_PASS("You may want to test with more procs");
        return;
    }
    
    /* test by populating 'count' messages per local board */
    count = 10;
    
    /* create board and populate */
    rc = MB_Create(&mb, sizeof(dummy_msg));
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* testing invalid inputs */
    
    for (i = 0; i < count; i++)
    {
        msg.ernet = i + (count * MBI_CommRank);
        MB_AddMessage(mb, &msg);
    }
    
    /* Register function and assign to board */
    rc = MB_Function_Register(&fh_even, &filter_even);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* ---- firstly, test invalid inputs ----- */
    fh_min = 99999999;
    mb2    = MB_NULL_MBOARD;
    /* invalid board */
    rc = MB_Function_Assign(mb2, fh_even, NULL, 0);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    /* invalid fh */
    rc = MB_Function_Assign(mb, fh_min, NULL, 0);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    /* invalid param size */
    rc = MB_Function_Assign(mb, fh_min, &input, 0);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    /* locked board */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    board->locked = MB_TRUE;
    rc = MB_Function_Assign(mb, fh_even, NULL, 0);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    board->locked = MB_FALSE;

    /* assign proper function */
    rc = MB_Function_Assign(mb, fh_even, NULL, 0);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->fh, fh_even);
    CU_ASSERT_EQUAL(board->tagging, MB_TRUE);
    
    /* now back to bussiness.. sync board */
    MB_SyncStart(mb);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    MB_SyncComplete(mb);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);

    rc = MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* check content */
    e = 0;
    MB_Iterator_GetMessage(itr, &ptr);
    m = (dummy_msg*)ptr;
    while(m)
    {

        /* skip our own messages */
        if ( (m->ernet < count*MBI_CommRank) || 
             (m->ernet >  (count - 1 + (count*MBI_CommRank)) ) )
        {
            if (m->ernet % 2 == 1) e++;
        }
        free(m);
        MB_Iterator_GetMessage(itr, &ptr);
        m = (dummy_msg*)ptr;
    }
    CU_ASSERT_EQUAL(e, 0);
    
    /* clear board and start over */
    MB_Iterator_Delete(&itr);
    MB_Clear(mb);
    
    for (i = 0; i < count; i++)
    {
        msg.ernet = i + (count * MBI_CommRank);
        MB_AddMessage(mb, &msg);
    }
    
    /* REMOVE FUNCTION FILTER */
    rc = MB_Function_Assign(mb, MB_NULL_FUNCTION, NULL, 0);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->fh, MB_NULL_FUNCTION);
    CU_ASSERT_EQUAL(board->tagging, MB_FALSE);
    
    MB_SyncStart(mb);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    MB_SyncComplete(mb);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);

    MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);

    /* check content, we should get all messages */
    e = 0;
    MB_Iterator_GetMessage(itr, &ptr);
    m = (dummy_msg*)ptr;
    while(m)
    {
        e++;
        free(m);
        MB_Iterator_GetMessage(itr, &ptr);
        m = (dummy_msg*)ptr;
    }
    CU_ASSERT_EQUAL(e, count * MBI_CommSize);
    
    /* clear board and start over */
    MB_Iterator_Delete(&itr);
    MB_Clear(mb);
    
    
    for (i = 0; i < count; i++)
    {
        msg.ernet = i + (count * MBI_CommRank);
        MB_AddMessage(mb, &msg);
    }
    
    /* Register function and assign to board */
    rc = MB_Function_Register(&fh_min, &filter_minimum);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    rc = MB_Function_Assign(mb, fh_min, &input, sizeof(int));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->fh, fh_min);
    CU_ASSERT_EQUAL(board->tagging, MB_TRUE);
    
    input = (count * MBI_CommSize) / 2;
    
    MB_SyncStart(mb);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    MB_SyncComplete(mb);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);

    MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* check content */
    e = 0;
    MB_Iterator_GetMessage(itr, &ptr);
    m = (dummy_msg*)ptr;
    while(m)
    {

        /* skip our own messages */
        if ( (m->ernet < count*MBI_CommRank) || 
             (m->ernet >  (count - 1 + (count*MBI_CommRank)) ) )
        {
            if (m->ernet < input) e++;
        }
        free(m);
        MB_Iterator_GetMessage(itr, &ptr);
        m = (dummy_msg*)ptr;
    }
    CU_ASSERT_EQUAL(e, 0);
    
    /* clear board and start over */
    MB_Iterator_Delete(&itr);
    MB_Delete(&mb);
    rc = MB_Function_Free(&fh_even);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_Function_Free(&fh_min);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    

}


int filter_even(const void *msg, const void *params) {
    dummy_msg *m;
    
    assert(msg != NULL);
    assert(params == NULL);
    
    m = (dummy_msg*)msg;
    
    if (m->ernet % 2 == 0) return 1;
    else return 0;
}

int filter_minimum(const void *msg, const void *params) {
    
    int *p;
    dummy_msg *m;
    
    assert(msg != NULL);
    assert(params != NULL);
    
    p = (int *)params;
    m = (dummy_msg*)msg;
    
    if ( m->ernet >= *p ) return 1;
    else return 0;
}
