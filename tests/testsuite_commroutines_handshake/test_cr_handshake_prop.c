/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Sept 2009
 * 
 */
#include "header_commroutines_handshake.h"

/* RW on all procs */
void test_cr_handshake_propagatemessages_rw(void) {
    
    int i, rc;
    struct MBIt_commqueue *node = node_rw_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    if (MBI_CommSize < 2) /* single proc. No comms. */
    {
        CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_END);
        return;
    }
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_START_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);

    
    /* repeat calls until state changes */
    while (node->stage == MB_COMM_HANDSHAKE_START_PROP)
    {
        rc = MBI_CommRoutine_HANDSHAKE_PropagateMessages(node);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("MBI_CommRoutine_HANDSHAKE_PropagateMessages() returned error code");
            return;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_HANDSHAKE_COMPLETE_PROP);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq2);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq2);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, MBI_CommSize - 1);
    CU_ASSERT_EQUAL((int)node->pending_out2, MBI_CommSize - 1);
    
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank)
        {
            CU_ASSERT_EQUAL(node->sendreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->recvreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->incount[i], -1);
            CU_ASSERT_PTR_NULL(node->inbuf[i]);
        }
        else
        {
            CU_ASSERT_NOT_EQUAL(node->sendreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_NOT_EQUAL(node->recvreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->incount[i], TEST_MESSAGE_COUNT);
            CU_ASSERT_PTR_NOT_NULL(node->inbuf[i]);
        }
    }
    
    CU_ASSERT_PTR_NOT_NULL(node->outbuf[0]);
    for (i = 1; i < MBI_CommSize; i++)
    {
        CU_ASSERT_PTR_NULL(node->outbuf[i]);
    }
}

/* RW on all procs. No messages */
void test_cr_handshake_propagatemessages_rw_empty(void) {
    
    int i, rc;
    struct MBIt_commqueue *node = node_rw_empty_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    if (MBI_CommSize < 2) /* single proc. No comms. */
    {
        CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_END);
        return;
    }
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_START_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);

    
    /* repeat calls until state changes */
    while (node->stage == MB_COMM_HANDSHAKE_START_PROP)
    {
        rc = MBI_CommRoutine_HANDSHAKE_PropagateMessages(node);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("MBI_CommRoutine_HANDSHAKE_PropagateMessages() returned error code");
            return;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_HANDSHAKE_COMPLETE_PROP);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq2);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq2);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, 0);
    CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    
    
    for (i = 0; i < MBI_CommSize; i++)
    {
        CU_ASSERT_EQUAL(node->sendreq2[i], MPI_REQUEST_NULL);
        CU_ASSERT_EQUAL(node->recvreq2[i], MPI_REQUEST_NULL);
        CU_ASSERT_PTR_NULL(node->inbuf[i]);
        CU_ASSERT_PTR_NULL(node->outbuf[i]);
        if (i == MBI_CommRank)
        {
            CU_ASSERT_EQUAL(node->incount[i], -1);
        }
        else
        {
            CU_ASSERT_EQUAL(node->incount[i], 0);
        }
    }
}

