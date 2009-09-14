/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */
#include "header_commroutines_old.h"

void test_cr_old_completepropagation(void) {
    
    int rc = -1;
    
    /* -------- check critical prerequisites ------------ */
    
    /* stage */
    CU_ASSERT_EQUAL_FATAL(node_filter->stage, MB_COMM_OLD_PROPAGATION);
    CU_ASSERT_EQUAL_FATAL(node_nofilter->stage, MB_COMM_OLD_PROPAGATION);
    CU_ASSERT_EQUAL_FATAL(node_filter_fdr->stage, MB_COMM_OLD_PROPAGATION);
    CU_ASSERT_EQUAL_FATAL(node_empty->stage, MB_COMM_OLD_PROPAGATION);
    CU_ASSERT_EQUAL_FATAL(node_empty_filter->stage, MB_COMM_OLD_PROPAGATION);
    
    /* board ptr cache set */
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_filter->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_nofilter->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_filter_fdr->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_empty->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_empty_filter->board);
    
    /* ----- run function till state changes ------ */
    
    /* keep calling until comms have completed */
    while (node_filter->stage == MB_COMM_OLD_PROPAGATION)
    {
        rc = MBI_CommRoutine_OLD_CompletePropagation(node_filter);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("Call to MBI_CommRoutine_OLD_CompletePropagation for node_filter failed");
            break;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS_2);
    
    /* keep calling until comms have completed */
    while (node_nofilter->stage == MB_COMM_OLD_PROPAGATION)
    {
        rc = MBI_CommRoutine_OLD_CompletePropagation(node_nofilter);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("Call to MBI_CommRoutine_OLD_CompletePropagation for node_nofilter failed");
            break;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS_2);
    
    /* keep calling until comms have completed */
    while (node_filter_fdr->stage == MB_COMM_OLD_PROPAGATION)
    {
        rc = MBI_CommRoutine_OLD_CompletePropagation(node_filter_fdr);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("Call to MBI_CommRoutine_OLD_CompletePropagation for node_filter_fdr failed");
            break;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS_2);
    
    /* keep calling until comms have completed */
    while (node_empty->stage == MB_COMM_OLD_PROPAGATION)
    {
        rc = MBI_CommRoutine_OLD_CompletePropagation(node_empty);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("Call to MBI_CommRoutine_OLD_CompletePropagation for node_empty failed");
            break;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS_2);
    
    /* keep calling until comms have completed */
    while (node_empty_filter->stage == MB_COMM_OLD_PROPAGATION)
    {
        rc = MBI_CommRoutine_OLD_CompletePropagation(node_empty_filter);
        if (rc != MB_SUCCESS && rc != MB_SUCCESS_2)
        {
            CU_FAIL("Call to MBI_CommRoutine_OLD_CompletePropagation for node_empty_filter failed");
            break;
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS_2);
    
    /* -------- check board content --------- */
    if (MBI_CommSize > 1)
    {
        CU_ASSERT_EQUAL((int)node_filter->board->data->count_current, 
                TEST_MESSAGE_COUNT + (2*SMALLNUM));
    }
    else
    {
        CU_ASSERT_EQUAL((int)node_filter->board->data->count_current, 
                TEST_MESSAGE_COUNT);
    }
    CU_ASSERT_EQUAL((int)node_nofilter->board->data->count_current, 
            TEST_MESSAGE_COUNT * MBI_CommSize);
    CU_ASSERT_EQUAL((int)node_filter_fdr->board->data->count_current, 
            TEST_MESSAGE_COUNT + (TEST_MESSAGE_COUNT/2)*(MBI_CommSize-1));
    CU_ASSERT_EQUAL((int)node_empty->board->data->count_current, 0);
    CU_ASSERT_EQUAL((int)node_empty_filter->board->data->count_current, 0);
    
    /* -------- check post conditions ------------ */
    
    /* ready for next stage */
    CU_ASSERT_EQUAL(node_filter->stage, MB_COMM_END);
    CU_ASSERT_EQUAL(node_nofilter->stage, MB_COMM_END);
    CU_ASSERT_EQUAL(node_filter_fdr->stage, MB_COMM_END);
    CU_ASSERT_EQUAL(node_empty->stage, MB_COMM_END);
    CU_ASSERT_EQUAL(node_empty_filter->stage, MB_COMM_END);
    
    /* pending_in set */
    CU_ASSERT_EQUAL(node_filter->pending_in, 0);
    CU_ASSERT_EQUAL(node_nofilter->pending_in, 0);
    CU_ASSERT_EQUAL(node_filter_fdr->pending_in, 0);
    CU_ASSERT_EQUAL(node_empty->pending_in, 0);
    CU_ASSERT_EQUAL(node_empty_filter->pending_in, 0);
    
    /* pending_out set */
    CU_ASSERT_EQUAL(node_filter->pending_out, 0);
    CU_ASSERT_EQUAL(node_nofilter->pending_out, 0);
    CU_ASSERT_EQUAL(node_filter_fdr->pending_out, 0);
    CU_ASSERT_EQUAL(node_empty->pending_out, 0);
    CU_ASSERT_EQUAL(node_empty_filter->pending_out, 0);
    
    /* check that incount has deallocated */
    CU_ASSERT_PTR_NULL(node_filter->incount);
    CU_ASSERT_PTR_NULL(node_nofilter->incount);
    CU_ASSERT_PTR_NULL(node_filter_fdr->incount);
    CU_ASSERT_PTR_NULL(node_empty->incount);
    CU_ASSERT_PTR_NULL(node_empty_filter->incount);
    
    /* check that outcount has deallocated */
    CU_ASSERT_PTR_NULL(node_filter->outcount);
    CU_ASSERT_PTR_NULL(node_nofilter->outcount);
    CU_ASSERT_PTR_NULL(node_filter_fdr->outcount);
    CU_ASSERT_PTR_NULL(node_empty->outcount);
    CU_ASSERT_PTR_NULL(node_empty_filter->outcount);
    
    /* check that sendreq has deallocated */
    CU_ASSERT_PTR_NULL(node_filter->sendreq);
    CU_ASSERT_PTR_NULL(node_nofilter->sendreq);
    CU_ASSERT_PTR_NULL(node_filter_fdr->sendreq);
    CU_ASSERT_PTR_NULL(node_empty->sendreq);
    CU_ASSERT_PTR_NULL(node_empty_filter->sendreq);
    
    /* check that recvreq has deallocated */
    CU_ASSERT_PTR_NULL(node_filter->recvreq);
    CU_ASSERT_PTR_NULL(node_nofilter->recvreq);
    CU_ASSERT_PTR_NULL(node_filter_fdr->recvreq);
    CU_ASSERT_PTR_NULL(node_empty->recvreq);
    CU_ASSERT_PTR_NULL(node_empty_filter->recvreq);
    
    /* check sync completed flag */
    CU_ASSERT_EQUAL(node_filter->board->syncCompleted, MB_TRUE);
    CU_ASSERT_EQUAL(node_nofilter->board->syncCompleted, MB_TRUE);
    CU_ASSERT_EQUAL(node_filter_fdr->board->syncCompleted, MB_TRUE);
    CU_ASSERT_EQUAL(node_empty->board->syncCompleted, MB_TRUE);
    CU_ASSERT_EQUAL(node_empty_filter->board->syncCompleted, MB_TRUE);

    
}
