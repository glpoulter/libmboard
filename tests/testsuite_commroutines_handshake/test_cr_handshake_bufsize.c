/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Sept 2009
 * 
 */
#include "header_commroutines_handshake.h"

/* RW on all procs */
void test_cr_handshake_agreebufsizes_rw(void) {
    
    int i, rc;
    struct MBIt_commqueue *node = node_rw_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_PRE_PROP);
    CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, 0);
    CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_PTR_NULL(node->board);
    
    /* run func */
    rc = MBI_CommRoutine_HANDSHAKE_AgreeBufSizes(node);
    if (MBI_CommSize == 1) /* single proc, no comms */
    {
        CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS_2);
        CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
        CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
        CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
        CU_ASSERT_EQUAL((int)node->pending_in, 0);
        CU_ASSERT_EQUAL((int)node->pending_out, 0);
        CU_ASSERT_EQUAL((int)node->pending_in2, 0);
        CU_ASSERT_EQUAL((int)node->pending_out2, 0);
        CU_ASSERT_PTR_NULL(node->inbuf);
        CU_ASSERT_PTR_NULL(node->outbuf);
        CU_ASSERT_PTR_NULL(node->sendreq);
        CU_ASSERT_PTR_NULL(node->recvreq);
        CU_ASSERT_PTR_NULL(node->sendreq2);
        CU_ASSERT_PTR_NULL(node->recvreq2);
        CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
        CU_ASSERT_EQUAL(node->board->data->count_current, TEST_MESSAGE_COUNT);
        return;
    }
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_HANDSHAKE_START_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->incount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->outcount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->recvreq);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    
    CU_ASSERT_EQUAL(node->board->reader_count, MBI_CommSize - 1);
    CU_ASSERT_EQUAL(node->board->writer_count, MBI_CommSize - 1);
    
    CU_ASSERT_PTR_NULL(node->board->tt);
    CU_ASSERT_EQUAL(node->board->locked, MB_TRUE);
    CU_ASSERT_EQUAL(node->board->syncCompleted, MB_FALSE);
    
    CU_ASSERT_EQUAL((int)node->pending_in, node->board->writer_count);
    CU_ASSERT_EQUAL((int)node->pending_out, node->board->reader_count);
    CU_ASSERT_EQUAL((int)node->pending_in, MBI_CommSize - 1);
    CU_ASSERT_EQUAL((int)node->pending_out, MBI_CommSize - 1);
    
    CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_TRUE);
    
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank)
        {
            CU_ASSERT_EQUAL(node->incount[i], -1);
            CU_ASSERT_EQUAL(node->outcount[i], -1);
            CU_ASSERT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
        }
        else
        {
            CU_ASSERT_EQUAL(node->outcount[i], TEST_MESSAGE_COUNT);
            CU_ASSERT_NOT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
            CU_ASSERT_NOT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
        }
    }
}

/* RW on all procs. No messages */
void test_cr_handshake_agreebufsizes_rw_empty(void) {
    
    int i, rc;
    struct MBIt_commqueue *node = node_rw_empty_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_PRE_PROP);
    CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, 0);
    CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_PTR_NULL(node->board);
    
    /* run func */
    rc = MBI_CommRoutine_HANDSHAKE_AgreeBufSizes(node);
    if (MBI_CommSize == 1) /* single proc, no comms */
    {
        CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS_2);
        CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
        CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
        CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
        CU_ASSERT_EQUAL((int)node->pending_in, 0);
        CU_ASSERT_EQUAL((int)node->pending_out, 0);
        CU_ASSERT_EQUAL((int)node->pending_in2, 0);
        CU_ASSERT_EQUAL((int)node->pending_out2, 0);
        CU_ASSERT_PTR_NULL(node->inbuf);
        CU_ASSERT_PTR_NULL(node->outbuf);
        CU_ASSERT_PTR_NULL(node->sendreq);
        CU_ASSERT_PTR_NULL(node->recvreq);
        CU_ASSERT_PTR_NULL(node->sendreq2);
        CU_ASSERT_PTR_NULL(node->recvreq2);
        CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
        CU_ASSERT_EQUAL(node->board->data->count_current, 0);
        return;
    }
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_HANDSHAKE_START_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->incount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->outcount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->recvreq);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    
    CU_ASSERT_PTR_NULL(node->board->tt);
    CU_ASSERT_EQUAL(node->board->locked, MB_TRUE);
    CU_ASSERT_EQUAL(node->board->syncCompleted, MB_FALSE);
    
    CU_ASSERT_EQUAL(node->board->reader_count, MBI_CommSize - 1);
    CU_ASSERT_EQUAL(node->board->writer_count, MBI_CommSize - 1);
    
    CU_ASSERT_EQUAL((int)node->pending_in, node->board->writer_count);
    CU_ASSERT_EQUAL((int)node->pending_out, node->board->reader_count);
    CU_ASSERT_EQUAL((int)node->pending_in, MBI_CommSize - 1);
    CU_ASSERT_EQUAL((int)node->pending_out, MBI_CommSize - 1);
    
    CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_TRUE);
    
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank)
        {
            CU_ASSERT_EQUAL(node->incount[i], -1);
            CU_ASSERT_EQUAL(node->outcount[i], -1);
            CU_ASSERT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
        }
        else
        {
            CU_ASSERT_EQUAL(node->outcount[i], 0);
            CU_ASSERT_NOT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
            CU_ASSERT_NOT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
        }
    }
}

