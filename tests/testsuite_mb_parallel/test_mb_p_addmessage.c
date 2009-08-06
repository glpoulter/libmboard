/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */

#include "header_mb_parallel.h"
#include <stdlib.h>

/* test adding message to board */
void test_mb_p_addmessage(void) {
    
    int rc, rc2;
    
    MBt_Board mb, mb2;
    MBt_Board invalid_mb = (MBt_Board)-1;
    MBt_Board null_mb = (MBt_Board)MB_NULL_MBOARD;
    
    pooled_list *pl, *pl2;
    void *obj, *ptr;
    MBIt_Board *board;
    
    dummy_msg   msg;
    dummy_msg  *msg_ptr;
    dummy_msg2  msg2;
    dummy_msg2 *msg2_ptr;
    
    
    /* Create message boards */
    rc = MB_Create(&mb, sizeof(dummy_msg));
    rc2 = MB_Create(&mb2, sizeof(dummy_msg2));
    if (rc != MB_SUCCESS || rc2 != MB_SUCCESS)
    { /* if mb creation failed, do not proceed */
        CU_FAIL("Failed to create message boards");
        MB_Delete(&mb);
        MB_Delete(&mb2);
        return;
    }
    
    /* Assign values for messages */
    msg.ernet    = 42;
    msg.jeopardy = 3.142;
    
    msg_ptr = (dummy_msg *)malloc(sizeof(dummy_msg));
    CU_ASSERT_PTR_NOT_NULL_FATAL(msg_ptr);
    msg_ptr->ernet    = 24;
    msg_ptr->jeopardy = 2.413;
    
    msg2.eger    = 36;
    msg2.trouble = 19.0179;
    
    /* Adding message to null board */
    rc = MB_AddMessage(null_mb, (void *)&msg);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Adding message to undefined board */
    rc = MB_AddMessage(invalid_mb, (void *)&msg);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    
    /* Adding static messages to board */
    rc = MB_AddMessage(mb, (void *)&msg);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_AddMessage(mb, (void *)msg_ptr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Add message to other board */
    rc = MB_AddMessage(mb2, (void *)&msg2);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Change values of statically declared messages */
    msg.ernet    = 0;
    msg.jeopardy = 1.2;
    msg2.eger    = 3;
    msg2.trouble = 4.5;
    /* Free memory allocated for local message */
    free(msg_ptr);
    
    
    /* --- Dig deep to make sure internal values are in order --- */
    
    /* Get access to internal data */
    CU_ASSERT_PTR_NOT_NULL_FATAL(MBI_OM_mboard);
    CU_ASSERT_EQUAL_FATAL(MBI_OM_mboard->type, OM_TYPE_MBOARD);
    
    obj = MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(obj);
    pl = ((MBIt_Board *)obj)->data;
    CU_ASSERT_PTR_NOT_NULL_FATAL(pl);
    
    obj = MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb2);
    CU_ASSERT_PTR_NOT_NULL_FATAL(obj);
    pl2 = ((MBIt_Board *)obj)->data;
    CU_ASSERT_PTR_NOT_NULL_FATAL(pl2);
    
    /* artificially lock the board */
    board = (MBIt_Board *)obj;
    board->locked = MB_TRUE;
    rc = MB_AddMessage(mb2, (void *)&msg2);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    /* unlock so it can be deleted */
    board->locked = MB_FALSE; 
    
    /* Check that message count is consistent */
    CU_ASSERT_EQUAL(pl->count_current, 2);
    CU_ASSERT_EQUAL(pl2->count_current, 1);
    
    /* Check that message board data is consistent */
    CU_ASSERT_PTR_NOT_NULL_FATAL(pl->head);
    CU_ASSERT_PTR_NOT_NULL_FATAL(pl2->head);
    
    rc = pl_getnode(pl, 0, &ptr);
    msg_ptr = (dummy_msg*)ptr;
    CU_ASSERT_EQUAL(rc, PL_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(msg_ptr);
    CU_ASSERT_EQUAL(msg_ptr->ernet, 42);
    CU_ASSERT_EQUAL(msg_ptr->jeopardy, 3.142);
    
    rc = pl_getnode(pl, 1, &ptr);
    msg_ptr = (dummy_msg*)ptr;
    CU_ASSERT_EQUAL(rc, PL_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(msg_ptr);
    CU_ASSERT_EQUAL(msg_ptr->ernet, 24);
    CU_ASSERT_EQUAL(msg_ptr->jeopardy,2.413);
    
    rc = pl_getnode(pl2, 0, &ptr);
    msg2_ptr = (dummy_msg2*)ptr;
    CU_ASSERT_EQUAL(rc, PL_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(msg2_ptr);
    CU_ASSERT_EQUAL(msg2_ptr->eger, 36);
    CU_ASSERT_EQUAL(msg2_ptr->trouble, 19.0179);
    
    
    /* Delete boards */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_Delete(&mb2);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}

/* test adding many message to board (beyond single pool block) */
void test_mb_p_addmessage_many(void) {
    
    int i, rc, errfound;
    MBt_Board mb;
    dummy_msg msg;
    dummy_msg *msg_ptr;
    void *obj, *ptr;
    pooled_list *pl;
    
    /* create message board */
    rc = MB_Create(&mb, sizeof(dummy_msg));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Add messages */
    errfound = 0;
    for (i = 0; i < (int)(MBI_CONFIG.mempool_blocksize * 2.5); i++)
    {
        /* assign dummy value */
        msg.ernet = i;
        
        /* add message to board */
        rc = MB_AddMessage(mb, &msg);
        if (rc != MB_SUCCESS) errfound++;
    }
    CU_ASSERT_EQUAL(errfound, 0);
    
    /* Check board content */
    errfound = 0;
    obj = MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(obj);
    pl = ((MBIt_Board *)obj)->data;
    
    for (i = 0; i < (int)(MBI_CONFIG.mempool_blocksize * 2.5); i++)
    {       
        /* Get ref to internal message */
        rc = pl_getnode(pl, i, &ptr);
        msg_ptr = (dummy_msg*)ptr;
        if (rc != PL_SUCCESS || !msg_ptr) 
        {
            errfound++;
            break;
        }
        
        if (msg_ptr->ernet != i) errfound++;
    }
    CU_ASSERT_EQUAL(errfound, 0);
    
    /* Delete board */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}
