/* $Id: test_cr_old_waitbufinfo.c 2108 2009-09-14 14:14:31Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */
#include "header_commroutines_old.h"

void test_cr_old_waitbufinfo(void) {
    
    int rc, i, top, bottom;
    
    /* -------- check critical prerequisites ------------ */
    
    /* stage */
    CU_ASSERT_EQUAL_FATAL(node_filter->stage, MB_COMM_OLD_BUFINFO_SENT);
    CU_ASSERT_EQUAL_FATAL(node_nofilter->stage, MB_COMM_OLD_BUFINFO_SENT);
    CU_ASSERT_EQUAL_FATAL(node_filter_fdr->stage, MB_COMM_OLD_BUFINFO_SENT);
    CU_ASSERT_EQUAL_FATAL(node_empty->stage, MB_COMM_OLD_BUFINFO_SENT);
    CU_ASSERT_EQUAL_FATAL(node_empty_filter->stage, MB_COMM_OLD_BUFINFO_SENT);
    
    /* board ptr cache set */
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_filter->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_nofilter->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_filter_fdr->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_empty->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_empty_filter->board);
    
    
    /* ----- run function till state changes ------ */
    
    /* keep calling until comms have completed */
    while (node_filter->stage == MB_COMM_OLD_BUFINFO_SENT)
    {
        rc = MBI_CommRoutine_OLD_WaitBufInfo(node_filter);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("Call to MBI_CommRoutine_OLD_WaitBufInfo for node_filter failed");
            break;
        }
    }
    
    /* keep calling until comms have completed */
    while (node_nofilter->stage == MB_COMM_OLD_BUFINFO_SENT)
    {
        rc = MBI_CommRoutine_OLD_WaitBufInfo(node_nofilter);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("Call to MBI_CommRoutine_OLD_WaitBufInfo for node_nofilter failed");
            break;
        }
    }
    
    /* keep calling until comms have completed */
    while (node_filter_fdr->stage == MB_COMM_OLD_BUFINFO_SENT)
    {
        rc = MBI_CommRoutine_OLD_WaitBufInfo(node_filter_fdr);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("Call to MBI_CommRoutine_OLD_WaitBufInfo for node_filter_fdr failed");
            break;
        }
    }
    
    /* keep calling until comms have completed */
    while (node_empty->stage == MB_COMM_OLD_BUFINFO_SENT)
    {
        rc = MBI_CommRoutine_OLD_WaitBufInfo(node_empty);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("Call to MBI_CommRoutine_OLD_WaitBufInfo for node_empty failed");
            break;
        }
    }
    
    /* keep calling until comms have completed */
    while (node_empty_filter->stage == MB_COMM_OLD_BUFINFO_SENT)
    {
        rc = MBI_CommRoutine_OLD_WaitBufInfo(node_empty_filter);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("Call to MBI_CommRoutine_OLD_WaitBufInfo for node_empty_filter failed");
            break;
        }
    }
    
    /* ------ check data buffers -------------- */

    if (MBI_CommSize == 1)
    {
        CU_ASSERT_EQUAL(node_nofilter->flag_shareOutbuf, MB_FALSE);
        CU_ASSERT_EQUAL(node_nofilter->incount[0], 0);
    }
    else
    {
        CU_ASSERT_EQUAL(node_nofilter->flag_shareOutbuf, MB_TRUE);
        if (node_nofilter->incount != NULL)
        {
            for (i = 0; i < MBI_CommSize; i++)
            {
                if (i == MBI_CommRank) 
                {
                    CU_ASSERT_EQUAL(node_nofilter->incount[i], 0);
                }
                else 
                {
                    CU_ASSERT_EQUAL(node_nofilter->incount[i], TEST_MESSAGE_COUNT);
                }
            }
        }
    }
    
    if (MBI_CommSize > 3)
    {
        CU_ASSERT_EQUAL(node_filter_fdr->flag_shareOutbuf, MB_TRUE);
        if (node_filter_fdr->incount != NULL)
        {
            for (i = 0; i < MBI_CommSize; i++)
            {
                if (i == MBI_CommRank) 
                {
                    CU_ASSERT_EQUAL(node_filter_fdr->incount[i], 0);
                }
                else 
                {
                    CU_ASSERT_EQUAL(node_filter_fdr->incount[i], TEST_MESSAGE_COUNT);
                }
            }
        }
    }
    else
    {
        CU_ASSERT_EQUAL(node_filter_fdr->flag_shareOutbuf, MB_FALSE);
        if (node_filter_fdr->incount != NULL)
        {
            for (i = 0; i < MBI_CommSize; i++)
            {
                if (i == MBI_CommRank) 
                {
                    CU_ASSERT_EQUAL(node_filter_fdr->incount[i], 0);
                }
                else 
                {
                    CU_ASSERT_EQUAL(node_filter_fdr->incount[i], TEST_MESSAGE_COUNT/2);
                }
            }
        }
    }
    
    CU_ASSERT_EQUAL(node_filter->flag_shareOutbuf, MB_FALSE);
    if (node_filter->incount != NULL)
    {
        top = (MBI_CommRank == MBI_CommSize - 1) ? 0 : MBI_CommRank + 1;
        bottom = (MBI_CommRank == 0) ? MBI_CommSize - 1 : MBI_CommRank - 1;
        
        if (MBI_CommSize == 1)
        {
            CU_ASSERT_EQUAL(node_filter->incount[0], 0);
        }
        else
        {
            for (i = 0; i < MBI_CommSize; i++)
            {
                if (i == top || i == bottom) 
                {
                    if (top == bottom)
                    {
                        CU_ASSERT_EQUAL(node_filter->incount[i], 2*SMALLNUM);
                    }
                    else
                    {
                        CU_ASSERT_EQUAL(node_filter->incount[i], SMALLNUM);
                    }
                }
                else
                {
                    CU_ASSERT_EQUAL(node_filter->incount[i], 0);
                }
            }
        }
    }
    
    CU_ASSERT_EQUAL(node_empty->flag_shareOutbuf, MB_FALSE);
    if (node_empty->incount != NULL)
    {
        for (i = 0; i < MBI_CommSize; i++)
        {
            if (i == MBI_CommRank) 
            {
                CU_ASSERT_EQUAL(node_empty->incount[i], 0);
            }
            else 
            {
                CU_ASSERT_EQUAL(node_empty->incount[i], 0);
            }
        }
    }
    
    CU_ASSERT_EQUAL(node_empty_filter->flag_shareOutbuf, MB_FALSE);
    if (node_empty_filter->incount != NULL)
    {
        for (i = 0; i < MBI_CommSize; i++)
        {
            if (i == MBI_CommRank) 
            {
                CU_ASSERT_EQUAL(node_empty_filter->incount[i], 0);
            }
            else 
            {
                CU_ASSERT_EQUAL(node_empty_filter->incount[i], 0);
            }
        }
    }
    
    /* -------- check post conditions ------------ */
    
    /* ready for next stage */
    CU_ASSERT_EQUAL(node_filter->stage, MB_COMM_OLD_PRE_PROPAGATION);
    CU_ASSERT_EQUAL(node_nofilter->stage, MB_COMM_OLD_PRE_PROPAGATION);
    CU_ASSERT_EQUAL(node_filter_fdr->stage, MB_COMM_OLD_PRE_PROPAGATION);
    CU_ASSERT_EQUAL(node_empty->stage, MB_COMM_OLD_PRE_PROPAGATION);
    CU_ASSERT_EQUAL(node_empty_filter->stage, MB_COMM_OLD_PRE_PROPAGATION);
    
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
    
    
}
