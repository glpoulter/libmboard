/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */

#include "header_mb_parallel.h"
#include <pthread.h>

static void check_board_initial_values(MBt_Board mb, size_t msgsize);
static void check_board_same_handle(MBt_Board mb);

/* Test message board creation/deletion */
void test_mb_p_create(void) {
    
    int rc;
    MBt_Board mb  = (MBt_Board)9999999;
    MBt_Board mb2 = (MBt_Board)9999999;
    
    /* invalid size */
    rc = MB_Create(&mb, 0);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
    
    /* proper size */
    rc = MB_Create(&mb, sizeof(dummy_msg));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL(mb, MB_NULL_MBOARD);
    check_board_initial_values(mb, sizeof(dummy_msg));
    
    /* proper size */
    rc = MB_Create(&mb2, sizeof(dummy_msg));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL(mb2, MB_NULL_MBOARD);
    check_board_initial_values(mb2, sizeof(dummy_msg));
    
    /* make sure they don't return the same handle */
    CU_ASSERT_NOT_EQUAL(mb, mb2);
    
    /* Make sure all nodes have same handle */
    check_board_same_handle(mb);
    check_board_same_handle(mb2);
    
    /* delete board */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
    
    /* test locked board */
    rc = MB_Delete(&mb2);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb2, MB_NULL_MBOARD);
}


static void check_board_initial_values(MBt_Board mb, size_t msgsize) {
    
    int rc;
    void *obj;
    MBIt_Board *board;
    
    /* get ptr to board object */
    obj = MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(obj);
    board = (MBIt_Board*)obj;
    
    /* check flags */
    CU_ASSERT_EQUAL(board->syncCompleted, MB_FALSE);
    CU_ASSERT_EQUAL(board->locked, MB_FALSE);
    
    /* check internal data structure */
    CU_ASSERT_PTR_NOT_NULL_FATAL(board->data);
    CU_ASSERT_EQUAL(board->data->elem_size, (unsigned int)msgsize);
    
    /* check intial values for message tagging function */
    CU_ASSERT_EQUAL(board->fh, MB_NULL_FUNCTION);
    CU_ASSERT_EQUAL(board->tagging, MB_FALSE);
    CU_ASSERT_EQUAL(board->fparams_size, 0);
    CU_ASSERT_PTR_NULL(board->fparams);
    CU_ASSERT_PTR_NULL(board->tt);
    
    /* make sure lock initialised */
    rc = pthread_mutex_lock(&(board->syncLock));
    CU_ASSERT_EQUAL(rc, 0);
    rc = pthread_mutex_unlock(&(board->syncLock));
    CU_ASSERT_EQUAL(rc, 0);
    
    /* make sure cond var initialised */
    rc = pthread_cond_signal(&(board->syncCond));
    CU_ASSERT_EQUAL(rc, 0);
}

static void check_board_same_handle(MBt_Board mb) {
    
    int i, errcount = 0;
    MBt_Board *mb_array = NULL;
    
    /* only need to do this if more than one node */
    if (testsuite_mpi_size < 2) return;
    
    if (NODE_ZERO)
    {
        mb_array = (MBt_Board *)malloc(sizeof(MBt_Board) * testsuite_mpi_size);
        assert(mb_array != NULL);
    }
    
    /* gather all mb values to masternode */
    MPI_Gather(&mb, (int)sizeof(MBt_Board), MPI_BYTE, 
               mb_array, (int)sizeof(MBt_Board), MPI_BYTE, 0, testsuite_mpi_comm);

    if (NODE_ZERO)
    {
        /* make sure handle on all nodes are the same */
        for (i = 0; i < testsuite_mpi_size; i++)
        {
            if (mb_array[i] != mb) errcount++;
        }
        CU_ASSERT_EQUAL(errcount, 0);
        
        free(mb_array);
    }
}
