/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Aug 2009
 * 
 */
#include "header_commutils.h"

#define RESET(outcount) for(i=0;i<TCU_CommSize;i++) outcount[i]=0;
#define MSGCOUNT 200 /* must be divisible by 8 */

void test_cu_tagmessages(void) {
    
    int rc, i, next, count;
    int *outcount;
    MBIt_Board *board; /* mock board */
    
    board = mock_board_create();
    assert(board != NULL);
    outcount = (int*)malloc(sizeof(int) * TCU_CommSize);
    assert(outcount != NULL);
    RESET(outcount);
    
    /* when board is not a writer */
    board->is_writer = MB_FALSE;
    board->reader_count = 0;
    rc = MBI_CommUtil_TagMessages(board, outcount);
    CU_ASSERT_PTR_NULL(board->tt);
    for (i = 0; i < TCU_CommSize; i++)
    {
        CU_ASSERT_EQUAL(outcount[i], -1);
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    mock_board_reset_accessmode(board);
    RESET(outcount);
    
    /* no readers */
    board->reader_count = 0;
    rc = MBI_CommUtil_TagMessages(board, outcount);
    CU_ASSERT_PTR_NULL(board->tt);
    for (i = 0; i < TCU_CommSize; i++)
    {
        CU_ASSERT_EQUAL(outcount[i], -1);
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    mock_board_reset_accessmode(board);
    RESET(outcount);
    
    /* empty board */
    rc = MBI_CommUtil_TagMessages(board, outcount);
    CU_ASSERT_PTR_NULL(board->tt);
    for (i = 0; i < TCU_CommSize; i++)
    {
        if ( i == TCU_CommRank)
        {
            CU_ASSERT_EQUAL(outcount[i], -1);
        }
        else
        {
            CU_ASSERT_EQUAL(outcount[i], 0);
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    mock_board_reset_accessmode(board);
    RESET(outcount);
    
    /* empty board with some non readers */
    next = 0;
    board->reader_count = 0;
    for (i = 0; i < TCU_CommSize; i++) 
    {
        if (i == TCU_CommRank) continue;
        if (i % 2 == 0)
        {
            board->reader_list[next++] = i;
            board->reader_count++;
        }
    }
    rc = MBI_CommUtil_TagMessages(board, outcount);
    CU_ASSERT_PTR_NULL(board->tt);
    for (i = 0; i < TCU_CommSize; i++)
    {
        if ( i == TCU_CommRank || i % 2 != 0)
        {
            CU_ASSERT_EQUAL(outcount[i], -1);
        }
        else
        {
            CU_ASSERT_EQUAL(outcount[i], 0);
        }
    }
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    mock_board_reset_accessmode(board);
    RESET(outcount);
    
    /* populate board and test with filtering */
    /* if TCU_CommSize > 3, we will fallback to full data replication */
    /* test should therefore ideally be run for at lest {1,4} procs */
    assert(MSGCOUNT % 8 == 0);
    mock_board_populate(board, MSGCOUNT);
    CU_ASSERT_EQUAL(board->data->count_current, MSGCOUNT);
    if (TCU_CommSize > 3) /* fallback to FDR */
    {
        rc = MBI_CommUtil_TagMessages(board, outcount);
        CU_ASSERT_PTR_NULL(board->tt);
        for (i = 0; i < TCU_CommSize; i++)
        {
            if ( i == TCU_CommRank)
            {
                CU_ASSERT_EQUAL(outcount[i], -1);
            }
            else
            {
                CU_ASSERT_EQUAL(outcount[i], MSGCOUNT);
            }
        }
        CU_ASSERT_EQUAL(rc, MB_SUCCESS_2);
    }
    else
    {
        rc = MBI_CommUtil_TagMessages(board, outcount);
        if (TCU_CommSize > 1)
        {
            CU_ASSERT_PTR_NOT_NULL(board->tt);
        }
        for (i = 0; i < TCU_CommSize; i++)
        {
            if ( i == TCU_CommRank)
            {
                CU_ASSERT_EQUAL(outcount[i], -1);
                if (board->tt != NULL)
                {
                    rc = tt_getcount_col(board->tt, i, &count);
                    CU_ASSERT_EQUAL(rc, TT_SUCCESS);
                    CU_ASSERT_EQUAL(count, 0);
                }
            }
            else
            {
                CU_ASSERT_EQUAL(outcount[i], MSGCOUNT / 2);
                if (board->tt != NULL)
                {
                    rc = tt_getcount_col(board->tt, i, &count);
                    CU_ASSERT_EQUAL(rc, TT_SUCCESS);
                    CU_ASSERT_EQUAL(count, MSGCOUNT / 2);
                }
            }
        }
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    }
    mock_board_delete(&board);
    board = mock_board_create();
    
    /* populate board and test with filtering. synced_cursor moved 
     * forward 1/4 across board content */
    /* if TCU_CommSize > 3, we will fallback to full data replication */
    /* test should therefore ideally be run for at lest {1,4} procs */
    assert(MSGCOUNT % 8 == 0);
    mock_board_populate(board, MSGCOUNT);
    board->synced_cursor = MSGCOUNT / 4;
    CU_ASSERT_EQUAL(board->data->count_current, MSGCOUNT);
    if (TCU_CommSize > 3) /* fallback to FDR */
    {
        rc = MBI_CommUtil_TagMessages(board, outcount);
        CU_ASSERT_PTR_NULL(board->tt);
        for (i = 0; i < TCU_CommSize; i++)
        {
            if ( i == TCU_CommRank)
            {
                CU_ASSERT_EQUAL(outcount[i], -1);
            }
            else
            {
                CU_ASSERT_EQUAL(outcount[i], (MSGCOUNT / 4) * 3);
            }
        }
        CU_ASSERT_EQUAL(rc, MB_SUCCESS_2);
    }
    else
    {
        rc = MBI_CommUtil_TagMessages(board, outcount);
        if (TCU_CommSize > 1)
        {
            CU_ASSERT_PTR_NOT_NULL(board->tt);
        }
        for (i = 0; i < TCU_CommSize; i++)
        {
            if ( i == TCU_CommRank)
            {
                CU_ASSERT_EQUAL(outcount[i], -1);
                if (board->tt != NULL)
                {
                    rc = tt_getcount_col(board->tt, i, &count);
                    CU_ASSERT_EQUAL(rc, TT_SUCCESS);
                    CU_ASSERT_EQUAL(count, 0);
                }
            }
            else
            {
                if (outcount[i] != (MSGCOUNT/8) * 3)
                    printf("\n***** %d != %d\n", outcount[i],(MSGCOUNT/8) * 3);
                CU_ASSERT_EQUAL(outcount[i], (MSGCOUNT/8) * 3);
                if (board->tt != NULL)
                {
                    rc = tt_getcount_col(board->tt, i, &count);
                    CU_ASSERT_EQUAL(rc, TT_SUCCESS);
                    CU_ASSERT_EQUAL(count, (MSGCOUNT/8) * 3);
                }
            }
        }
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    }
    
    /* reset board */
    mock_board_delete(&board);
    board = mock_board_create();
    board->filter = NULL; /* remove filter */
    mock_board_populate(board, MSGCOUNT);
    rc = MBI_CommUtil_TagMessages(board, outcount);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(board->tt);
    for (i = 0; i < TCU_CommSize; i++)
    {
        if (i == TCU_CommRank)
        {
            CU_ASSERT_EQUAL(outcount[i], -1);
        }
        else
        {
            CU_ASSERT_EQUAL(outcount[i], MSGCOUNT);
        }
    }
    
    /* try with synced_cursor set */
    board->synced_cursor = (MSGCOUNT / 4) * 3;
    rc = MBI_CommUtil_TagMessages(board, outcount);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(board->tt);
    for (i = 0; i < TCU_CommSize; i++)
    {
        if (i == TCU_CommRank)
        {
            CU_ASSERT_EQUAL(outcount[i], -1);
        }
        else
        {
            CU_ASSERT_EQUAL(outcount[i], MSGCOUNT / 4);
        }
    }
    
    /* clean up */
    free(outcount);
    mock_board_delete(&board);
    assert(board == NULL);
}