/* RW on all procs. Has filter that uses indexmap */
void test_cr_handshake_agreebufsizes_rw_fmap(void) {
    
    int i, rc;
    int neigh1, neigh2;
    struct MBIt_commqueue *node = node_rw_fmap_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_PRE_PROP);
    CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, 0);
    CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_PTR_NULL(node->board);
    
    /* run func */
    rc = MBI_CommRoutine_HANDSHAKE_AgreeBufSizes(node);
    if (MBI_CommSize == 1) /* single proc, no comms */
    {
        CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS_2);
        CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
        CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
        CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
        CU_ASSERT_EQUAL((int)node->pending_in, 0);
        CU_ASSERT_EQUAL((int)node->pending_out, 0);
        CU_ASSERT_EQUAL((int)node->pending_in2, 0);
        CU_ASSERT_EQUAL((int)node->pending_out2, 0);
        CU_ASSERT_PTR_NULL(node->inbuf);
        CU_ASSERT_PTR_NULL(node->outbuf);
        CU_ASSERT_PTR_NULL(node->sendreq);
        CU_ASSERT_PTR_NULL(node->recvreq);
        CU_ASSERT_PTR_NULL(node->sendreq2);
        CU_ASSERT_PTR_NULL(node->recvreq2);
        CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
        CU_ASSERT_EQUAL(node->board->data->count_current, TEST_MESSAGE_COUNT);
        return;
    }
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_HANDSHAKE_START_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->incount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->outcount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->recvreq);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    
    CU_ASSERT_PTR_NOT_NULL(node->board->tt);
    CU_ASSERT_EQUAL(node->board->locked, MB_TRUE);
    CU_ASSERT_EQUAL(node->board->syncCompleted, MB_FALSE);
    
    CU_ASSERT_EQUAL(node->board->reader_count, MBI_CommSize - 1);
    CU_ASSERT_EQUAL(node->board->writer_count, MBI_CommSize - 1);
    
    CU_ASSERT_EQUAL((int)node->pending_in, node->board->writer_count);
    CU_ASSERT_EQUAL((int)node->pending_out, node->board->reader_count);
    CU_ASSERT_EQUAL((int)node->pending_in, MBI_CommSize - 1);
    CU_ASSERT_EQUAL((int)node->pending_out, MBI_CommSize - 1);
    
    CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
    
    neigh1 = (MBI_CommRank + 1) % MBI_CommSize;
    neigh2 = (MBI_CommRank == 0) ? MBI_CommSize - 1: MBI_CommRank - 1;
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank)
        {
            CU_ASSERT_EQUAL(node->incount[i], -1);
            CU_ASSERT_EQUAL(node->outcount[i], -1);
            CU_ASSERT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
        }
        else
        {
            if (i == neigh1 || i == neigh2)
            {
                if (neigh1 == neigh2) /* for 2 procs */
                {
                    CU_ASSERT_EQUAL(node->outcount[i], SMALLNUM * 2);
                }
                else
                {
                    CU_ASSERT_EQUAL(node->outcount[i], SMALLNUM);
                }
            }
            else
            {
                CU_ASSERT_EQUAL(node->outcount[i], 0);
            }
            CU_ASSERT_NOT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
            CU_ASSERT_NOT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
        }
    }
}

