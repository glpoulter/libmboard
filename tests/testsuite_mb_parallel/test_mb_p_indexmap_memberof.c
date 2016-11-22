/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_mb_parallel.h"
static int _sync_arrays(int **arr, int pcount, int elemcount);
void test_mb_p_indexmap_memberof(void) {
    
    int expected_rc;
    int i, v, rc, p;
    MBt_IndexMap im;
    
    /* checking entries in map that does not exist */
    im = -99;
    rc = MB_IndexMap_MemberOf(im, 0, 55699);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* checking entries in null map */
    im = MB_NULL_INDEXMAP;
    rc = MB_IndexMap_MemberOf(im, 0, 55699);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Create a map */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Add entries to the map */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        rc = MB_IndexMap_AddEntry(im, i);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }
    
    /* check pre-sync */
    rc = MB_IndexMap_MemberOf(im, 0, 0);
    CU_ASSERT_EQUAL(rc, MB_ERR_NOTREADY);
    
    /* sync map */
    rc = MB_IndexMap_Sync(im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    
    /* check random entries */
    for (p = 0; p < MBI_CommSize; p++)
    {
        for (i = 0; i < 2 * PARALLEL_TEST_MSG_COUNT; i++)
        {
            v = rand() % (2 * PARALLEL_TEST_MSG_COUNT);
            if (v < PARALLEL_TEST_MSG_COUNT) expected_rc = MB_TRUE;
            else expected_rc = MB_FALSE;
            
            rc = MB_IndexMap_MemberOf(im, p, v);
            if (rc != expected_rc)
            {
                CU_FAIL("MB_IndexMap_MemberOf() returned a wrong answer")
                break;
            }
        }
    }
    
    /* make sure node caching does not break the code */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        for (p = 0; p < MBI_CommSize; p++)
        {
            rc = MB_IndexMap_MemberOf(im, p, i);
            CU_ASSERT_EQUAL(rc, MB_TRUE);
        }
    }
    
    /* add more values, but don't sync */
    rc = MB_IndexMap_AddEntry(im, PARALLEL_TEST_MSG_COUNT + 10);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    for (p = 0; p < MBI_CommSize; p++)
    {
        rc = MB_IndexMap_MemberOf(im, p, PARALLEL_TEST_MSG_COUNT + 10);
        CU_ASSERT_EQUAL(rc, MB_FALSE);
    }
    
    /* delete the map */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}


void test_mb_p_indexmap_memberof_nooverlaps(void) {
    
    int expected_rc, offset;
    int i, v, rc, p;
    MBt_IndexMap im;
    
    /* checking entries in map that does not exist */
    im = -99;
    rc = MB_IndexMap_MemberOf(im, 0, 55699);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* checking entries in null map */
    im = MB_NULL_INDEXMAP;
    rc = MB_IndexMap_MemberOf(im, 0, 55699);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Create a map */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Add entries to the map */
    offset = MBI_CommRank * PARALLEL_TEST_MSG_COUNT;
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        rc = MB_IndexMap_AddEntry(im, i + offset);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }
    
    /* check pre-sync */
    rc = MB_IndexMap_MemberOf(im, 0, 0);
    CU_ASSERT_EQUAL(rc, MB_ERR_NOTREADY);
    
    /* sync map */
    rc = MB_IndexMap_Sync(im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* check random entries */
    for (p = 0; p < MBI_CommSize; p++)
    {
        for (i = 0; i < 2 * PARALLEL_TEST_MSG_COUNT; i++)
        {
            v = rand() % (MBI_CommSize * PARALLEL_TEST_MSG_COUNT);
            if (v >= p * PARALLEL_TEST_MSG_COUNT &&
                v < (p+1) * PARALLEL_TEST_MSG_COUNT) expected_rc = MB_TRUE;
            else expected_rc = MB_FALSE;
            
            rc = MB_IndexMap_MemberOf(im, p, v);
            if (rc != expected_rc)
            {
                CU_FAIL("MB_IndexMap_MemberOf() returned a wrong answer")
                break;
            }
        }
    }
    
    /* delete the map */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}