/* RW on all procs. Has filter that uses indexmap */
void test_cr_handshake_propagatemessages_rw_fmap(void) {
    
    int i, neigh1, neigh2, rc;
    struct MBIt_commqueue *node = node_rw_fmap_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    if (MBI_CommSize < 2) /* single proc. No comms. */
    {
        CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_END);
        return;
    }
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_START_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);

    
    /* repeat calls until state changes */
    while (node->stage == MB_COMM_HANDSHAKE_START_PROP)
    {
        rc = MBI_CommRoutine_HANDSHAKE_PropagateMessages(node);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("MBI_CommRoutine_HANDSHAKE_PropagateMessages() returned error code");
            return;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_HANDSHAKE_COMPLETE_PROP);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq2);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq2);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    if (MBI_CommSize > 2)
    {
        CU_ASSERT_EQUAL((int)node->pending_in2, 2);
        CU_ASSERT_EQUAL((int)node->pending_out2, 2);
    }
    else if (MBI_CommSize == 2)
    {
        CU_ASSERT_EQUAL((int)node->pending_in2, 1);
        CU_ASSERT_EQUAL((int)node->pending_out2, 1);
    }
    else
    {
        CU_ASSERT_EQUAL((int)node->pending_in2, 0);
        CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    }
    
    neigh1 = (MBI_CommRank + 1) % MBI_CommSize;
    neigh2 = (MBI_CommRank == 0) ? MBI_CommSize -1 : MBI_CommRank - 1;
    
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank)
        {
            CU_ASSERT_EQUAL(node->sendreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->recvreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->incount[i], -1);
            CU_ASSERT_PTR_NULL(node->inbuf[i]);
            CU_ASSERT_PTR_NULL(node->outbuf[i]);
        }
        else if (i == neigh1 || i == neigh2)
        {
            CU_ASSERT_NOT_EQUAL(node->sendreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_NOT_EQUAL(node->recvreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_PTR_NOT_NULL(node->inbuf[i]);
            CU_ASSERT_PTR_NOT_NULL(node->outbuf[i]);
            
            if (neigh1 == neigh2)
            {
                CU_ASSERT_EQUAL(node->incount[i], 2*SMALLNUM);
            }
            else
            {
                CU_ASSERT_EQUAL(node->incount[i], SMALLNUM);
            }
        }
        else
        {
            CU_ASSERT_EQUAL(node->sendreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->recvreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->incount[i], 0);
            CU_ASSERT_PTR_NULL(node->inbuf[i]);
            CU_ASSERT_PTR_NULL(node->outbuf[i]);
        }
    }
}

/* RW on all procs. Has filter that will cause fallback to full data
 * replication when procs > 3 */
void test_cr_handshake_propagatemessages_rw_fdr(void) {
    
    int i, rc;
    struct MBIt_commqueue *node = node_rw_fdr_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    if (MBI_CommSize < 2) /* single proc. No comms. */
    {
        CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_END);
        return;
    }
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_START_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);

    
    /* repeat calls until state changes */
    while (node->stage == MB_COMM_HANDSHAKE_START_PROP)
    {
        rc = MBI_CommRoutine_HANDSHAKE_PropagateMessages(node);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("MBI_CommRoutine_HANDSHAKE_PropagateMessages() returned error code");
            return;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_HANDSHAKE_COMPLETE_PROP);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq2);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq2);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, MBI_CommSize - 1);
    CU_ASSERT_EQUAL((int)node->pending_out2, MBI_CommSize - 1);
    
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank)
        {
            CU_ASSERT_EQUAL(node->sendreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->recvreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->incount[i], -1);
            CU_ASSERT_PTR_NULL(node->inbuf[i]);
        }
        else
        {
            CU_ASSERT_NOT_EQUAL(node->sendreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_NOT_EQUAL(node->recvreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_PTR_NOT_NULL(node->inbuf[i]);
        }
    }
    
    if (MBI_CommSize > 3)
    {
        CU_ASSERT_PTR_NOT_NULL(node->outbuf[0]);
        for (i = 0; i < MBI_CommSize; i++)
        {
            if (i != 0)
            {
                CU_ASSERT_PTR_NULL(node->outbuf[i]);
            }
            
            if (i != MBI_CommRank)
            {
                CU_ASSERT_EQUAL(node->incount[i], TEST_MESSAGE_COUNT);
            }
        }
        
        
    }
    else
    {
        for (i = 0; i < MBI_CommSize; i++)
        {
            if (i == MBI_CommRank)
            {
                CU_ASSERT_PTR_NULL(node->outbuf[i]);
            }
            else
            {
                CU_ASSERT_PTR_NOT_NULL(node->outbuf[i]);
                CU_ASSERT_EQUAL(node->incount[i], TEST_MESSAGE_COUNT / 2);
            }
        }
    }
}