/* RW on all procs. Has filter that will cause fallback to full data
 * replication when procs > 3 */
void test_cr_handshake_agreebufsizes_rw_fdr(void) {
    
    int i, rc;
    struct MBIt_commqueue *node = node_rw_fdr_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_PRE_PROP);
    CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, 0);
    CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_PTR_NULL(node->board);
    
    /* run func */
    rc = MBI_CommRoutine_HANDSHAKE_AgreeBufSizes(node);
    if (MBI_CommSize == 1) /* single proc, no comms */
    {
        CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS_2);
        CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
        CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
        CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
        CU_ASSERT_EQUAL((int)node->pending_in, 0);
        CU_ASSERT_EQUAL((int)node->pending_out, 0);
        CU_ASSERT_EQUAL((int)node->pending_in2, 0);
        CU_ASSERT_EQUAL((int)node->pending_out2, 0);
        CU_ASSERT_PTR_NULL(node->inbuf);
        CU_ASSERT_PTR_NULL(node->outbuf);
        CU_ASSERT_PTR_NULL(node->sendreq);
        CU_ASSERT_PTR_NULL(node->recvreq);
        CU_ASSERT_PTR_NULL(node->sendreq2);
        CU_ASSERT_PTR_NULL(node->recvreq2);
        CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
        CU_ASSERT_EQUAL(node->board->data->count_current, TEST_MESSAGE_COUNT);
        return;
    }
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_HANDSHAKE_START_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->incount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->outcount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->recvreq);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    
    CU_ASSERT_EQUAL(node->board->locked, MB_TRUE);
    CU_ASSERT_EQUAL(node->board->syncCompleted, MB_FALSE);
    
    CU_ASSERT_EQUAL(node->board->reader_count, MBI_CommSize - 1);
    CU_ASSERT_EQUAL(node->board->writer_count, MBI_CommSize - 1);
    
    CU_ASSERT_EQUAL((int)node->pending_in, node->board->writer_count);
    CU_ASSERT_EQUAL((int)node->pending_out, node->board->reader_count);
    CU_ASSERT_EQUAL((int)node->pending_in, MBI_CommSize - 1);
    CU_ASSERT_EQUAL((int)node->pending_out, MBI_CommSize - 1);
    
    if (MBI_CommSize > 3)
    {
        CU_ASSERT_PTR_NULL(node->board->tt);
        CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_TRUE);
        CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_TRUE);

        for (i = 0; i < MBI_CommSize; i++)
        {
            if (i == MBI_CommRank)
            {
                CU_ASSERT_EQUAL(node->incount[i], -1);
                CU_ASSERT_EQUAL(node->outcount[i], -1);
                CU_ASSERT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
                CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
            }
            else
            {
                CU_ASSERT_EQUAL(node->outcount[i], TEST_MESSAGE_COUNT);
                CU_ASSERT_NOT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
                CU_ASSERT_NOT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
            }
        }
    }
    else
    {
        CU_ASSERT_PTR_NOT_NULL(node->board->tt);
        CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
        CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);

        for (i = 0; i < MBI_CommSize; i++)
        {
            if (i == MBI_CommRank)
            {
                CU_ASSERT_EQUAL(node->incount[i], -1);
                CU_ASSERT_EQUAL(node->outcount[i], -1);
                CU_ASSERT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
                CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
            }
            else
            {
                CU_ASSERT_EQUAL(node->outcount[i], TEST_MESSAGE_COUNT / 2);
                CU_ASSERT_NOT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
                CU_ASSERT_NOT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
            }
        }
    }
}

