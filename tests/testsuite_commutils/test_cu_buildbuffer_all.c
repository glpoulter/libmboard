/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Aug 2009
 * 
 */
#include "header_commutils.h"

#define UPDATE_CURSOR(board) (board->synced_cursor = board->data->count_current)
#define ALLZEROS (0x00)
#define BIT_IS_SET(octet, mask) ((octet & mask) == mask)

void test_cu_buildbuffer_all(void) {
    
    int i, rc, errcount;
    int *msgptr;
    size_t size;
    void *buffer;
    MBIt_Board *board; /* mock board */
    char *header_byte, *bufptr;
    
    board = mock_board_create();
    assert(board != NULL);
    assert(board->data != NULL);
    assert(board->synced_cursor == 0);
    assert(board->data->count_current == 0);
    
    /* first sync */
    buffer = NULL;
    UPDATE_CURSOR(board);
    mock_board_populate(board, 100);
    rc = MBI_CommUtil_BuildBuffer_All(board, &buffer, MB_FALSE, &size);
    CU_ASSERT_PTR_NOT_NULL(buffer);
    CU_ASSERT_EQUAL(board->synced_cursor, 0);
    CU_ASSERT_EQUAL(board->data->count_current, 100);
    CU_ASSERT_EQUAL((int)size, 1 + (100 * (int)board->data->elem_size));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    if (buffer != NULL)
    {
        /* check header */
        header_byte = (char *)buffer;
        CU_ASSERT_EQUAL(*header_byte, ALLZEROS);
        
        /* check buffer content */
        bufptr = (char *)buffer + 1;
        errcount = 0;
        for (i = 0; i < 100; i++)
        {
            msgptr = (int *)bufptr;
            bufptr += board->data->elem_size;
            
            if (*msgptr != i) errcount++;
        }
        CU_ASSERT_EQUAL(errcount, 0);
        
        /* clean up */
        free(buffer);
    }
    
    /* second sync */
    buffer = NULL;
    UPDATE_CURSOR(board);
    mock_board_populate(board, 75);
    rc = MBI_CommUtil_BuildBuffer_All(board, &buffer, MB_FALSE, &size);
    CU_ASSERT_PTR_NOT_NULL(buffer);
    CU_ASSERT_EQUAL(board->synced_cursor, 100);
    CU_ASSERT_EQUAL(board->data->count_current, 175);
    CU_ASSERT_EQUAL((int)size, 1 + (75 * (int)board->data->elem_size));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    if (buffer != NULL)
    {
        /* check header */
        header_byte = (char *)buffer;
        CU_ASSERT_EQUAL(*header_byte, ALLZEROS);
        
        /* check buffer content */
        bufptr = (char *)buffer + 1;
        errcount = 0;
        for (i = 0; i < 75; i++)
        {
            msgptr = (int *)bufptr;
            bufptr += board->data->elem_size;
            
            if (*msgptr != i) errcount++;
        }

        CU_ASSERT_EQUAL(errcount, 0);
        
        /* clean up */
        free(buffer);
    }

    /* third sync, with fdr */
    buffer = NULL;
    UPDATE_CURSOR(board);
    mock_board_populate(board, 25);
    rc = MBI_CommUtil_BuildBuffer_All(board, &buffer, MB_TRUE, &size);
    CU_ASSERT_PTR_NOT_NULL(buffer);
    CU_ASSERT_EQUAL(board->synced_cursor, 175);
    CU_ASSERT_EQUAL(board->data->count_current, 200);
    CU_ASSERT_EQUAL((int)size, 1 + (25 * (int)board->data->elem_size));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    if (buffer != NULL)
    {
        /* check header */
        header_byte = (char *)buffer;
        CU_ASSERT(BIT_IS_SET(*header_byte, MBI_COMM_HEADERBYTE_FDR));
        
        /* check buffer content */
        bufptr = (char *)buffer + 1;
        errcount = 0;
        for (i = 0; i < 25; i++)
        {
            msgptr = (int *)bufptr;
            bufptr += board->data->elem_size;
            
            if (*msgptr != i) errcount++;
        }
        CU_ASSERT_EQUAL(errcount, 0);
        
        /* clean up */
        free(buffer);
    }

    /* clean up */
    mock_board_delete(&board);
    assert(board == NULL);
}
