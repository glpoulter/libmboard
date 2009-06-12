/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */
#include "header_commroutines.h"

#define BIT_IS_SET(byte, mask) ((byte & mask) == mask)

void test_cr_initpropagation(void) {
    
    int i, rc, top, bottom;
    int bit_set;
    char *header_byte;
    /* -------- check critical prerequisites ------------ */
    
    /* stage */
    CU_ASSERT_EQUAL_FATAL(node_filter->stage, PRE_PROPAGATION);
    CU_ASSERT_EQUAL_FATAL(node_nofilter->stage, PRE_PROPAGATION);
    CU_ASSERT_EQUAL_FATAL(node_filter_fdr->stage, PRE_PROPAGATION);
    CU_ASSERT_EQUAL_FATAL(node_empty->stage, PRE_PROPAGATION);
    CU_ASSERT_EQUAL_FATAL(node_empty_filter->stage, PRE_PROPAGATION);
    
    /* board ptr cache set */
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_filter->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_nofilter->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_filter_fdr->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_empty->board);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_empty_filter->board);
    
    /* ----- run func ----- */
    rc = MBI_Comm_InitPropagation(node_filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_Comm_InitPropagation(node_nofilter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_Comm_InitPropagation(node_filter_fdr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_Comm_InitPropagation(node_empty);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_Comm_InitPropagation(node_empty_filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* ------ check data buffers -------------- */
    
    /* - filtered board where we only send messages to prev/next proc */
    /* check number of pending comms */
    if (MBI_CommSize > 2)
    {
        CU_ASSERT_EQUAL((int)node_filter->pending_out, 2);
        CU_ASSERT_EQUAL((int)node_filter->pending_in,  2);
    }
    else 
    {
        CU_ASSERT_EQUAL((int)node_filter->pending_out, MBI_CommSize - 1);
        CU_ASSERT_EQUAL((int)node_filter->pending_in,  MBI_CommSize - 1);
    }
    CU_ASSERT_EQUAL(node_filter->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node_filter->flag_shareOutbuf, MB_FALSE);
    /* check data in outbuf */
    CU_ASSERT_PTR_NOT_NULL(node_filter->outbuf);
    if (node_filter->outbuf != NULL)
    {
        if (MBI_CommSize == 1)
        {
            CU_ASSERT_PTR_NULL(node_filter->outbuf[0]);
        }
        else
        {
            top = (MBI_CommRank == MBI_CommSize-1) ? 0 : MBI_CommRank+1;
            bottom = (MBI_CommRank == 0) ? MBI_CommSize-1 : MBI_CommRank-1;
            for (i = 0; i < MBI_CommSize; i++)
            {
                if (i == top || i == bottom)
                {
                    CU_ASSERT_PTR_NOT_NULL(node_filter->outbuf[i]);
                    header_byte = (char *)node_filter->outbuf[i];
                    if (header_byte != NULL)
                    {
                        bit_set =  BIT_IS_SET(*header_byte, MBI_COMM_HEADERBYTE_FDR);
                        CU_ASSERT_EQUAL(bit_set, 0);
                    }
                }
                else
                {
                    CU_ASSERT_PTR_NULL(node_filter->outbuf[i]);
                }
            }
        }
    }


    
    /* - non filtered board . all messages are send to all procs */
    /* check number of pending comms */
    CU_ASSERT_EQUAL((int)node_nofilter->pending_out, MBI_CommSize - 1);
    CU_ASSERT_EQUAL((int)node_nofilter->pending_in, MBI_CommSize -1);
    CU_ASSERT_EQUAL(node_nofilter->flag_fdrFallback, MB_FALSE);
    if (MBI_CommSize == 1)
    {
        CU_ASSERT_EQUAL(node_nofilter->flag_shareOutbuf, MB_FALSE);
    }
    else
    {
        CU_ASSERT_EQUAL(node_nofilter->flag_shareOutbuf, MB_TRUE);
    }
    /* check data in outbuf */
    CU_ASSERT_PTR_NOT_NULL(node_nofilter->outbuf);
    if (node_filter->outbuf != NULL)
    {
        if (MBI_CommSize == 1)
        {
            CU_ASSERT_PTR_NULL(node_nofilter->outbuf[0]);
        }
        else
        {
            CU_ASSERT_PTR_NOT_NULL(node_nofilter->outbuf[0]);
            header_byte = (char *)node_nofilter->outbuf[0];
            if (header_byte != NULL)
            {
                bit_set =  BIT_IS_SET(*header_byte, MBI_COMM_HEADERBYTE_FDR);
                CU_ASSERT_EQUAL(bit_set, 0);
            }
            for (i = 1; i < MBI_CommSize; i++)
            {
                CU_ASSERT_PTR_NULL(node_nofilter->outbuf[i]);
            }
        }
    }

    
    
    /* - filtered board will fallback to full data replication */
    /* check number of pending comms */
    CU_ASSERT_EQUAL((int)node_filter_fdr->pending_out, MBI_CommSize - 1);
    CU_ASSERT_EQUAL((int)node_filter_fdr->pending_in, MBI_CommSize -1);
    if (MBI_CommSize > 3)
    {
        CU_ASSERT_EQUAL(node_filter_fdr->flag_fdrFallback, MB_TRUE);
        CU_ASSERT_EQUAL(node_filter_fdr->flag_shareOutbuf, MB_TRUE);
    
        /* check data in outbuf */
        CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->outbuf);
        if (node_filter->outbuf != NULL)
        {
            CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->outbuf[0]);
            header_byte = (char *)node_filter_fdr->outbuf[0];
            if (header_byte != NULL)
            {
                bit_set =  BIT_IS_SET(*header_byte, MBI_COMM_HEADERBYTE_FDR);
                CU_ASSERT_EQUAL(bit_set, 1);
            }
            for (i = 1; i < MBI_CommSize; i++)
            {
                CU_ASSERT_PTR_NULL(node_filter_fdr->outbuf[i]);
            }
        }
    }
    else
    {
        CU_ASSERT_EQUAL(node_filter_fdr->flag_fdrFallback, MB_FALSE);
        CU_ASSERT_EQUAL(node_filter_fdr->flag_shareOutbuf, MB_FALSE);
        
        /* check data in outbuf */
        CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->outbuf);
        if (node_filter->outbuf != NULL)
        {
            if (MBI_CommSize == 1)
            {
                CU_ASSERT_PTR_NULL(node_filter_fdr->outbuf[0]);
            }
            else
            {
                if (MBI_CommSize > 3)
                {
                    CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->outbuf[0]);
                    if (node_filter_fdr->outbuf[0] != NULL)
                    {
                        header_byte = (char *)node_filter_fdr->outbuf[0];
                        if (header_byte != NULL)
                        {
                            bit_set =  BIT_IS_SET(*header_byte, MBI_COMM_HEADERBYTE_FDR);
                            CU_ASSERT_EQUAL(bit_set, 1);
                        }
                    }
                }
                else
                {
                    for (i = 0; i < MBI_CommSize; i++)
                    {
                        if (i == MBI_CommRank)
                        {
                            CU_ASSERT_PTR_NULL(node_filter_fdr->outbuf[i]);
                        }
                        else
                        {
                            CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->outbuf[i]);
                            if (node_filter_fdr->outbuf[i] != NULL)
                            {
                                header_byte = (char *)node_filter_fdr->outbuf[i];
                                if (header_byte != NULL)
                                {
                                    bit_set =  BIT_IS_SET(*header_byte, MBI_COMM_HEADERBYTE_FDR);
                                    CU_ASSERT_EQUAL(bit_set, 0);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    /* - empty board . no sends */
    /* check number of pending comms */
    CU_ASSERT_EQUAL((int)node_empty->pending_out, 0);
    CU_ASSERT_EQUAL((int)node_empty->pending_in, 0);
    CU_ASSERT_EQUAL(node_empty->flag_fdrFallback, MB_FALSE);
    if (MBI_CommSize == 1)
    {
        CU_ASSERT_EQUAL(node_nofilter->flag_shareOutbuf, MB_FALSE);
    }
    else
    {
        CU_ASSERT_EQUAL(node_nofilter->flag_shareOutbuf, MB_TRUE);
    }
    /* check data in outbuf */
    CU_ASSERT_PTR_NOT_NULL(node_empty->outbuf);
    if (node_empty->outbuf != NULL)
    {
        for (i = 0; i < MBI_CommSize; i++)
        {
            CU_ASSERT_PTR_NULL(node_empty->outbuf[i]);
        }
    }
    
    /* - empty board . no sends */
    /* check number of pending comms */
    CU_ASSERT_EQUAL((int)node_empty_filter->pending_out, 0);
    CU_ASSERT_EQUAL((int)node_empty_filter->pending_in, 0);
    CU_ASSERT_EQUAL(node_empty_filter->flag_fdrFallback, MB_FALSE);
    if (MBI_CommSize == 1)
    {
        CU_ASSERT_EQUAL(node_nofilter->flag_shareOutbuf, MB_FALSE);
    }
    else
    {
        CU_ASSERT_EQUAL(node_nofilter->flag_shareOutbuf, MB_TRUE);
    }
    /* check data in outbuf */
    CU_ASSERT_PTR_NOT_NULL(node_empty_filter->outbuf);
    if (node_filter->outbuf != NULL)
    {
        for (i = 0; i < MBI_CommSize; i++)
        {
            CU_ASSERT_PTR_NULL(node_empty->outbuf[i]);
        }
    }
    
    /* -------- check post conditions ------------ */
    
    /* ready for next stage */
    CU_ASSERT_EQUAL(node_filter->stage, PROPAGATION);
    CU_ASSERT_EQUAL(node_nofilter->stage, PROPAGATION);
    CU_ASSERT_EQUAL(node_filter_fdr->stage, PROPAGATION);
    CU_ASSERT_EQUAL(node_empty->stage, PROPAGATION);
    CU_ASSERT_EQUAL(node_empty_filter->stage, PROPAGATION);
    
    /* check that outcount has deallocated */
    CU_ASSERT_PTR_NULL(node_filter->outcount);
    CU_ASSERT_PTR_NULL(node_nofilter->outcount);
    CU_ASSERT_PTR_NULL(node_filter_fdr->outcount);
    CU_ASSERT_PTR_NULL(node_empty->outcount);
    CU_ASSERT_PTR_NULL(node_empty_filter->outcount);
    
    /* check that tagtable has deallocated */
    CU_ASSERT_PTR_NULL(node_filter->board->tt);
    CU_ASSERT_PTR_NULL(node_nofilter->board->tt);
    CU_ASSERT_PTR_NULL(node_filter_fdr->board->tt);
    CU_ASSERT_PTR_NULL(node_empty->board->tt);
    CU_ASSERT_PTR_NULL(node_empty_filter->board->tt);
    
    /* check that outbuf is allocated */
    CU_ASSERT_PTR_NOT_NULL(node_filter->outbuf);
    CU_ASSERT_PTR_NOT_NULL(node_nofilter->outbuf);
    CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->outbuf);
    CU_ASSERT_PTR_NOT_NULL(node_empty->outbuf);
    CU_ASSERT_PTR_NOT_NULL(node_empty_filter->outbuf);
    
    /* check that inbuf is allocated */
    CU_ASSERT_PTR_NOT_NULL(node_filter->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node_nofilter->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node_empty->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node_empty_filter->inbuf);
    
    /* check that sendreq is allocated */
    CU_ASSERT_PTR_NOT_NULL(node_filter->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node_nofilter->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node_empty->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node_empty_filter->sendreq);
    
    /* check that recvreq is allocated */
    CU_ASSERT_PTR_NOT_NULL(node_filter->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node_nofilter->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node_empty->recvreq);
    CU_ASSERT_PTR_NOT_NULL(node_empty_filter->recvreq);
}
