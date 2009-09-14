/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */
#include "header_commroutines_old.h"

void test_cr_old_tagmessages(void) {
    
    int rc;
    MBIt_Board *board;
    
    /* ------------ check prerequisites ------------------- */
    
    /* message board handles are valid and boards are in the right state */
    board = (MBIt_Board *)MBI_getMBoardRef(node_filter->mb);
    CU_ASSERT_PTR_NOT_NULL(board);
    if (board != NULL)
    {
        /* board locked */
        CU_ASSERT_EQUAL(board->locked, MB_TRUE);
        /* syncCompleted flag set */
        CU_ASSERT_EQUAL(board->syncCompleted, MB_FALSE);
    }
    board = (MBIt_Board *)MBI_getMBoardRef(node_nofilter->mb);
    CU_ASSERT_PTR_NOT_NULL(board);
    if (board != NULL)
    {
        /* board locked */
        CU_ASSERT_EQUAL(board->locked, MB_TRUE);
        /* syncCompleted flag set */
        CU_ASSERT_EQUAL(board->syncCompleted, MB_FALSE);
    }
    board = (MBIt_Board *)MBI_getMBoardRef(node_filter_fdr->mb);
    CU_ASSERT_PTR_NOT_NULL(board);
    if (board != NULL)
    {
        /* board locked */
        CU_ASSERT_EQUAL(board->locked, MB_TRUE);
        /* syncCompleted flag set */
        CU_ASSERT_EQUAL(board->syncCompleted, MB_FALSE);
    }
    board = (MBIt_Board *)MBI_getMBoardRef(node_empty->mb);
    CU_ASSERT_PTR_NOT_NULL(board);
    if (board != NULL)
    {
        /* board locked */
        CU_ASSERT_EQUAL(board->locked, MB_TRUE);
        /* syncCompleted flag set */
        CU_ASSERT_EQUAL(board->syncCompleted, MB_FALSE);
    }
    board = (MBIt_Board *)MBI_getMBoardRef(node_empty_filter->mb);
    CU_ASSERT_PTR_NOT_NULL(board);
    if (board != NULL)
    {
        /* board locked */
        CU_ASSERT_EQUAL(board->locked, MB_TRUE);
        /* syncCompleted flag set */
        CU_ASSERT_EQUAL(board->syncCompleted, MB_FALSE);
    }
    
    /* stage set */
    CU_ASSERT_EQUAL(node_filter->stage, MB_COMM_OLD_PRE_TAGGING);
    CU_ASSERT_EQUAL(node_nofilter->stage, MB_COMM_OLD_PRE_TAGGING);
    CU_ASSERT_EQUAL(node_filter_fdr->stage, MB_COMM_OLD_PRE_TAGGING);
    CU_ASSERT_EQUAL(node_empty->stage, MB_COMM_OLD_PRE_TAGGING);
    CU_ASSERT_EQUAL(node_empty_filter->stage, MB_COMM_OLD_PRE_TAGGING);
    
    /* fdrFallback flag set */
    CU_ASSERT_EQUAL(node_filter->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node_nofilter->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node_filter_fdr->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node_empty_filter->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node_empty->flag_fdrFallback, MB_FALSE);
    
    /* shareOutbuf flag set */
    CU_ASSERT_EQUAL(node_filter->flag_shareOutbuf, MB_FALSE);
    CU_ASSERT_EQUAL(node_nofilter->flag_shareOutbuf, MB_FALSE);
    CU_ASSERT_EQUAL(node_filter_fdr->flag_shareOutbuf, MB_FALSE);
    CU_ASSERT_EQUAL(node_empty->flag_shareOutbuf, MB_FALSE);
    CU_ASSERT_EQUAL(node_empty_filter->flag_shareOutbuf, MB_FALSE);
    
    /* incount pointer */
    CU_ASSERT_PTR_NULL(node_filter->incount);
    CU_ASSERT_PTR_NULL(node_nofilter->incount);
    CU_ASSERT_PTR_NULL(node_filter_fdr->incount);
    CU_ASSERT_PTR_NULL(node_empty->incount);
    CU_ASSERT_PTR_NULL(node_empty_filter->incount);
    
    /* outcount pointer */
    CU_ASSERT_PTR_NULL(node_filter->outcount);
    CU_ASSERT_PTR_NULL(node_nofilter->outcount);
    CU_ASSERT_PTR_NULL(node_filter_fdr->outcount);
    CU_ASSERT_PTR_NULL(node_empty->outcount);
    CU_ASSERT_PTR_NULL(node_empty_filter->outcount);
    
    /* inbuf pointer */
    CU_ASSERT_PTR_NULL(node_filter->inbuf);
    CU_ASSERT_PTR_NULL(node_nofilter->inbuf);
    CU_ASSERT_PTR_NULL(node_filter_fdr->inbuf);
    CU_ASSERT_PTR_NULL(node_empty->inbuf);
    CU_ASSERT_PTR_NULL(node_empty_filter->inbuf);
    
    /* outbuf pointer */
    CU_ASSERT_PTR_NULL(node_filter->outbuf);
    CU_ASSERT_PTR_NULL(node_nofilter->outbuf);
    CU_ASSERT_PTR_NULL(node_filter_fdr->outbuf);
    CU_ASSERT_PTR_NULL(node_empty->outbuf);
    CU_ASSERT_PTR_NULL(node_empty_filter->outbuf);
    
    /* sendreq pointer */
    CU_ASSERT_PTR_NULL(node_filter->sendreq);
    CU_ASSERT_PTR_NULL(node_nofilter->sendreq);
    CU_ASSERT_PTR_NULL(node_filter_fdr->sendreq);
    CU_ASSERT_PTR_NULL(node_empty->sendreq);
    CU_ASSERT_PTR_NULL(node_empty_filter->sendreq);
    
    /* recvreq pointer */
    CU_ASSERT_PTR_NULL(node_filter->recvreq);
    CU_ASSERT_PTR_NULL(node_nofilter->recvreq);
    CU_ASSERT_PTR_NULL(node_filter_fdr->recvreq);
    CU_ASSERT_PTR_NULL(node_empty->recvreq);
    CU_ASSERT_PTR_NULL(node_empty_filter->recvreq);
    
    
    /* ----- run function on nodes ------------- */
    rc = MBI_CommRoutine_OLD_TagMessages(node_filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_CommRoutine_OLD_TagMessages(node_nofilter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_CommRoutine_OLD_TagMessages(node_filter_fdr);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_CommRoutine_OLD_TagMessages(node_empty);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_CommRoutine_OLD_TagMessages(node_empty_filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    
    /* ---------- Check post conditions ------------ */ 
    
    /* ready for next stage */
    CU_ASSERT_EQUAL(node_filter->stage, MB_COMM_OLD_READY_FOR_PROP);
    CU_ASSERT_EQUAL(node_nofilter->stage, MB_COMM_OLD_READY_FOR_PROP);
    CU_ASSERT_EQUAL(node_filter_fdr->stage, MB_COMM_OLD_READY_FOR_PROP);
    CU_ASSERT_EQUAL(node_empty->stage, MB_COMM_OLD_READY_FOR_PROP);
    CU_ASSERT_EQUAL(node_empty_filter->stage, MB_COMM_OLD_READY_FOR_PROP);
    
    /* cache of board object pointers are now assigned */
    CU_ASSERT_PTR_NOT_NULL(node_filter->board);
    CU_ASSERT_PTR_NOT_NULL(node_nofilter->board);
    CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->board);
    CU_ASSERT_PTR_NOT_NULL(node_empty->board);
    CU_ASSERT_PTR_NOT_NULL(node_empty_filter->board);
    
    /* outcount array allocated */
    CU_ASSERT_PTR_NOT_NULL(node_filter->outcount);
    CU_ASSERT_PTR_NOT_NULL(node_nofilter->outcount);
    CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->outcount);
    CU_ASSERT_PTR_NOT_NULL(node_empty->outcount);
    CU_ASSERT_PTR_NOT_NULL(node_empty_filter->outcount);
    
    /* node_filter should have tag table initalised */
    CU_ASSERT_EQUAL(node_filter->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node_filter->flag_shareOutbuf, MB_FALSE);
    if (node_filter->board != NULL && MBI_CommSize > 1)
    {
        CU_ASSERT_NOT_EQUAL(node_filter->board->filter, (MBIt_filterfunc)NULL);
        CU_ASSERT_PTR_NOT_NULL(node_filter->board->tt); 
    }
    
    /* node_nofilter should use shared outbuf and have no tag table */
    CU_ASSERT_EQUAL(node_nofilter->flag_fdrFallback, MB_FALSE);
    if (MBI_CommSize > 1)
    {
        CU_ASSERT_EQUAL(node_nofilter->flag_shareOutbuf, MB_TRUE);
    }
    else
    {
        CU_ASSERT_EQUAL(node_filter->flag_shareOutbuf, MB_FALSE);
    }
    if (node_nofilter->board != NULL)
    {
        CU_ASSERT_EQUAL(node_nofilter->board->filter, (MBIt_filterfunc)NULL);
        CU_ASSERT_PTR_NULL(node_nofilter->board->tt);
    }

    
    /* node_filter_fdr should use shared outbuf and have no tag table when p > 2 */
    if (MBI_CommSize > 3)
    {
        CU_ASSERT_EQUAL(node_filter_fdr->flag_fdrFallback, MB_TRUE);
        CU_ASSERT_EQUAL(node_filter_fdr->flag_shareOutbuf, MB_TRUE);
        if (node_filter_fdr->board != NULL)
        {
            CU_ASSERT_NOT_EQUAL(node_filter_fdr->board->filter, (MBIt_filterfunc)NULL);
            CU_ASSERT_PTR_NULL(node_filter_fdr->board->tt);
        }
    }
    else
    {
        CU_ASSERT_EQUAL(node_filter_fdr->flag_fdrFallback, MB_FALSE);
        CU_ASSERT_EQUAL(node_filter_fdr->flag_shareOutbuf, MB_FALSE);
        CU_ASSERT_NOT_EQUAL(node_filter_fdr->board->filter, (MBIt_filterfunc)NULL);
        if (MBI_CommSize > 1)
        {
            CU_ASSERT_PTR_NOT_NULL(node_filter_fdr->board->tt);
        }
        else
        {
            CU_ASSERT_PTR_NULL(node_filter_fdr->board->tt);
        }
    }
    
    /* node_empty should not share outbuf and have no tag table */
    CU_ASSERT_EQUAL(node_empty->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node_empty->flag_shareOutbuf, MB_FALSE);
    if (node_nofilter->board != NULL)
    {
        CU_ASSERT_EQUAL(node_empty->board->filter, (MBIt_filterfunc)NULL);
        CU_ASSERT_PTR_NULL(node_empty->board->tt);
    }
    
    /* node_empty_filter should not share outbuf and have no tag table */
    CU_ASSERT_EQUAL(node_empty_filter->flag_fdrFallback, MB_FALSE);
    CU_ASSERT_EQUAL(node_empty_filter->flag_shareOutbuf, MB_FALSE);
    if (node_nofilter->board != NULL)
    {
        CU_ASSERT_NOT_EQUAL(node_empty_filter->board->filter, (MBIt_filterfunc)NULL);
        CU_ASSERT_PTR_NULL(node_empty_filter->board->tt);
    }
}
