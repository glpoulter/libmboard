/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_mb_parallel.h"

#define SMALLNUM 10
#define SETVAL(matrix, r, c, val) matrix[(r*MBI_CommSize) + c] = val
static int _cmpfunc (const void *m1, const void *m2);
static int _filterfunc (const void *m, int pid);
static int _filterfunc2 (const void *m, int pid);
static int _filterfunc3 (const void *m, int pid);
MBt_IndexMap map;

void test_mb_p_sync_basic(void) {
    
    int i, v, rc, flag;
    MBt_Board mb;
    MBIt_Board *board;
    
    /* --- test MB_Sync_* routines with NULL board */
    mb = MB_NULL_MBOARD;
    /* syncing null board */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    /* testing null board */
    rc = MB_SyncTest(mb, &flag);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(flag, MB_TRUE);
    /* completing a null board */
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* --- test MB_Sync_* routines with invalid board */
    mb = 99999;
    /* syncing null board */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    /* testing null board */
    rc = MB_SyncTest(mb, &flag);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(flag, MB_TRUE);
    /* completing a null board */
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* --- create board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    
    /* --- synching empty board */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    /* check that board is locked */
    CU_ASSERT_EQUAL(board->locked, MB_TRUE);
    /* complete the sync */
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(board->locked, MB_FALSE);
    
    /* ---  add some messages */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        v = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) + i;
        rc = MB_AddMessage(mb, &v);
        if (rc != MB_SUCCESS) CU_FAIL("Failed to add message");
    }
    
    /* ---  sync again, this time, complete using _Test() */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    /* check that board is locked */
    CU_ASSERT_EQUAL(board->locked, MB_TRUE);
    /* complete the sync */
    flag = MB_FALSE;
    while (flag == MB_FALSE)
    {
        rc = MB_SyncTest(mb, &flag);
        if (rc != MB_SUCCESS) CU_FAIL("MB_Sync_Test() failed");
    }
    /* check that board is unlocked */
    CU_ASSERT_EQUAL(board->locked, MB_FALSE);
    
    /* --- delete board */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
}

void test_mb_p_sync_checkcontent(void) {
    
    int rc, i, v;
    MBt_Board mb;
    MBt_Iterator iter;
    void *msg;

    /* create board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* add messages */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        v = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) + i;
        rc = MB_AddMessage(mb, &v);
        if (rc != MB_SUCCESS) CU_FAIL("Failed to add message");
    }
    
    /* sync boards */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);

    /* create a sorted iterator */
    rc = MB_Iterator_CreateSorted(mb, &iter, &_cmpfunc);
    for (i = 0; i < (testsuite_mpi_size * PARALLEL_TEST_MSG_COUNT); i++)
    {
        rc = MB_Iterator_GetMessage(iter, &msg);
        if (rc != MB_SUCCESS) 
        {
            CU_FAIL("MB_Iterator_GetMessage() failed");
            break;
        }
        if (msg == NULL)
        {
            CU_FAIL("Board contains less messages than expected");
            break;
        }
        
        /* retrieve content of message */
        v = *((int*)msg);
        free(msg);
        
        /* check value */
        if (v != i)
        {
            //printf("[%d] Got %d, expected %d\n", testsuite_mpi_rank, v, i);
            CU_FAIL("Wrong value found");
            break;
        }
    }
    
    /* delete iterator */
    rc = MB_Iterator_Delete(&iter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(iter, MB_NULL_ITERATOR);
    
    /* delete board */    /* --- delete board */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
    
}

/* use filter such that only a small number of messages are tagged
 * for each board and comm routines would not fall back to 
 * full data replication
 */
void test_mb_p_sync_withfilter(void) {
    
    int min, max;
    int rc, i, v;
    MBt_Board mb;
    MBt_Filter filter;
    MBt_Iterator iter;
    void *msg;
    
    /* create board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* add messages */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        v = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) + i;
        rc = MB_AddMessage(mb, &v);
        if (rc != MB_SUCCESS) CU_FAIL("Failed to add message");
    }
    
    /* create filter */
    rc = MB_Filter_Create(&filter, &_filterfunc);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    /* assign filter */
    rc = MB_Filter_Assign(mb, filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* sync boards */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* create a sorted iterator */
    rc = MB_Iterator_CreateSorted(mb, &iter, &_cmpfunc);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    min = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) - SMALLNUM;
    max = ((testsuite_mpi_rank+1) * PARALLEL_TEST_MSG_COUNT) + SMALLNUM;
    if (min < 0) min = 0;
    if (max >= testsuite_mpi_size * PARALLEL_TEST_MSG_COUNT) 
        max = testsuite_mpi_size * PARALLEL_TEST_MSG_COUNT - 1;
    for (i = min; i <= max; i++)
    {
        rc = MB_Iterator_GetMessage(iter, &msg);
        if (rc != MB_SUCCESS) 
        {
            CU_FAIL("MB_Iterator_GetMessage() failed");
            break;
        }
        if (msg == NULL)
        {
            CU_FAIL("Board contains less messages than expected");
            break;
        }
        
        /* retrieve content of message */
        v = *((int*)msg);
        free(msg);
        
        
        
        /* check value */
        if (v != i)
        {
            CU_FAIL("Wrong value found");
            break;
        }
    }
    rc = MB_Iterator_GetMessage(iter, &msg);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(msg); /* board should now be empty */
    
    /* delete filter */
    rc = MB_Filter_Delete(&filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(filter, MB_NULL_FILTER);
    
    /* delete iterator */
    rc = MB_Iterator_Delete(&iter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(iter, MB_NULL_ITERATOR);
    
    /* delete board */    /* --- delete board */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);

}