void test_mb_p_indexmap_memberof_someoverlaps(void) {
    
    int expected_rc, offset;
    int i, v, rc, p;
    MBt_IndexMap im;
    
    /* checking entries in map that does not exist */
    im = -99;
    rc = MB_IndexMap_MemberOf(im, 0, 55699);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* checking entries in null map */
    im = MB_NULL_INDEXMAP;
    rc = MB_IndexMap_MemberOf(im, 0, 55699);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Create a map */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Add entries to the map */
    offset = MBI_CommRank * (PARALLEL_TEST_MSG_COUNT / 2);
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        rc = MB_IndexMap_AddEntry(im, i + offset);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }
    
    /* check pre-sync */
    rc = MB_IndexMap_MemberOf(im, 0, 0);
    CU_ASSERT_EQUAL(rc, MB_ERR_NOTREADY);
    
    /* sync map */
    rc = MB_IndexMap_Sync(im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* check random entries */
    for (p = 0; p < MBI_CommSize; p++)
    {
        for (i = 0; i < 2 * PARALLEL_TEST_MSG_COUNT; i++)
        {
            v = rand() % ((MBI_CommSize+1) * (PARALLEL_TEST_MSG_COUNT/2));
            if (v >= p*(PARALLEL_TEST_MSG_COUNT/2) &&
                v < p*(PARALLEL_TEST_MSG_COUNT/2)+PARALLEL_TEST_MSG_COUNT) 
                 expected_rc = MB_TRUE;
            else expected_rc = MB_FALSE;
            
            rc = MB_IndexMap_MemberOf(im, p, v);
            if (rc != expected_rc)
            {
                CU_FAIL("MB_IndexMap_MemberOf() returned a wrong answer")
                break;
            }
        }
    }
    
    /* delete the map */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}

