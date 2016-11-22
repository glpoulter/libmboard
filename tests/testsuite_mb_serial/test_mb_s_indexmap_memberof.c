/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_mb_serial.h"


void test_mb_s_indexmap_memberof(void) {
    
    int expected_rc;
    int i, v, rc;
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
    for (i = 0; i < SERIAL_TEST_MSG_COUNT; i++)
    {
        rc = MB_IndexMap_AddEntry(im, i);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }

    /* check random entries */
    for (i = 0; i < 2 * SERIAL_TEST_MSG_COUNT; i++)
    {
        v = rand() % (2 * SERIAL_TEST_MSG_COUNT);
        if (v < SERIAL_TEST_MSG_COUNT) expected_rc = MB_TRUE;
        else expected_rc = MB_FALSE;
        
        /* pid should not matter in serial */
        rc = MB_IndexMap_MemberOf(im, (int)rand(), v);
        if (rc != expected_rc)
        {
            CU_FAIL("MB_IndexMap_MemberOf() returned a wrong answer")
            break;
        }
    }
    
    /* delete the map */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}


void test_mb_s_indexmap_memberof_withdups(void) {
    
    int expected_rc;
    int i, v, rc;
    MBt_IndexMap im;
    
    /* Create a map */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Add entries to the map (first half) */
    for (i = 0; i < (SERIAL_TEST_MSG_COUNT / 2); i++)
    {
        rc = MB_IndexMap_AddEntry(im, i);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }
    
    /* random entries within same range */
    for (i = 0; i < (SERIAL_TEST_MSG_COUNT / 2); i++)
    {
        rc = MB_IndexMap_AddEntry(im, rand() % SERIAL_TEST_MSG_COUNT);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }
    
    /* Add entries to the map (second half) */
    for (i = (SERIAL_TEST_MSG_COUNT / 2); i < SERIAL_TEST_MSG_COUNT; i++)
    {
        rc = MB_IndexMap_AddEntry(im, i);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }

    /* check random entries */
    for (i = 0; i < 2 * SERIAL_TEST_MSG_COUNT; i++)
    {
        v = rand() % (2 * SERIAL_TEST_MSG_COUNT);
        if (v < SERIAL_TEST_MSG_COUNT) expected_rc = MB_TRUE;
        else expected_rc = MB_FALSE;
        
        /* pid should not matter in serial */
        rc = MB_IndexMap_MemberOf(im, (int)rand(), v);
        if (rc != expected_rc)
        {
            CU_FAIL("MB_IndexMap_MemberOf() returned a wrong answer")
            break;
        }
    }
    
    /* delete the map */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}

void test_mb_s_indexmap_memberof_randomvals(void) {
    
    int expected_rc;
    int i, v, rc;
    MBt_IndexMap im;
    int randomints[SERIAL_TEST_MSG_COUNT];
    
    /* Create a map */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    generate_random_unique_ints(randomints, SERIAL_TEST_MSG_COUNT);
    
    /* Add entries to the map */
    for (i = 0; i < SERIAL_TEST_MSG_COUNT; i++)
    {
        rc = MB_IndexMap_AddEntry(im, randomints[i]);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }
    /* Throw in some random duplicate ones, just because we can */
    for (i = 0; i < (SERIAL_TEST_MSG_COUNT / 2); i++)
    {
        rc = MB_IndexMap_AddEntry(im, randomints[rand() % SERIAL_TEST_MSG_COUNT]);
        if (rc != MB_SUCCESS)
        {
            CU_FAIL("MB_IndexMap_AddEntry() returned an error")
            break;
        }
    }

    /* check with random values */
    for (i = 0; i < SERIAL_TEST_MSG_COUNT; i++)
    {
        v = rand();
        if (is_in_array(randomints, SERIAL_TEST_MSG_COUNT, v)) expected_rc = MB_TRUE;
        else expected_rc = MB_FALSE;
        
        /* pid should not matter in serial */
        rc = MB_IndexMap_MemberOf(im, (int)rand(), v);
        if (rc != expected_rc)
        {
            CU_FAIL("MB_IndexMap_MemberOf() returned a wrong answer")
            break;
        }
        
    }
    
    /* check valid values */
    for (i = 0; i < SERIAL_TEST_MSG_COUNT; i++)
    {
        v = randomints[i];
        expected_rc = MB_TRUE;
        
        /* pid should not matter in serial */
        rc = MB_IndexMap_MemberOf(im, (int)rand(), v);
        if (rc != expected_rc)
        {
            CU_FAIL("MB_IndexMap_MemberOf() returned a wrong answer")
            break;
        }
    }
    
    /* delete the map */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}
