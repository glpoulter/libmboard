/* $Id: test_cu_loadbuffer.c 2108 2009-09-14 14:14:31Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Sept 2009
 * 
 */
#include "header_commutils.h"

#define MSGCOUNT 100 /* must be even */
#define ALLZEROS (0x00)
#define BIT_IS_SET(octet, mask) ((octet & mask) == mask)

void test_cu_loadbuffer(void) {
    
    int i, rc, errcount;
    size_t size;
    int *msg;
    void *buffer;
    char *ptr;
    pl_address_node *pl_itr;
    MBIt_Board *board; /* mock board */
    
    board = mock_board_create();
    assert(board != NULL);
    assert(board->data->count_current == 0);
    assert(MSGCOUNT % 2 == 0);
    
    /* create dummy buffer, then populate */
    size = 1 + (MSGCOUNT * sizeof(int));
    buffer = malloc(size);
    CU_ASSERT_PTR_NOT_NULL_FATAL(buffer);
    
    ptr  = (char *)buffer;
    *ptr = ALLZEROS; /* reset header byte */
    ptr += 1;        /* now points to data section */
    
    for (i = 0; i < MSGCOUNT; i++)
    {
        memcpy((void*)ptr, (void*)&i, sizeof(int));
        ptr += sizeof(int);
    }
    
    /* Load buffer into board. MBI_COMM_HEADERBYTE_FDR is not set */
    rc = MBI_CommUtil_LoadBuffer(board, buffer, size);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->data->count_current, MSGCOUNT);
    errcount = i = 0;
    for (pl_itr = PL_ITERATOR(board->data); pl_itr; pl_itr = pl_itr->next)
    {
        msg = (int*)PL_NODEDATA(pl_itr);
        assert(msg != NULL);
        
        if (*msg != i) errcount++;
        i++;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    CU_ASSERT_EQUAL(i, MSGCOUNT);
    
    /* reset board */
    mock_board_delete(&board);
    board = mock_board_create();
    assert(board != NULL);
    assert(board->data->count_current == 0);
    assert(board->filter != NULL);
    
    /* update buffer header. set MBI_COMM_HEADERBYTE_FDR */
    ptr  = (char *)buffer;
    *ptr = ALLZEROS | MBI_COMM_HEADERBYTE_FDR;
    ptr += 1; 
    
    /* filter should now be applied to messages before storing */
    rc = MBI_CommUtil_LoadBuffer(board, buffer, size);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->data->count_current, MSGCOUNT / 2);
    errcount = i = 0;
    for (pl_itr = PL_ITERATOR(board->data); pl_itr; pl_itr = pl_itr->next)
    {
        msg = (int*)PL_NODEDATA(pl_itr);
        assert(msg != NULL);
        
        if (*msg != i*2) errcount++;
        i++;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    CU_ASSERT_EQUAL(i, MSGCOUNT / 2);
    
    /* clean up */
    free(buffer);
    mock_board_delete(&board);
    
}