/* RW on even procs. IDLE on odd */
void test_cr_handshake_propagatemessages_rw_id(void) {
    
    int i, rc, peers;
    struct MBIt_commqueue *node = node_rw_id_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    /* IDLE procs or when no comms (P < 3) */
    if (MBI_CommRank % 2 != 0 || MBI_CommSize < 3)
    {
        CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
        return;
    }
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_START_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);

    
    /* repeat calls until state changes */
    while (node->stage == MB_COMM_HANDSHAKE_START_PROP)
    {
        rc = MBI_CommRoutine_HANDSHAKE_PropagateMessages(node);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("MBI_CommRoutine_HANDSHAKE_PropagateMessages() returned error code");
            return;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_HANDSHAKE_COMPLETE_PROP);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq2);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq2);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    
    if (MBI_CommSize % 2 == 0) peers = (MBI_CommSize / 2) - 1;
    else peers = (MBI_CommSize / 2);
    
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, peers);
    CU_ASSERT_EQUAL((int)node->pending_out2, peers);
    
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank || i % 2 != 0)
        {
            CU_ASSERT_EQUAL(node->sendreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->recvreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->incount[i], -1);
            CU_ASSERT_PTR_NULL(node->inbuf[i]);
        }
        else
        {
            CU_ASSERT_NOT_EQUAL(node->sendreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_NOT_EQUAL(node->recvreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->incount[i], TEST_MESSAGE_COUNT);
            CU_ASSERT_PTR_NOT_NULL(node->inbuf[i]);
        }
    }
    
    CU_ASSERT_PTR_NOT_NULL(node->outbuf[0]);
    for (i = 1; i < MBI_CommSize; i++)
    {
        CU_ASSERT_PTR_NULL(node->outbuf[i]);
    }
}

/* RO on even procs. WO on odd */
void test_cr_handshake_propagatemessages_ro_wo(void) {
    
    int i, rc;
    struct MBIt_commqueue *node = node_ro_wo_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    if (MBI_CommSize < 2) /* single proc. No comms. */
    {
        CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_END);
        return;
    }
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_START_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->outcount);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);

    
    /* repeat calls until state changes */
    while (node->stage == MB_COMM_HANDSHAKE_START_PROP)
    {
        rc = MBI_CommRoutine_HANDSHAKE_PropagateMessages(node);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("MBI_CommRoutine_HANDSHAKE_PropagateMessages() returned error code");
            return;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_HANDSHAKE_COMPLETE_PROP);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->outcount);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq2);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq2);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    
    if (MBI_CommRank % 2 == 0) /* even. RO */
    {
        CU_ASSERT_EQUAL((int)node->pending_in2, MBI_CommSize / 2);
        CU_ASSERT_EQUAL((int)node->pending_out2, 0);
        
        for (i = 0; i < MBI_CommSize; i++)
        {
            CU_ASSERT_PTR_NULL(node->outbuf[i]);
            CU_ASSERT_EQUAL(node->sendreq2[i], MPI_REQUEST_NULL);
            
            if (i == MBI_CommRank || i % 2 == 0)
            { 
                CU_ASSERT_EQUAL(node->recvreq2[i], MPI_REQUEST_NULL);
                CU_ASSERT_EQUAL(node->incount[i], -1);
                CU_ASSERT_PTR_NULL(node->inbuf[i]);
            }
            else
            {
                CU_ASSERT_NOT_EQUAL(node->recvreq2[i], MPI_REQUEST_NULL);
                CU_ASSERT_EQUAL(node->incount[i], TEST_MESSAGE_COUNT);
                CU_ASSERT_PTR_NOT_NULL(node->inbuf[i]);
            }

        }
    }
    else /* odd. WO */
    {
        CU_ASSERT_EQUAL((int)node->pending_in2, 0 );
        if (MBI_CommSize % 2 == 0)
        {
            CU_ASSERT_EQUAL((int)node->pending_out2, MBI_CommSize / 2);
        }
        else
        {
            CU_ASSERT_EQUAL((int)node->pending_out2, (MBI_CommSize / 2) + 1);
        }
        
        for (i = 0; i < MBI_CommSize; i++)
        {
            CU_ASSERT_EQUAL(node->recvreq2[i], MPI_REQUEST_NULL);
            CU_ASSERT_PTR_NULL(node->inbuf[i]);
            CU_ASSERT_EQUAL(node->incount[i], -1);
            if (i == MBI_CommRank || i % 2 != 0)
            {
                CU_ASSERT_EQUAL(node->sendreq2[i], MPI_REQUEST_NULL);
            }
            else
            {
                CU_ASSERT_NOT_EQUAL(node->sendreq2[i], MPI_REQUEST_NULL);
            }
        }
        
        CU_ASSERT_PTR_NOT_NULL(node->outbuf[0]);
        for (i = 1; i < MBI_CommSize; i++)
        {
            CU_ASSERT_PTR_NULL(node->outbuf[i]);
        }
    }
}
