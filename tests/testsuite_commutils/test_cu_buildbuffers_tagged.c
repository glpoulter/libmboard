/* $Id: test_cu_buildbuffers_tagged.c 2113 2009-09-15 14:42:01Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Aug 2009
 * 
 */
#include "header_commutils.h"


#define MSGCOUNT 1000
#define ALLZEROS (0x00)
#define SET_ALL_NULL(buffers) for (i=0; i<TCU_CommSize;i++) buffers[i] = NULL;
#define FREE_EACH_BUFFER(buffers) \
    for (i=0; i<TCU_CommSize;i++) { \
        if (buffers[i] != NULL) {   \
            free(buffers[i]);       \
            buffers[i] = NULL;      \
        }                           \
    }
#define SWAP_INT(a,b,i) (i)=(a);(a)=(b);(b)=(i)
    
void test_cu_buildbuffers_tagged(void) {

    int i, p, rc;
    int errcount;
    int *outcount;
    int *msgptr;
    void **buffers;
    MBIt_Board *board; /* mock board */
    char *header_byte, *bufptr;
    
    /* requirements of using mock_filter2 as board filter */
    assert(TCU_CommSize < MSGCOUNT/2);
    assert(MSGCOUNT >= 1000);
    
    /* Test needs at least 4 procs */
    if (TCU_CommSize < 4) return;
    
    /* create outcount array */
    outcount = (int *)malloc(sizeof(int) * TCU_CommSize);
    assert(outcount != NULL);
    
    /* create array of buffer ptrs */
    buffers = (void **)malloc(sizeof(void*) * TCU_CommSize);
    assert(buffers != NULL);
    SET_ALL_NULL(buffers);
    
    /* --- test where all remote procs are readers --- */
    
    /* create board */
    board = mock_board_create();
    assert(board != NULL);
    assert(board->data != NULL);
    assert(board->synced_cursor == 0);
    assert(board->data->count_current == 0);
    /* different filter */
    board->filter = &mock_filter2;
    
    /* by default, all remote procs are readers */
    mock_board_populate(board, MSGCOUNT);
    assert(board->data->count_current == MSGCOUNT);
    rc = MBI_CommUtil_TagMessages(board, outcount);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board->tt);
    for (p = 0; p < TCU_CommSize; p++)
    {
        if (p == TCU_CommRank)
        {
            CU_ASSERT_EQUAL_FATAL(outcount[p], -1);
        }
        else
        {
            CU_ASSERT_EQUAL_FATAL(outcount[p], 2);
        }
    }
    
    rc = MBI_CommUtil_BuildBuffers_Tagged(board, buffers, outcount);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    for (p = 0; p < TCU_CommSize; p++)
    {
        if (p == TCU_CommRank)
        {
            CU_ASSERT_PTR_NULL(buffers[p]);
        }
        else
        {
            CU_ASSERT_PTR_NOT_NULL(buffers[p]);
            if (buffers[p] != NULL)
            {
                /* check header */
                header_byte = (char *)buffers[p];
                CU_ASSERT_EQUAL(*header_byte, ALLZEROS);
                
                /* check buffer content */
                bufptr = (char *)(buffers[p]) + 1;
                errcount = 0;
                for (i = 0; i < outcount[p]; i++)
                {
                    msgptr = (int *)bufptr;
                    bufptr += board->data->elem_size;
                    
                    if (*msgptr != p + i) errcount++;
                }
                CU_ASSERT_EQUAL(errcount, 0);
            }
        }
    }
    /* reset */
    FREE_EACH_BUFFER(buffers);
    mock_board_delete(&board);
    
    
    /* --- test where only some procs are readers --- */
    
    /* create board */
    board = mock_board_create();
    assert(board != NULL);
    assert(board->data != NULL);
    assert(board->synced_cursor == 0);
    assert(board->data->count_current == 0);
    /* different filter */
    board->filter = &mock_filter2;
    
    /* update reader_list */
    assert(TCU_CommSize > 3);
    board->reader_list[0] = (TCU_CommRank + 1) % TCU_CommSize;  
    board->reader_list[1] = (TCU_CommRank + 2) % TCU_CommSize;  
    if (board->reader_list[0] > board->reader_list[1]) 
    {
        SWAP_INT(board->reader_list[0], board->reader_list[1], i);
    }
    board->reader_count = 2;
    
    mock_board_populate(board, MSGCOUNT);
    rc = MBI_CommUtil_TagMessages(board, outcount);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board->tt);
    for (p = 0; p < TCU_CommSize; p++)
    {
        if (p == board->reader_list[0] || p == board->reader_list[1] )
        {
            CU_ASSERT_EQUAL_FATAL(outcount[p], 2);
        }
        else
        {
            CU_ASSERT_EQUAL_FATAL(outcount[p], -1);
        }
    }
    
    rc = MBI_CommUtil_BuildBuffers_Tagged(board, buffers, outcount);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    for (p = 0; p < TCU_CommSize; p++)
    {
        if (outcount[p] == -1)
        {
            CU_ASSERT_PTR_NULL(buffers[p]);
        }
        else
        {
            CU_ASSERT_PTR_NOT_NULL(buffers[p]);
            if (buffers[p] != NULL)
            {
                /* check header */
                header_byte = (char *)buffers[p];
                CU_ASSERT_EQUAL(*header_byte, ALLZEROS);
                
                /* check buffer content */
                bufptr = (char *)(buffers[p]) + 1;
                errcount = 0;
                for (i = 0; i < outcount[p]; i++)
                {
                    msgptr = (int *)bufptr;
                    bufptr += board->data->elem_size;
                    
                    if (*msgptr != p + i) errcount++;
                }
                CU_ASSERT_EQUAL(errcount, 0);
            }
        }
    }
    /* reset */
    FREE_EACH_BUFFER(buffers);
    mock_board_delete(&board);
    
    free(outcount);
    free(buffers);
}