/* use filter such that a large number of messages are tagged and the 
 * comm routines will need to fall back to full data replication
 */
void test_mb_p_sync_withfilter_fdr(void) {
    
    int min, max;
    int rc, i, v;
    int count;
    MBt_Board mb;
    MBt_Filter filter;
    MBt_Iterator iter;
    void *msg;

    /* create board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* add messages */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        v = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) + i;
        rc = MB_AddMessage(mb, &v);
        if (rc != MB_SUCCESS) CU_FAIL("Failed to add message");
    }

    /* create filter */
    rc = MB_Filter_Create(&filter, &_filterfunc2);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    /* assign filter */
    rc = MB_Filter_Assign(mb, filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);

    /* sync boards */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);

    /* create a iterator */
    rc = MB_Iterator_Create(mb, &iter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    min = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT);
    max = min + PARALLEL_TEST_MSG_COUNT;
    
    rc = MB_Iterator_GetMessage(iter, &msg);
    
    count = 0;
    while(msg != NULL)
    {
        count++;

        /* retrieve content of message */
        v = *((int*)msg);
        free(msg);
       
        /* check value */
        if ((v < min || v > max) && v%2 != 0)
        {
            CU_FAIL("Wrong value found");
            break;
        }
        
        rc = MB_Iterator_GetMessage(iter, &msg);
        if (rc != MB_SUCCESS) 
        {
            CU_FAIL("MB_Iterator_GetMessage() failed");
            break;
        }
    } 
    if (testsuite_mpi_size > 1)
    {
        CU_ASSERT_EQUAL(count, 
                PARALLEL_TEST_MSG_COUNT + 
                ((PARALLEL_TEST_MSG_COUNT/2) * (testsuite_mpi_size-1)));
    }
    
    /* delete filter */
    rc = MB_Filter_Delete(&filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(filter, MB_NULL_FILTER);
    
    /* delete iterator */
    rc = MB_Iterator_Delete(&iter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(iter, MB_NULL_ITERATOR);
    
    /* delete board */    /* --- delete board */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);

}

