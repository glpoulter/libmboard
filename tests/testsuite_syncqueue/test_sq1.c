/* $Id: test_sq1.c 816 2008-04-22 13:49:51Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */
#include "header_syncqueue.h"

/* test Sync Request Queue initialisation */
void test_sq_init(void) {
    
    int rc;
    
    /* check SRQ initalised and is empty */
    CU_ASSERT(MBI_SyncQueue_isEmpty());
    
    /* --- make sure locks and condition variables initialised --- */
    
    /* try locking SRQ */
    rc = pthread_mutex_lock(&MBI_SRQLock);
    CU_ASSERT_EQUAL(rc, 0);
    
    /* ... and then unlock */
    rc = pthread_mutex_unlock(&MBI_SRQLock);
    CU_ASSERT_EQUAL(rc, 0);
    
    /* try sending signal to SRQCond */
    rc = pthread_cond_signal(&MBI_SRQCond);
    CU_ASSERT_EQUAL(rc, 0);
    
}

/* test adding sync requests */
void test_sq_push(void) {
    
    int rc;
    
    /* push requests into queue */
    rc = MBI_SyncQueue_Push((MBt_Board)111);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* SRQ should no longer be empty */
    CU_ASSERT(!MBI_SyncQueue_isEmpty());
    
    /* add a few more requests */
    rc = MBI_SyncQueue_Push((MBt_Board)222);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT(!MBI_SyncQueue_isEmpty());
    
    rc = MBI_SyncQueue_Push((MBt_Board)333);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT(!MBI_SyncQueue_isEmpty());
    
}

/* test popping sync requests */
void test_sq_pop(void) {
    
    int rc;
    MBt_Board mb;
    
    /* pop request from queue */
    rc = MBI_SyncQueue_Pop(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, (MBt_Board)111);
    
    /* should still not be empty */
    CU_ASSERT(!MBI_SyncQueue_isEmpty());
    
    /* pop a remaining requests */
    rc = MBI_SyncQueue_Pop(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, (MBt_Board)222);
    /* should still not be empty */
    CU_ASSERT(!MBI_SyncQueue_isEmpty());
    
    rc = MBI_SyncQueue_Pop(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, (MBt_Board)333);
    /* should now be empty */
    CU_ASSERT(MBI_SyncQueue_isEmpty());
    
    /* try popping from empty queue */
    rc = MBI_SyncQueue_Pop(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
    /* should still be empty */
    CU_ASSERT(MBI_SyncQueue_isEmpty());
    
}
