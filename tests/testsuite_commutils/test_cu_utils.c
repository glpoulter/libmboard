/* $Id: test_cu_utils.c 2113 2009-09-15 14:42:01Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Aug 2009
 * 
 */
#include "header_commutils.h"


MBIt_Board* mock_board_create(void) {
    
    int rc;
    MBIt_Board *board;
    
    board = (MBIt_Board*)malloc(sizeof(MBIt_Board));
    assert(board != NULL);
    
    board->tt = NULL;
    board->filter = &mock_filter;
    board->locked = MB_FALSE;
    board->syncCompleted = MB_FALSE;
    board->synced_cursor = 0;

    board->reader_list = (int*)malloc(sizeof(int) * TCU_CommSize);
    assert(board->reader_list != NULL);
    board->writer_list = (int*)malloc(sizeof(int) * TCU_CommSize);
    assert(board->writer_list != NULL);
    
    mock_board_reset_accessmode(board);
    
    rc = pl_create(&(board->data), sizeof(int), 100);
    assert(rc == PL_SUCCESS);
    
    return board;
}

void mock_board_delete(MBIt_Board **board_ptr) {
    
    int rc;
    MBIt_Board *board;
    
    assert(board_ptr   != NULL);
    board = *board_ptr;
    *board_ptr = NULL;
    
    assert(board  != NULL);
    assert(board->data != NULL);
    assert(board->reader_list != NULL);
    assert(board->writer_list != NULL);
    
    rc = pl_delete(&(board->data));
    assert(rc == PL_SUCCESS);
    
    if (board->tt != NULL)
    {
        rc = tt_delete(&(board->tt));
        assert(rc == TT_SUCCESS);
        assert(board->tt == NULL);
    }
    
    free(board->reader_list);
    free(board->writer_list);
    free(board);
}

void mock_board_addmessage(MBIt_Board *board, void *msg) {
    
    int rc;
    void *ptr_new;
    pooled_list *pl;
    
    assert(board != NULL);
    assert(msg   != NULL);
    assert(board->data != NULL);
    
    pl = board->data;
    rc = pl_newnode(pl, &ptr_new);
    assert(rc == PL_SUCCESS);
    memcpy(ptr_new, msg, (size_t)pl->elem_size);
}

void mock_board_reset_accessmode(MBIt_Board *board) {
    
    int i, next;
    
    assert(board != NULL);
    assert(board->reader_list != NULL);
    assert(board->writer_list != NULL);
    
    board->is_writer = MB_TRUE;
    board->is_reader = MB_TRUE;
    
    board->reader_count = TCU_CommSize - 1;
    board->writer_count = TCU_CommSize - 1;
    
    for (i = 0, next = 0; i < TCU_CommSize - 1; i++, next++)
    {
        if (next == TCU_CommRank) next++;
        board->reader_list[i] = board->writer_list[i] = next;
    }
}

void mock_board_populate(MBIt_Board *board, int count) {
    
    int i;
    
    assert(board != NULL);
    assert(count > 0);
    
    for (i = 0; i < count; i++) mock_board_addmessage(board, &i);
}

int mock_filter (const void *msg, int pid) {
    int *val;
    
    /* acknowledge+hide "unused parameter" compiler warnings */
    ACKNOWLEDGED_UNUSED(pid);
    
    val = (int*)msg;
    if (*val % 2 == 0) return 1;
    else return 0;
}

/* 
 * this filter must give different values to different procs
 * and never fallback to FDR
 * (assuming TCU_CommSize < NumMsg/2 && NumMsg >= 1000)
 */
int mock_filter2 (const void *msg, int pid) {
    int *val;
    
    val = (int*)msg;
    if (*val == pid || *val == (pid + 1)) return 1;
    else return 0;
}