/* RW on even procs. IDLE on odd */
void test_cr_handshake_agreebufsizes_rw_id(void) {
    
    int i, rc, peers;
    struct MBIt_commqueue *node = node_rw_id_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_PRE_PROP);
    CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, 0);
    CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_PTR_NULL(node->board);
    
    /* run func */
    rc = MBI_CommRoutine_HANDSHAKE_AgreeBufSizes(node);
    if (MBI_CommSize < 3) /* 1 or 2 proc, no comms */
    {
        CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS_2);
        CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
        CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
        CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
        CU_ASSERT_EQUAL((int)node->pending_in, 0);
        CU_ASSERT_EQUAL((int)node->pending_out, 0);
        CU_ASSERT_EQUAL((int)node->pending_in2, 0);
        CU_ASSERT_EQUAL((int)node->pending_out2, 0);
        CU_ASSERT_PTR_NULL(node->inbuf);
        CU_ASSERT_PTR_NULL(node->outbuf);
        CU_ASSERT_PTR_NULL(node->sendreq);
        CU_ASSERT_PTR_NULL(node->recvreq);
        CU_ASSERT_PTR_NULL(node->sendreq2);
        CU_ASSERT_PTR_NULL(node->recvreq2);
        CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
        if (MBI_CommRank == 0)
        {
            CU_ASSERT_EQUAL(node->board->data->count_current, TEST_MESSAGE_COUNT);
        }
        else /* MBI_CommRank == 1. IDLE proc */
        {
            CU_ASSERT_EQUAL(node->board->data->count_current, 0);
        }
        return;
    }

    if (MBI_CommRank % 2 != 0) /* odd procs are IDLE */
    {
        CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
        CU_ASSERT_EQUAL(node->board->locked, MB_TRUE);
        CU_ASSERT_EQUAL(node->board->syncCompleted, MB_TRUE);
        CU_ASSERT_EQUAL(node->board->reader_count, 0);
        CU_ASSERT_EQUAL(node->board->writer_count, 0);
        CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
        CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
        CU_ASSERT_EQUAL((int)node->pending_in, 0);
        CU_ASSERT_EQUAL((int)node->pending_out, 0);
        CU_ASSERT_EQUAL((int)node->pending_in2, 0);
        CU_ASSERT_EQUAL((int)node->pending_out2, 0);
        CU_ASSERT_PTR_NULL(node->incount);
        CU_ASSERT_PTR_NULL(node->outcount);
        CU_ASSERT_PTR_NULL(node->recvreq);
        CU_ASSERT_PTR_NULL(node->sendreq);
        CU_ASSERT_PTR_NULL(node->recvreq2);
        CU_ASSERT_PTR_NULL(node->sendreq2);
        CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
        CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS_2);
        return;
    }

    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    CU_ASSERT_EQUAL(node->stage, MB_COMM_HANDSHAKE_START_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->incount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->outcount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->recvreq);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    
    CU_ASSERT_PTR_NULL(node->board->tt);
    CU_ASSERT_EQUAL(node->board->locked, MB_TRUE);
    CU_ASSERT_EQUAL(node->board->syncCompleted, MB_FALSE);
    
    if (MBI_CommSize % 2 == 0) peers = (MBI_CommSize / 2) - 1;
    else peers = (MBI_CommSize / 2);
    
    CU_ASSERT_EQUAL(node->board->reader_count, peers);
    CU_ASSERT_EQUAL(node->board->writer_count, peers);
    
    CU_ASSERT_EQUAL((int)node->pending_in, node->board->writer_count);
    CU_ASSERT_EQUAL((int)node->pending_out, node->board->reader_count);
    
    CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_TRUE);
    
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank || i % 2 != 0)
        {
            CU_ASSERT_EQUAL(node->incount[i], -1);
            CU_ASSERT_EQUAL(node->outcount[i], -1);
            CU_ASSERT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
        }
        else
        {
            CU_ASSERT_EQUAL(node->outcount[i], TEST_MESSAGE_COUNT);
            CU_ASSERT_NOT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
            CU_ASSERT_NOT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
        }
    }
}

