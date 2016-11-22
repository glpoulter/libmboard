/* $Id: test_mb_p_setaccessmode.c 2128 2009-09-17 14:17:21Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Aug 2009
 * 
 */

#include "header_mb_parallel.h"

#define PSEUDO_BARRIER(mbx) MB_SyncStart(mbx); MB_SyncComplete(mbx)

/* Test MB_SetAccessMode */
void test_mb_p_setaccessmode(void) {
    
    int rc, i, next, half;
    MBt_Board mb;
    MBIt_Board *board;
    
    /* testing with invalid board */
    mb = 9999999;
    rc = MB_SetAccessMode(mb, MB_MODE_READWRITE);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* testing with null board */
    mb = MB_NULL_MBOARD;
    rc = MB_SetAccessMode(mb, MB_MODE_READWRITE);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
   
    /* create a real board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    board = (MBIt_Board*)MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    
    /* testing with an invalid mode */
    rc = MB_SetAccessMode(mb, 10000);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* locked board */
    board->locked = MB_TRUE;
    rc = MB_SetAccessMode(mb, MB_MODE_IDLE);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE); /* default */
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE); /* default */
    board->locked = MB_FALSE;
    
    /* non-empty board */
    rc = MB_AddMessage(mb, &rc);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_SetAccessMode(mb, MB_MODE_IDLE);
    CU_ASSERT_EQUAL(rc, MB_ERR_NOTREADY);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE); /* default */
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE); /* default */
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    
    /* --- cycle through different modes --- */
    /* some tests repeated to make sure both flags are toggled */
    rc = MB_SetAccessMode(mb, MB_MODE_IDLE);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_FALSE);
    CU_ASSERT_EQUAL(board->is_writer, MB_FALSE);
    CU_ASSERT_EQUAL(board->reader_count, 0);
    CU_ASSERT_EQUAL(board->writer_count, 0);

    rc = MB_SetAccessMode(mb, MB_MODE_READWRITE);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE);
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE);
    CU_ASSERT_EQUAL(board->reader_count, MBI_CommSize-1);
    CU_ASSERT_EQUAL(board->writer_count, MBI_CommSize-1);
    if (board->reader_list != NULL && board->writer_list != NULL)
    {
        for (i = 0, next = 0; i < MBI_CommSize-1; i++, next++)
        {
            if (i == MBI_CommRank) next++;
            CU_ASSERT_EQUAL(board->reader_list[i], next);
            CU_ASSERT_EQUAL(board->writer_list[i], next);
        }
    }
    
    rc = MB_SetAccessMode(mb, MB_MODE_READONLY);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE);
    CU_ASSERT_EQUAL(board->is_writer, MB_FALSE);
    CU_ASSERT_EQUAL(board->reader_count, 0);
    CU_ASSERT_EQUAL(board->writer_count, 0);
    if (board->reader_list != NULL && board->writer_list != NULL)
    {
        for (i = 0, next = 0; i < MBI_CommSize-1; i++, next++)
        {
            if (i == MBI_CommRank) next++;
            CU_ASSERT_EQUAL(board->reader_list[i], next);
        }
    }
    
    rc = MB_SetAccessMode(mb, MB_MODE_WRITEONLY);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_FALSE);
    CU_ASSERT_EQUAL(board->is_writer, MB_TRUE);
    CU_ASSERT_EQUAL(board->reader_count, 0);
    CU_ASSERT_EQUAL(board->writer_count, 0);
    if (board->reader_list != NULL && board->writer_list != NULL)
    {
        for (i = 0, next = 0; i < MBI_CommSize-1; i++, next++)
        {
            if (i == MBI_CommRank) next++;
            CU_ASSERT_EQUAL(board->writer_list[i], next);
        }
    }
    
    rc = MB_SetAccessMode(mb, MB_MODE_READONLY); /* repeat */
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->is_reader, MB_TRUE);
    CU_ASSERT_EQUAL(board->is_writer, MB_FALSE);
    CU_ASSERT_EQUAL(board->reader_count, 0);
    CU_ASSERT_EQUAL(board->writer_count, 0);
    if (board->reader_list != NULL && board->writer_list != NULL)
    {
        for (i = 0, next = 0; i < MBI_CommSize-1; i++, next++)
        {
            if (i == MBI_CommRank) next++;
            CU_ASSERT_EQUAL(board->reader_list[i], next);
        }
    }
    
    /* even procs are readers, odd procs are writers */
    half = MBI_CommSize / 2;
    if (MBI_CommSize > 1)
    {
        if (MBI_CommRank % 2 == 0) /* even ranks */
        {
            rc = MB_SetAccessMode(mb, MB_MODE_READONLY);
            CU_ASSERT_EQUAL(rc, MB_SUCCESS);
            CU_ASSERT_EQUAL(board->is_reader, MB_TRUE);
            CU_ASSERT_EQUAL(board->is_writer, MB_FALSE);
            CU_ASSERT_EQUAL(board->reader_count, 0);
            CU_ASSERT_EQUAL(board->writer_count, half);
            if (board->reader_list != NULL && board->writer_list != NULL)
            {
                next = 0;
                for (i = 0; i < half; i++)
                {
                    CU_ASSERT_EQUAL(board->writer_list[i], (i*2)+1);
                }
            }

        }
        else /* odd ranks */
        {
            rc = MB_SetAccessMode(mb, MB_MODE_WRITEONLY); /* repeat */
            CU_ASSERT_EQUAL(rc, MB_SUCCESS);
            CU_ASSERT_EQUAL(board->is_reader, MB_FALSE);
            CU_ASSERT_EQUAL(board->is_writer, MB_TRUE);
            if (MBI_CommSize % 2 != 0) half += 1;/* odd proc count */
            CU_ASSERT_EQUAL(board->reader_count, half);
            CU_ASSERT_EQUAL(board->writer_count, 0);
            if (board->reader_list != NULL && board->writer_list != NULL)
            {
                for (i = 0; i < half; i++)
                {
                    CU_ASSERT_EQUAL(board->reader_list[i], i*2);
                }
            }
        }
    }
    
    
    /* even procs RW, odd procs IDLE */
    if (MBI_CommSize > 1)
    {
        if (MBI_CommRank % 2 == 0) /* even ranks */
        {
            rc = MB_SetAccessMode(mb, MB_MODE_READWRITE); 
            CU_ASSERT_EQUAL(rc, MB_SUCCESS);
            CU_ASSERT_EQUAL(board->is_reader, MB_TRUE);
            CU_ASSERT_EQUAL(board->is_writer, MB_TRUE);
            if (MBI_CommSize % 2 != 0) half += 1;/* odd proc count */
            CU_ASSERT_EQUAL(board->reader_count, half - 1);
            CU_ASSERT_EQUAL(board->writer_count, half - 1);
            if (board->reader_list != NULL && board->writer_list != NULL)
            {
                next = 0;
                for (i = 0; i < half - 1; i++)
                {
                    if (i*2 >= MBI_CommRank) next = 1; /* skip self */ 
                    CU_ASSERT_EQUAL(board->reader_list[i], (i+next)*2);
                    CU_ASSERT_EQUAL(board->writer_list[i], (i+next)*2);
                }
            }

        }
        else /* odd ranks */
        {
            rc = MB_SetAccessMode(mb, MB_MODE_IDLE); 
            CU_ASSERT_EQUAL(rc, MB_SUCCESS);
            CU_ASSERT_EQUAL(board->is_reader, MB_FALSE);
            CU_ASSERT_EQUAL(board->is_writer, MB_FALSE);
            CU_ASSERT_EQUAL(board->reader_count, 0);
            CU_ASSERT_EQUAL(board->writer_count, 0);
        }
    }
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
}
