/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */
#include "header_commqueue.h"

/* Testing Initial state */
void test_cq_init(void) {
    
    struct MBIt_commqueue *node;
    
    /* check using isEmpty routine */
    CU_ASSERT_FATAL(MBI_CommQueue_isEmpty());
    
    /* check using GetFirstNode routine */
    node = MBI_CommQueue_GetFirstNode();
    CU_ASSERT_PTR_NULL_FATAL(node);
}

/* Testing addition of new node to CommQueue */
void test_cq_newnode(void) {
    
    int rc;
    struct MBIt_commqueue *node;
    
    /* add a node */
    rc = MBI_CommQueue_Push((MBt_Board)100, PRE_TAGGING);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT(!MBI_CommQueue_isEmpty());

    
    /* and another */
    rc = MBI_CommQueue_Push((MBt_Board)101, PRE_TAGGING);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT(!MBI_CommQueue_isEmpty());
    
    /* get reference to first node */
    node = MBI_CommQueue_GetFirstNode();
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    CU_ASSERT_EQUAL(node->mb, (MBt_Board)100);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_EQUAL(node->stage, PRE_TAGGING)
    
    /* get ref to second node */
    node = node->hh.next;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    CU_ASSERT_EQUAL(node->mb, (MBt_Board)101);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_EQUAL(node->stage, PRE_TAGGING)
    
    /* make sure this is last node */
    CU_ASSERT_PTR_NULL(node->hh.next);
}

/* Testing deletion of nodes from CommQueue */
void test_cq_deletenode(void) {
    
    int rc;
    struct MBIt_commqueue *node;
    
    /* Delete invalid node */
    rc = MBI_CommQueue_Pop((MBt_Board)999);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Delete first node */
    rc = MBI_CommQueue_Pop((MBt_Board)100);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT(!MBI_CommQueue_isEmpty());
    
    /* make sure remaining node is the right one */
    node = MBI_CommQueue_GetFirstNode();
    CU_ASSERT_EQUAL(node->mb, (MBt_Board)101);
    
    /* Delete second node */
    rc = MBI_CommQueue_Pop((MBt_Board)101);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* CommQueue should now be empty */
    CU_ASSERT(MBI_CommQueue_isEmpty());
}