void test_mb_p_sync_indexmap(void) {
    
    int count;
    int min, max;
    int from, to;
    int rc, i, v;
    MBt_Board mb;
    MBt_Filter filter;
    MBt_Iterator iter;
    void *msg;
    
    /* create a map */
    rc = MB_IndexMap_Create(&map, "mymap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* add values to map */
    from = ((testsuite_mpi_rank+1) % testsuite_mpi_size) * PARALLEL_TEST_MSG_COUNT;
    to   = from + SMALLNUM;
    for (i = from; i < to; i++)
    {
        rc = MB_IndexMap_AddEntry(map, i);
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    }
    
    /* sync map */
    rc = MB_IndexMap_Sync(map);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    
    /* create board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
   
    /* add messages */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        v = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) + i;
        rc = MB_AddMessage(mb, &v);
        if (rc != MB_SUCCESS) CU_FAIL("Failed to add message");
    }
    
    /* create filter */
    rc = MB_Filter_Create(&filter, &_filterfunc3);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    /* assign filter */
    rc = MB_Filter_Assign(mb, filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* sync boards */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* create a iterator */
    rc = MB_Iterator_Create(mb, &iter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    min = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT);
    max = min + PARALLEL_TEST_MSG_COUNT;

    count = 0;
    rc = MB_Iterator_GetMessage(iter, &msg);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    while(msg != NULL)
    {
        count++;

        /* retrieve content of message */
        v = *((int*)msg);
        free(msg);
       
        /* check value */
        if ((v < min || v > max) && (v < from || v > to))
        {
            CU_FAIL("Wrong value found");
            break;
        }
        
        rc = MB_Iterator_GetMessage(iter, &msg);
        if (rc != MB_SUCCESS) 
        {
            CU_FAIL("MB_Iterator_GetMessage() failed");
            break;
        }
    } 
    if (testsuite_mpi_size > 1)
    {
        CU_ASSERT_EQUAL(count, PARALLEL_TEST_MSG_COUNT + SMALLNUM);
    }
    
    /* delete map */
    rc = MB_IndexMap_Delete(&map);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(map, MB_NULL_INDEXMAP);
    
    /* delete filter */
    rc = MB_Filter_Delete(&filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(filter, MB_NULL_FILTER);
    
    /* delete iterator */
    rc = MB_Iterator_Delete(&iter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(iter, MB_NULL_ITERATOR);
    
    /* delete board */    /* --- delete board */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
    
}

void test_mb_p_sync_accessmode(void) {
    
    int i, v, half, rc;
    MBt_Board mb;
    MBIt_Board *board;
    
    if (testsuite_mpi_size < 4) return;
    if (MBI_CONFIG.comm_protocol == MB_CONFIG_PROTO_OLD) return;
    
    /* --- create board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    
    /* first half nodes read, second half write */
    half = testsuite_mpi_size / 2;
    if (testsuite_mpi_rank < half) /* read-only */
    {
        rc = MB_SetAccessMode(mb, MB_MODE_READONLY);
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
        
        rc = MB_SyncStart(mb);
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
        rc = MB_SyncComplete(mb);
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
        
        /* check board size */
        CU_ASSERT_EQUAL((int)board->data->count_current, 
                PARALLEL_TEST_MSG_COUNT * (testsuite_mpi_size - half));
    }
    else /* write-only */
    {
        rc = MB_SetAccessMode(mb, MB_MODE_WRITEONLY);
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
        
        for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
        {
            v = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) + i;
            rc = MB_AddMessage(mb, &v);
            if (rc != MB_SUCCESS) CU_FAIL("Failed to add message");
        }
        
        rc = MB_SyncStart(mb);
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
        rc = MB_SyncComplete(mb);
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
        
        /* check board size */
        CU_ASSERT_EQUAL((int)board->data->count_current, 
                PARALLEL_TEST_MSG_COUNT);
    }
    
    /* clear messages */
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* first half nodes read-write, second half idle */
    if (testsuite_mpi_rank < half) /* read-only */
    {
        rc = MB_SetAccessMode(mb, MB_MODE_READWRITE);
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
        
        for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
        {
            v = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) + i;
            rc = MB_AddMessage(mb, &v);
            if (rc != MB_SUCCESS) CU_FAIL("Failed to add message");
        }
        
        rc = MB_SyncStart(mb);
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
        rc = MB_SyncComplete(mb);
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
        
        /* check board size */
        CU_ASSERT_EQUAL((int)board->data->count_current, 
                PARALLEL_TEST_MSG_COUNT * half);
    }
    else /* write-only */
    {
        rc = MB_SetAccessMode(mb, MB_MODE_IDLE);
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
        
        rc = MB_SyncStart(mb);
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
        rc = MB_SyncComplete(mb);
        CU_ASSERT_EQUAL(rc, MB_SUCCESS);
        
        /* check board size */
        CU_ASSERT_EQUAL((int)board->data->count_current, 0);
    }
    
    /* --- delete board */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
}

void test_mb_p_sync_pattern(void) {
    
    int count, c, c2, i, rc, v;
    unsigned int *sync_matrix;
    MBt_Board mb;
    MBIt_Board *board;
    
    if (MBI_CommSize < 4) return;
    if (MBI_CONFIG.comm_protocol == MB_CONFIG_PROTO_OLD) return;
    
    count = testsuite_mpi_size * testsuite_mpi_size;
    
    sync_matrix = (unsigned int*)calloc((size_t)count, sizeof(unsigned int));
    CU_ASSERT_PTR_NOT_NULL_FATAL(sync_matrix);
    
    /* populate matrix so board only reads message from self and next 2 
     * procs in list (cyclic)
     */
    for (i = 0; i < testsuite_mpi_size; i++)
    {
        c  = (i + 1) % testsuite_mpi_size;
        c2 = (i + 2) % testsuite_mpi_size;
        SETVAL(sync_matrix, i, i, 1);
        SETVAL(sync_matrix, i, c, 1);
        SETVAL(sync_matrix, i, c2, 1);
    }
    
    /* --- create board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    
    /* set sync pattern */
    rc = MB_SetSyncPattern(mb, sync_matrix);
    
    /* add messages to board */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        v = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) + i;
        rc = MB_AddMessage(mb, &v);
        if (rc != MB_SUCCESS) CU_FAIL("Failed to add message");
    }
    CU_ASSERT_EQUAL((int)board->data->count_current, PARALLEL_TEST_MSG_COUNT);
    
    /* sync */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* check post-sync msg count */
    CU_ASSERT_EQUAL((int)board->data->count_current, 3 * PARALLEL_TEST_MSG_COUNT);
    
    /* clean up */
    free(sync_matrix);
    
    /* --- delete board */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
}

