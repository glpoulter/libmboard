/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Sept 2009
 * 
 */
#include "header_mb_parallel.h"

#define SETVAL(matrix, r, c, val) matrix[(r*MBI_CommSize) + c] = val
#define PSEUDO_BARRIER(mb) MB_SyncStart(mb); MB_SyncComplete(mb)

/* Test MB_SetSyncPattern */
void test_mb_p_setsyncpattern(void) {
    
    int rc, r, c, i, next;
    int rcount, wcount;
    unsigned int dummy;
    MBt_Board mb;
    MBIt_Board *board;
    unsigned int *matrix;

    /* testing with invalid board */
    mb = 9999999;
    matrix = &dummy;
    rc = MB_SetSyncPattern(mb, matrix);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* testing with null board */
    mb = MB_NULL_MBOARD;
    rc = MB_SetSyncPattern(mb, matrix);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* create a real board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    board = (MBIt_Board*)MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);

    /* testing with null matrix */
    if (MBI_CommRank == 0)
    {
        matrix = NULL;
        rc = MB_SetSyncPattern(mb, matrix);
        CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    }
    
    /* create the matrix */
    matrix = (unsigned int*)calloc((size_t)(MBI_CommSize * MBI_CommSize), 
                                   sizeof(unsigned int));
    
    /* testing with a locked board */
    board->locked = MB_TRUE;
    rc = MB_SetSyncPattern(mb, matrix);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE); /* default, no change */
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE); /* default, no change */
    CU_ASSERT_EQUAL(board->writer_count, MBI_CommSize -1); 
    CU_ASSERT_EQUAL(board->reader_count, MBI_CommSize -1);
    board->locked = MB_FALSE;
    
    /* non-empty board */
    rc = MB_AddMessage(mb, &rc);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    rc = MB_SetSyncPattern(mb, matrix);
    CU_ASSERT_EQUAL(rc, MB_ERR_NOTREADY);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE); /* default, no change */
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE); /* default, no change */
    CU_ASSERT_EQUAL(board->writer_count, MBI_CommSize -1); 
    CU_ASSERT_EQUAL(board->reader_count, MBI_CommSize -1);
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* --- now do test for different types of matrices */
    
    /* all zeros */
    /* calloc would have already set all values to 0 */
    rc = MB_SetSyncPattern(mb, matrix);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_FALSE); 
    CU_ASSERT_EQUAL(board->is_writer, MB_FALSE); 
    CU_ASSERT_EQUAL(board->writer_count, 0); 
    CU_ASSERT_EQUAL(board->reader_count, 0);
    
    /* all zeros, except diagonal */
    for (i = 0; i < MBI_CommSize; i++) SETVAL(matrix, i, i, 1);
    rc = MB_SetSyncPattern(mb, matrix);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE); 
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE); 
    CU_ASSERT_EQUAL(board->writer_count, 0); 
    CU_ASSERT_EQUAL(board->reader_count, 0);

    /* All filled */
    for (i = 0; i < MBI_CommSize*MBI_CommSize; i++) matrix[i] = 1;
    rc = MB_SetSyncPattern(mb, matrix);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE); 
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE); 
    CU_ASSERT_EQUAL(board->writer_count, MBI_CommSize - 1); 
    CU_ASSERT_EQUAL(board->reader_count, MBI_CommSize - 1);
    for (i = 0, next = 0; i < board->writer_count; i++, next++)
    {
        if (i == MBI_CommRank) next++;
        CU_ASSERT_EQUAL(board->writer_list[i], next);
    }
    for (i = 0, next = 0; i < board->reader_count; i++, next++)
    {
        if (i == MBI_CommRank) next++;
        CU_ASSERT_EQUAL(board->reader_list[i], next);
    }
    
    /* All filled, except diagonal */
    for (i = 0; i < MBI_CommSize; i++) SETVAL(matrix, 0, 0, 1);
    rc = MB_SetSyncPattern(mb, matrix);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE); 
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE); 
    CU_ASSERT_EQUAL(board->writer_count, MBI_CommSize - 1); 
    CU_ASSERT_EQUAL(board->reader_count, MBI_CommSize - 1);
    for (i = 0, next = 0; i < board->writer_count; i++, next++)
    {
        if (i == MBI_CommRank) next++;
        CU_ASSERT_EQUAL(board->writer_list[i], next);
    }
    for (i = 0, next = 0; i < board->reader_count; i++, next++)
    {
        if (i == MBI_CommRank) next++;
        CU_ASSERT_EQUAL(board->reader_list[i], next);
    }
    
    /* checker box : even rw from odd, and vice versa */
    for (r = 0; r < MBI_CommSize; r++)
    {
        for (c = 0; c < MBI_CommSize; c++)
        {
            if ((r % 2) == (c % 2)) SETVAL(matrix, r, c, 0);
            else SETVAL(matrix, r, c, 1);
                
        }
    }
    rc = MB_SetSyncPattern(mb, matrix);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    if (MBI_CommSize > 1)
    {
        CU_ASSERT_EQUAL(board->is_reader, MB_TRUE); 
        CU_ASSERT_EQUAL(board->is_writer, MB_TRUE);
    }
    else
    {
        CU_ASSERT_EQUAL(board->is_reader, MB_FALSE); 
        CU_ASSERT_EQUAL(board->is_writer, MB_FALSE);
    }
    /* handle even/odd proc counts */
    rcount = MBI_CommSize / 2; 
    if (MBI_CommSize % 2 == 1 && MBI_CommRank % 2 == 1) rcount++;
    wcount = rcount;
    CU_ASSERT_EQUAL(board->writer_count, wcount); 
    CU_ASSERT_EQUAL(board->reader_count, rcount);
    for (i = 0; i < wcount; i++)
    {
        CU_ASSERT_EQUAL(board->writer_list[i], (i * 2) + 1 - (MBI_CommRank % 2));
    }
    for (i = 0; i < rcount; i++)
    {
        CU_ASSERT_EQUAL(board->reader_list[i], (i * 2) + 1 - (MBI_CommRank % 2));
    }
    
    /* alternate zero rows : even rows all zeros (even procs not reader) */
    for (r = 0; r < MBI_CommSize; r++)
    {
        for (c = 0; c < MBI_CommSize; c++)
        {
            if (r % 2 == 0) SETVAL(matrix, r, c, 0);
            else SETVAL(matrix, r, c, 1);
                
        }
    }
    rc = MB_SetSyncPattern(mb, matrix);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    if (MBI_CommRank % 2 == 0)
    {
        CU_ASSERT_EQUAL(board->is_reader, MB_FALSE); 
        CU_ASSERT_EQUAL(board->writer_count, 0);
    }
    else
    {
        CU_ASSERT_EQUAL(board->is_reader, MB_TRUE); 
        CU_ASSERT_EQUAL(board->writer_count, MBI_CommSize - 1);
        for (i = 0, next = 0; i < board->writer_count; i++, next++)
        {
            if (i == MBI_CommRank) next++;
            CU_ASSERT_EQUAL(board->writer_list[i], next);
        }
    }
    if (MBI_CommSize > 1)
    {
        CU_ASSERT_EQUAL(board->is_writer, MB_TRUE);
    }
    else
    {
        CU_ASSERT_EQUAL(board->is_writer, MB_FALSE);
    }
    rcount = MBI_CommSize / 2;
    if (MBI_CommRank % 2 != 0) rcount--;
    if (rcount < 0) rcount = 0;
    CU_ASSERT_EQUAL(board->reader_count, rcount); 
    for (i = 0, next = 0; i < rcount; i++)
    {
        if (1 + (i * 2) == MBI_CommRank) next = 2;
        CU_ASSERT_EQUAL(board->reader_list[i], next + 1 + (i * 2));
    }
    
    /* alternate zero cols : even columns all zeros (even procs not writers) */
    for (r = 0; r < MBI_CommSize; r++)
    {
        for (c = 0; c < MBI_CommSize; c++)
        {
            if (c % 2 == 0) SETVAL(matrix, r, c, 0);
            else SETVAL(matrix, r, c, 1);
                
        }
    }
    rc = MB_SetSyncPattern(mb, matrix);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    if (MBI_CommRank % 2 == 0)
    {
        CU_ASSERT_EQUAL(board->is_writer, MB_FALSE); 
        CU_ASSERT_EQUAL(board->reader_count, 0);
    }
    else
    {
        CU_ASSERT_EQUAL(board->is_writer, MB_TRUE); 
        CU_ASSERT_EQUAL(board->reader_count, MBI_CommSize - 1);
        for (i = 0, next = 0; i < board->reader_count; i++, next++)
        {
            if (i == MBI_CommRank) next++;
            CU_ASSERT_EQUAL(board->reader_list[i], next);
        }
    }
    if (MBI_CommSize > 1)
    {
        CU_ASSERT_EQUAL(board->is_reader, MB_TRUE);
    }
    else
    {
        CU_ASSERT_EQUAL(board->is_reader, MB_FALSE);
    }
    wcount = MBI_CommSize / 2;
    if (MBI_CommRank % 2 != 0) wcount--;
    if (wcount < 0) wcount = 0;
    CU_ASSERT_EQUAL(board->writer_count, wcount); 
    for (i = 0, next = 0; i < wcount; i++)
    {
        if (1 + (i * 2) == MBI_CommRank) next = 2;
        CU_ASSERT_EQUAL(board->writer_list[i], next + 1 + (i * 2));
    }
    
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    free(matrix);
}