/* RO on even procs. WO on odd */
void test_cr_handshake_agreebufsizes_ro_wo(void) {
    
    int i, rc;
    struct MBIt_commqueue *node = node_ro_wo_hs;
    CU_ASSERT_PTR_NOT_NULL_FATAL(node);
    
    /* pre */
    CU_ASSERT_EQUAL_FATAL(node->stage, MB_COMM_HANDSHAKE_PRE_PROP);
    CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
    CU_ASSERT_EQUAL((int)node->pending_in, 0);
    CU_ASSERT_EQUAL((int)node->pending_out, 0);
    CU_ASSERT_EQUAL((int)node->pending_in2, 0);
    CU_ASSERT_EQUAL((int)node->pending_out2, 0);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_PTR_NULL(node->outbuf);
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_PTR_NULL(node->board);
    
    /* run func */
    rc = MBI_CommRoutine_HANDSHAKE_AgreeBufSizes(node);
    if (MBI_CommSize == 1) /* single proc, no comms */
    {
        CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS_2);
        CU_ASSERT_EQUAL(node->stage, MB_COMM_END);
        CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
        CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_FALSE);
        CU_ASSERT_EQUAL((int)node->pending_in, 0);
        CU_ASSERT_EQUAL((int)node->pending_out, 0);
        CU_ASSERT_EQUAL((int)node->pending_in2, 0);
        CU_ASSERT_EQUAL((int)node->pending_out2, 0);
        CU_ASSERT_PTR_NULL(node->inbuf);
        CU_ASSERT_PTR_NULL(node->outbuf);
        CU_ASSERT_PTR_NULL(node->sendreq);
        CU_ASSERT_PTR_NULL(node->recvreq);
        CU_ASSERT_PTR_NULL(node->sendreq2);
        CU_ASSERT_PTR_NULL(node->recvreq2);
        CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
        CU_ASSERT_EQUAL(node->board->data->count_current, 0);
        return;
    }
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* post */
    CU_ASSERT_EQUAL(node->stage, MB_COMM_HANDSHAKE_START_PROP);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->incount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->outcount);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->recvreq);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq2);
    CU_ASSERT_PTR_NULL(node->sendreq2);
    
    CU_ASSERT_PTR_NULL(node->board->tt);
    CU_ASSERT_EQUAL(node->board->locked, MB_TRUE);
    CU_ASSERT_EQUAL(node->board->syncCompleted, MB_FALSE);
    
    CU_ASSERT_EQUAL((int)node->pending_in, node->board->writer_count);
    CU_ASSERT_EQUAL((int)node->pending_out, node->board->reader_count);
    
    if (MBI_CommRank % 2 == 0) /* even. RO */
    {
        CU_ASSERT_EQUAL(node->board->reader_count, 0);
        CU_ASSERT_EQUAL(node->board->writer_count, MBI_CommSize / 2);
        CU_ASSERT_EQUAL((int)node->pending_in, (MBI_CommSize / 2));
        CU_ASSERT_EQUAL((int)node->pending_out, 0);
    }
    else /* odd. WO */
    {
        if (MBI_CommSize % 2 == 0)
        {
            CU_ASSERT_EQUAL(node->board->reader_count, (MBI_CommSize / 2));
            CU_ASSERT_EQUAL((int)node->pending_out, (MBI_CommSize / 2));
        }
        else
        {
            CU_ASSERT_EQUAL(node->board->reader_count, (MBI_CommSize / 2) + 1);
            CU_ASSERT_EQUAL((int)node->pending_out, (MBI_CommSize / 2) + 1);
        }
        CU_ASSERT_EQUAL(node->board->writer_count, 0);
        CU_ASSERT_EQUAL((int)node->pending_in, 0);

    }
    
    CU_ASSERT_EQUAL(node->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node->flag_shareOutbuf, MB_TRUE);
    
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank) /* self */
        {
            CU_ASSERT_EQUAL(node->incount[i], -1);
            CU_ASSERT_EQUAL(node->outcount[i], -1);
            CU_ASSERT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
        }
        else
        {
            if (MBI_CommRank % 2 == 0) /* I am RO */
            {
                if (i % 2 == 0) /* from a RO node */
                {
                    CU_ASSERT_EQUAL(node->outcount[i], -1);
                    CU_ASSERT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
                    CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
                }
                else /* from a WO node */
                {

                    CU_ASSERT_EQUAL(node->outcount[i], -1);
                    CU_ASSERT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
                    CU_ASSERT_NOT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
                }
            }
            else /* I am WO */
            {
                if (i % 2 == 0) /* to a RO node */
                {
                    CU_ASSERT_EQUAL(node->outcount[i], TEST_MESSAGE_COUNT);
                    CU_ASSERT_NOT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
                    CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
                }
                else /* from a WO node */
                {
                    CU_ASSERT_EQUAL(node->outcount[i], -1);
                    CU_ASSERT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
                    CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
                }
            }
        }
    }
}
