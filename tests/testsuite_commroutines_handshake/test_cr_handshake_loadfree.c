/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Sept 2009
 * 
 */
#include "header_commroutines_handshake.h"

/* RW on all procs */
void test_cr_handshake_loadandfreebuffers_rw(void) {
    
    int rc;
    struct MBIt_commqueue *node = node_rw_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    if (MBI_CommSize < 2) /* single proc. No comms. */
    {
        CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_END);
        return;
    }
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_COMPLETE_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq2);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq2);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    
    /* repeat calls until state changes */
    while (node->stage == MB_COMM_HANDSHAKE_COMPLETE_PROP)
    {
        rc = MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers(node);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers() returned error code");
            return;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS_2);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
    CU_ASSERT_EQUAL(node->board->syncCompleted, MB_TRUE);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->incount);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, 0);
    CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    
    CU_ASSERT_EQUAL((int)node->board->data->count_current, 
            TEST_MESSAGE_COUNT * MBI_CommSize);
}

/* RW on all procs. No messages */
void test_cr_handshake_loadandfreebuffers_rw_empty(void) {
    
    int rc;
    struct MBIt_commqueue *node = node_rw_empty_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    if (MBI_CommSize < 2) /* single proc. No comms. */
    {
        CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_END);
        return;
    }
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_COMPLETE_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq2);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq2);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    
    /* repeat calls until state changes */
    while (node->stage == MB_COMM_HANDSHAKE_COMPLETE_PROP)
    {
        rc = MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers(node);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers() returned error code");
            return;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS_2);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
    CU_ASSERT_EQUAL(node->board->syncCompleted, MB_TRUE);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->incount);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, 0);
    CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    
    CU_ASSERT_EQUAL((int)node->board->data->count_current, 0);
}

/* RW on all procs. Has filter that uses indexmap */
void test_cr_handshake_loadandfreebuffers_rw_fmap(void) {
    
    int rc;
    struct MBIt_commqueue *node = node_rw_fmap_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    if (MBI_CommSize < 2) /* single proc. No comms. */
    {
        CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_END);
        return;
    }
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_COMPLETE_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq2);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq2);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    
    /* repeat calls until state changes */
    while (node->stage == MB_COMM_HANDSHAKE_COMPLETE_PROP)
    {
        rc = MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers(node);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers() returned error code");
            return;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS_2);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
    CU_ASSERT_EQUAL(node->board->syncCompleted, MB_TRUE);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->incount);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, 0);
    CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    
    CU_ASSERT_EQUAL((int)node->board->data->count_current, 
            TEST_MESSAGE_COUNT + 2*SMALLNUM);
}

/* RW on all procs. Has filter that will cause fallback to full data
 * replication when procs > 3 */
void test_cr_handshake_loadandfreebuffers_rw_fdr(void) {
    
    int rc;
    struct MBIt_commqueue *node = node_rw_fdr_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    if (MBI_CommSize < 2) /* single proc. No comms. */
    {
        CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_END);
        return;
    }
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_COMPLETE_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq2);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq2);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    
    /* repeat calls until state changes */
    while (node->stage == MB_COMM_HANDSHAKE_COMPLETE_PROP)
    {
        rc = MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers(node);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers() returned error code");
            return;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS_2);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
    CU_ASSERT_EQUAL(node->board->syncCompleted, MB_TRUE);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->incount);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, 0);
    CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    
    CU_ASSERT_EQUAL((int)node->board->data->count_current, 
            TEST_MESSAGE_COUNT + (TEST_MESSAGE_COUNT/2 * (MBI_CommSize-1)));
}

/* RW on even procs. IDLE on odd */
void test_cr_handshake_loadandfreebuffers_rw_id(void) {
    
    int rc;
    struct MBIt_commqueue *node = node_rw_id_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    /* odd procs are IDLE and should not reach this stage */
    /* same for P < 3 */
    if (MBI_CommRank % 2 != 0 || MBI_CommSize < 3) 
    {
        CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_END);
        return;
    }
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_COMPLETE_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq2);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq2);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    
    /* repeat calls until state changes */
    while (node->stage == MB_COMM_HANDSHAKE_COMPLETE_PROP)
    {
        rc = MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers(node);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers() returned error code");
            return;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS_2);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
    CU_ASSERT_EQUAL(node->board->syncCompleted, MB_TRUE);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->incount);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, 0);
    CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    
    if (MBI_CommSize % 2 == 0)
    {
        CU_ASSERT_EQUAL((int)node->board->data->count_current, 
                        TEST_MESSAGE_COUNT * (MBI_CommSize/2));
    }
    else
    {
        CU_ASSERT_EQUAL((int)node->board->data->count_current, 
                        TEST_MESSAGE_COUNT * ((MBI_CommSize/2) + 1));
    }
}

/* RO on even procs. WO on odd */
void test_cr_handshake_loadandfreebuffers_ro_wo(void) {
    
    int rc;
    struct MBIt_commqueue *node = node_ro_wo_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    if (MBI_CommSize < 2) /* single proc. No comms. */
    {
        CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_END);
        return;
    }
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_COMPLETE_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq2);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq2);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    
    /* repeat calls until state changes */
    while (node->stage == MB_COMM_HANDSHAKE_COMPLETE_PROP)
    {
        rc = MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers(node);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers() returned error code");
            return;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS_2);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
    CU_ASSERT_EQUAL(node->board->syncCompleted, MB_TRUE);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->incount);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, 0);
    CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    
    if (MBI_CommRank % 2 != 0) /* WO */
    {
        CU_ASSERT_EQUAL((int)node->board->data->count_current, TEST_MESSAGE_COUNT);
    }
    else /* RO */
    {
        CU_ASSERT_EQUAL((int)node->board->data->count_current, 
                TEST_MESSAGE_COUNT * (MBI_CommSize/2));
    }
}
