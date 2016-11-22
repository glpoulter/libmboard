/* $Id: test_cr_old_sendbufinfo.c 2108 2009-09-14 14:14:31Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */
#include "header_commroutines_old.h"

void test_cr_old_sendbufinfo(void) {
    
    int rc, i, top, bottom;
    
    /* -------- check critical prerequisites ------------ */
    
    /* stage */
    CU_ASSERT_EQUAL_FATAL(node_filter->stage, MB_COMM_OLD_READY_FOR_PROP);
    CU_ASSERT_EQUAL_FATAL(node_nofilter->stage, MB_COMM_OLD_READY_FOR_PROP);
    CU_ASSERT_EQUAL_FATAL(node_filter_fdr->stage, MB_COMM_OLD_READY_FOR_PROP);
    CU_ASSERT_EQUAL_FATAL(node_empty->stage, MB_COMM_OLD_READY_FOR_PROP);
    CU_ASSERT_EQUAL_FATAL(node_empty_filter->stage, MB_COMM_OLD_READY_FOR_PROP);
    
    /* board ptr cache set */
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_filter->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_nofilter->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_filter_fdr->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_empty->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_empty_filter->board);
    
    
    /* ----- run func ----- */
    rc = MBI_CommRoutine_OLD_SendBufInfo(node_filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_CommRoutine_OLD_SendBufInfo(node_nofilter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_CommRoutine_OLD_SendBufInfo(node_filter_fdr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_CommRoutine_OLD_SendBufInfo(node_empty);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_CommRoutine_OLD_SendBufInfo(node_empty_filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    
    /* ------ check data buffers -------------- */
    if (MBI_CommSize > 1)
    {
        CU_ASSERT_EQUAL(node_nofilter->flag_shareOutbuf, MB_TRUE);
        if (node_nofilter->outcount != NULL)
        {
            for (i = 0; i < MBI_CommSize; i++)
            {
                if (i == MBI_CommRank)
                {
                    CU_ASSERT_EQUAL(node_nofilter->outcount[i], 0);
                }
                else
                {
                    CU_ASSERT_EQUAL(node_nofilter->outcount[i], TEST_MESSAGE_COUNT);
                }
            }
        }
    }
    else
    {
        CU_ASSERT_EQUAL(node_nofilter->flag_shareOutbuf, MB_FALSE);
        CU_ASSERT_EQUAL(node_nofilter->outcount[0], 0);
    }
    
    if (MBI_CommSize > 3)
    {
        CU_ASSERT_EQUAL(node_filter_fdr->flag_shareOutbuf, MB_TRUE);
        if (node_filter_fdr->outcount != NULL)
        {
            for (i = 0; i < MBI_CommSize; i++)
            {
                if (i == MBI_CommRank)
                {
                    CU_ASSERT_EQUAL(node_nofilter->outcount[i], 0);
                }
                else
                {
                    CU_ASSERT_EQUAL(node_nofilter->outcount[i], TEST_MESSAGE_COUNT);
                }
            }
        }
    }
    else if (MBI_CommSize == 1)
    {
        CU_ASSERT_EQUAL(node_filter_fdr->flag_shareOutbuf, MB_FALSE);
        CU_ASSERT_EQUAL(node_filter_fdr->outcount[0], 0);
    }
    else
    {
        CU_ASSERT_EQUAL(node_filter_fdr->flag_shareOutbuf, MB_FALSE);
        if (node_filter_fdr->outcount != NULL)
        {
            for (i = 0; i < MBI_CommSize; i++)
            {
                if (i == MBI_CommRank)
                {
                    CU_ASSERT_EQUAL(node_filter_fdr->outcount[i], 0);
                }
                else
                {
                    CU_ASSERT_EQUAL(node_filter_fdr->outcount[i], TEST_MESSAGE_COUNT/2);
                }
            
            }
        }
    }
    
    CU_ASSERT_EQUAL(node_filter->flag_shareOutbuf, MB_FALSE);
    if (node_filter->outcount != NULL)
    {
        top = (MBI_CommRank == MBI_CommSize - 1) ? 0 : MBI_CommRank + 1;
        bottom = (MBI_CommRank == 0) ? MBI_CommSize - 1 : MBI_CommRank - 1;
    
        if (MBI_CommSize == 1)
        {
            CU_ASSERT_EQUAL(node_filter->outcount[0],0);
        }
        else
        {
            for (i = 0; i < MBI_CommSize; i++)
            {
                if (i == top || i == bottom) 
                {
                    if (top == bottom)
                    {
                        CU_ASSERT_EQUAL(node_filter->outcount[i], 2*SMALLNUM);
                    }
                    else
                    {
                        CU_ASSERT_EQUAL(node_filter->outcount[i], SMALLNUM);
                    }
                }
                else
                {
                    CU_ASSERT_EQUAL(node_filter->outcount[i], 0);
                }
            }
        }
    }
    
    CU_ASSERT_EQUAL(node_empty->flag_shareOutbuf, MB_FALSE);
    if (node_empty->outcount != NULL)
    {
        for (i = 0; i < MBI_CommSize; i++)
        {
            CU_ASSERT_EQUAL(node_empty->outcount[i], 0);
        }
    }
    
    CU_ASSERT_EQUAL(node_empty_filter->flag_shareOutbuf, MB_FALSE);
    if (node_empty_filter->outcount != NULL)
    {
        for (i = 0; i < MBI_CommSize; i++)
        {
            CU_ASSERT_EQUAL(node_empty_filter->outcount[i], 0);
        }
    }
    
    /* -------- check post conditions ------------ */
    
    /* ready for next stage */
    CU_ASSERT_EQUAL(node_filter->stage, MB_COMM_OLD_BUFINFO_SENT);
    CU_ASSERT_EQUAL(node_nofilter->stage, MB_COMM_OLD_BUFINFO_SENT);
    CU_ASSERT_EQUAL(node_filter_fdr->stage, MB_COMM_OLD_BUFINFO_SENT);
    CU_ASSERT_EQUAL(node_empty->stage, MB_COMM_OLD_BUFINFO_SENT);
    CU_ASSERT_EQUAL(node_empty_filter->stage, MB_COMM_OLD_BUFINFO_SENT);
    
    /* incount array allocated */
    CU_ASSERT_PTR_NOT_NULL(node_filter->incount);
    CU_ASSERT_PTR_NOT_NULL(node_nofilter->incount);
    CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->incount);
    CU_ASSERT_PTR_NOT_NULL(node_empty->incount);
    CU_ASSERT_PTR_NOT_NULL(node_empty_filter->incount);
    
    /* outcount array allocated */
    CU_ASSERT_PTR_NOT_NULL(node_filter->outcount);
    CU_ASSERT_PTR_NOT_NULL(node_nofilter->outcount);
    CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->outcount);
    CU_ASSERT_PTR_NOT_NULL(node_empty->outcount);
    CU_ASSERT_PTR_NOT_NULL(node_empty_filter->outcount);
    
    /* sendreq array allocated */
    CU_ASSERT_PTR_NOT_NULL(node_filter->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node_nofilter->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node_empty->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node_empty_filter->sendreq);
    
    /* recvreq array allocated */
    CU_ASSERT_PTR_NOT_NULL(node_filter->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node_nofilter->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node_empty->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node_empty_filter->recvreq);
    
    /* pending_in set */
    CU_ASSERT_EQUAL(node_filter->pending_in, 1);
    CU_ASSERT_EQUAL(node_nofilter->pending_in, 1);
    CU_ASSERT_EQUAL(node_filter_fdr->pending_in, 1);
    CU_ASSERT_EQUAL(node_empty->pending_in, 1);
    CU_ASSERT_EQUAL(node_empty_filter->pending_in, 1);
    
    /* pending_out set */
    CU_ASSERT_EQUAL(node_filter->pending_out, 1);
    CU_ASSERT_EQUAL(node_nofilter->pending_out, 1);
    CU_ASSERT_EQUAL(node_filter_fdr->pending_out, 1);
    CU_ASSERT_EQUAL(node_empty->pending_out, 1);
    CU_ASSERT_EQUAL(node_empty_filter->pending_out, 1);
}