void test_mb_p_sync_resync(void) {
    int i, v, rc;
    MBt_Board mb;
    MBIt_Board *board;
    
    /* --- create board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    
    /* ---  add some messages */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        v = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) + i;
        rc = MB_AddMessage(mb, &v);
        if (rc != MB_SUCCESS) CU_FAIL("Failed to add message");
    }
    
    /* sync board */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* check board size */
    CU_ASSERT_EQUAL((int)board->data->count_current, 
            PARALLEL_TEST_MSG_COUNT * testsuite_mpi_size);
    
    /* ---  add more messages */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        v = (PARALLEL_TEST_MSG_COUNT * testsuite_mpi_size) +
            (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) + i;
        rc = MB_AddMessage(mb, &v);
        if (rc != MB_SUCCESS) CU_FAIL("Failed to add message");
    }
    
    /* sync board */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* check board size */
    CU_ASSERT_EQUAL((int)board->data->count_current, 
            2 * PARALLEL_TEST_MSG_COUNT * testsuite_mpi_size);
    
    /* --- clear board */
    rc = MB_Clear(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL_FATAL(board->synced_cursor, 0);
    CU_ASSERT_EQUAL_FATAL(board->data->count_current, 0);
    
    /* ---  add some messages */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        v = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) + i;
        rc = MB_AddMessage(mb, &v);
        if (rc != MB_SUCCESS) CU_FAIL("Failed to add message");
    }
    
    /* sync board */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* check board size */
    CU_ASSERT_EQUAL((int)board->data->count_current, 
            PARALLEL_TEST_MSG_COUNT * testsuite_mpi_size);
    
    /* --- delete board */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
}

void test_mb_p_sync_resync_filtered(void) {
    int i, v, rc;
    MBt_Board mb;
    MBIt_Board *board;
    MBt_Filter filter;
    
    /* --- create board */
    rc = MB_Create(&mb, sizeof(int));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    
    /* create filter */
    rc = MB_Filter_Create(&filter, &_filterfunc2);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    /* assign filter */
    rc = MB_Filter_Assign(mb, filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* ---  add some messages */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        v = (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) + i;
        rc = MB_AddMessage(mb, &v);
        if (rc != MB_SUCCESS) CU_FAIL("Failed to add message");
    }
    
    /* sync board */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* check board size */
    CU_ASSERT_EQUAL((int)board->data->count_current, 
            PARALLEL_TEST_MSG_COUNT + 
            ((testsuite_mpi_size-1) * PARALLEL_TEST_MSG_COUNT / 2));
    
    /* ---  add more messages */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        v = (PARALLEL_TEST_MSG_COUNT * testsuite_mpi_size) +
            (testsuite_mpi_rank * PARALLEL_TEST_MSG_COUNT) + i;
        rc = MB_AddMessage(mb, &v);
        if (rc != MB_SUCCESS) CU_FAIL("Failed to add message");
    }
    
    /* sync board */
    rc = MB_SyncStart(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_SyncComplete(mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* check board size */
    CU_ASSERT_EQUAL((int)board->data->count_current, 
            (testsuite_mpi_size+1) * PARALLEL_TEST_MSG_COUNT );
    
    
    /* delete filter */
    rc = MB_Filter_Delete(&filter);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(filter, MB_NULL_FILTER);
    
    /* --- delete board */
    rc = MB_Delete(&mb);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
}

static int _cmpfunc (const void *m1, const void *m2) {
    
    int one = *((int*)m1);
    int two = *((int*)m2);
    
    if (one > two) return 1;
    else if (one < two) return -1;
    else return 0;
}

static int _filterfunc (const void *m, int pid) {
    
    int min, max;
    int v = *((int*)m);
    
    min = (pid * PARALLEL_TEST_MSG_COUNT) - SMALLNUM;
    max = ((pid+1) * PARALLEL_TEST_MSG_COUNT) + SMALLNUM;
   
    if (min < 0) min = 0;
    if (max >= testsuite_mpi_size * PARALLEL_TEST_MSG_COUNT) 
        max = testsuite_mpi_size * PARALLEL_TEST_MSG_COUNT - 1;
    
    if (v >= min && v <= max ) return 1;
    else return 0;
}

static int _filterfunc2 (const void *m, int pid) {
    
    int v = *((int*)m);
    
    /* acknowledge+hide "unused parameter" compiler warnings */
    ACKNOWLEDGED_UNUSED(pid);
    
    if (v % 2 == 0) return 1;
    else return 0;

}

static int _filterfunc3 (const void *m, int pid) {
    
    if (MB_IndexMap_MemberOf(map, pid, *((int*)m))) return 1;
    else return 0;
}