void test_mb_p_indexmap_memberof_withdups(void) {
    
    int expected_rc;
    int i, v, rc, p;
    MBt_IndexMap im;
    
    /* Create a map */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Add entries to the map (first half) */
    for (i = 0; i < (PARALLEL_TEST_MSG_COUNT / 2); i++)
    {
        rc = MB_IndexMap_AddEntry(im, i);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }
    
    /* check pre-sync */
    rc = MB_IndexMap_MemberOf(im, 0, 0);
    CU_ASSERT_EQUAL(rc, MB_ERR_NOTREADY);
    
    /* sync map */
    rc = MB_IndexMap_Sync(im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* random entries within same range */
    for (i = 0; i < (PARALLEL_TEST_MSG_COUNT / 2); i++)
    {
        rc = MB_IndexMap_AddEntry(im, rand() % PARALLEL_TEST_MSG_COUNT);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }
    
    /* Add entries to the map (second half) */
    for (i = (PARALLEL_TEST_MSG_COUNT / 2); i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        rc = MB_IndexMap_AddEntry(im, i);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }
    
    /* sync map again */
    rc = MB_IndexMap_Sync(im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* check random entries */
    for (p = 0; p < MBI_CommSize; p++)
    {
        for (i = 0; i < 2 * PARALLEL_TEST_MSG_COUNT; i++)
        {
            v = rand() % (2 * PARALLEL_TEST_MSG_COUNT);
            if (v < PARALLEL_TEST_MSG_COUNT) expected_rc = MB_TRUE;
            else expected_rc = MB_FALSE;
            
            rc = MB_IndexMap_MemberOf(im, p, v);
            if (rc != expected_rc)
            {
                CU_FAIL("MB_IndexMap_MemberOf() returned a wrong answer")
                break;
            }
        }
    }
    
    /* delete the map */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}

void test_mb_p_indexmap_memberof_randomvals(void) {
    
    int expected_rc;
    int i, v, rc, p;
    MBt_IndexMap im;
    int **randomints;
    
    /* Create a map */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* allocate memory for random ints (diff on all procs) */
    randomints = (int**)malloc(sizeof(int*) * MBI_CommSize);
    CU_ASSERT_PTR_NOT_NULL_FATAL(randomints);
    for (p = 0; p < MBI_CommSize; p++)
    {
        randomints[p] = (int*)malloc(sizeof(int) * PARALLEL_TEST_MSG_COUNT);
        CU_ASSERT_PTR_NOT_NULL_FATAL(randomints[p]);
        generate_random_unique_ints(randomints[p], PARALLEL_TEST_MSG_COUNT);
    }
    
    /* Add entries to the map */
    for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
    {
        rc = MB_IndexMap_AddEntry(im, randomints[MBI_CommRank][i]);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }
    
    /* sync random array */
    rc = _sync_arrays(randomints, MBI_CommSize, PARALLEL_TEST_MSG_COUNT);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    /* sync map */
    rc = MB_IndexMap_Sync(im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Throw in some random duplicate ones, just because we can */
    for (i = 0; i < (PARALLEL_TEST_MSG_COUNT / 2); i++)
    {
        rc = MB_IndexMap_AddEntry(im, 
                randomints[MBI_CommRank][rand() % PARALLEL_TEST_MSG_COUNT]);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }

    /* sync map again */
    rc = MB_IndexMap_Sync(im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    for (p = 0; p < MBI_CommSize; p++)
    {
        /* check with random values */
        for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
        {
            v = rand();
            if (is_in_array(randomints[p], PARALLEL_TEST_MSG_COUNT, v)) expected_rc = MB_TRUE;
            else expected_rc = MB_FALSE;
            
            rc = MB_IndexMap_MemberOf(im, p, v);
            if (rc != expected_rc)
            {
                CU_FAIL("MB_IndexMap_MemberOf() returned a wrong answer")
                break;
            }
        }

        /* check valid values */
        for (i = 0; i < PARALLEL_TEST_MSG_COUNT; i++)
        {
            v = randomints[p][i];
            expected_rc = MB_TRUE;
            
            rc = MB_IndexMap_MemberOf(im, p, v);
            if (rc != expected_rc)
            {
                CU_FAIL("MB_IndexMap_MemberOf() returned a wrong answer")
                break;
            }
        }
    }
    
    for (p = 0; p < MBI_CommSize; p++) free(randomints[p]);
    free(randomints);
    
    /* delete the map */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}

static int _sync_arrays(int **arr, int pcount, int elemcount) {
    
    int i, rc;
    MPI_Request *sendreq, *recvreq;
    
    /* allocate memory for request arrays */
    sendreq = (MPI_Request*)malloc(sizeof(MPI_Request) * pcount);
    if (sendreq == NULL) return MB_ERR_MEMALLOC;
    recvreq = (MPI_Request*)malloc(sizeof(MPI_Request) * pcount);
    if (recvreq == NULL) return MB_ERR_MEMALLOC;
    
    /* issue receives */
    for (i = 0; i < pcount; i++)
    {
        if (i == MBI_CommRank)
        {
            recvreq[i] = MPI_REQUEST_NULL;
            continue;
        }
        rc = MPI_Irecv(arr[i], elemcount, MPI_INT, i, 100, 
                MPI_COMM_WORLD, &(recvreq[i]));
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
    }
    
    /* issue sends */
    for (i = 0; i < pcount; i++)
    {
        if (i == MBI_CommRank)
        {
            sendreq[i] = MPI_REQUEST_NULL;
            continue;
        }
        rc = MPI_Issend(arr[MBI_CommRank], elemcount, MPI_INT, i, 100, 
                MPI_COMM_WORLD, &(sendreq[i]));
        if (rc != MPI_SUCCESS) return MB_ERR_MPI;
    }
    
    /* wait for all receives to end */
    rc = MPI_Waitall(pcount, recvreq, MPI_STATUSES_IGNORE);
    if (rc != MPI_SUCCESS) return MB_ERR_MPI;
    
    /* wait for all sends to end */
    rc = MPI_Waitall(pcount, sendreq, MPI_STATUSES_IGNORE);
    if (rc != MPI_SUCCESS) return MB_ERR_MPI;
    
    free(sendreq);
    free(recvreq);
    
    return MB_SUCCESS;
}
