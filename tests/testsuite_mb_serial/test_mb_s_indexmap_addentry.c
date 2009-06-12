/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_mb_serial.h"

void test_mb_s_indexmap_addentry(void) {
    
    int i, rc;
    MBIt_IndexMap *obj;
    MBt_IndexMap im;
    
    /* adding entries to map that does not exist */
    im = -99;
    rc = MB_IndexMap_AddEntry(im, 55699);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* adding entries to null map */
    im = MB_NULL_INDEXMAP;
    rc = MB_IndexMap_AddEntry(im, 55699);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Create a map */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* get reference to map obj (so we can probe it) */
    obj = (MBIt_IndexMap *)MBI_getIndexMapRef(im);
    CU_ASSERT_PTR_NOT_NULL(obj);
    
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
    if (obj != NULL) CU_ASSERT_PTR_NOT_NULL(obj->tree);
    if (obj != NULL && obj->tree != NULL)
    {
        CU_ASSERT_EQUAL(obj->tree->count, SERIAL_TEST_MSG_COUNT);
    }
    
    /* delete the map */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}

void test_mb_s_indexmap_addentry_withdups(void) {
    
    int i, rc;
    MBIt_IndexMap *obj;
    MBt_IndexMap im;
    
    
    /* Create a map */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* get reference to map obj (so we can probe it) */
    obj = (MBIt_IndexMap *)MBI_getIndexMapRef(im);
    CU_ASSERT_PTR_NOT_NULL(obj);
    
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
    
    if (obj != NULL) CU_ASSERT_PTR_NOT_NULL(obj->tree);
    if (obj != NULL && obj->tree != NULL)
    {
        CU_ASSERT_EQUAL(obj->tree->count, SERIAL_TEST_MSG_COUNT);
    }
    
    /* delete the map */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}

void test_mb_s_indexmap_addentry_randomvals(void) {
    
    int i, rc;
    MBIt_IndexMap *obj;
    MBt_IndexMap im;
    int randomints[SERIAL_TEST_MSG_COUNT];
    
    /* Create a map */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* get reference to map obj (so we can probe it) */
    obj = (MBIt_IndexMap *)MBI_getIndexMapRef(im);
    CU_ASSERT_PTR_NOT_NULL(obj);
    
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
    
    if (obj != NULL) CU_ASSERT_PTR_NOT_NULL(obj->tree);
    if (obj != NULL && obj->tree != NULL)
    {
        CU_ASSERT_EQUAL(obj->tree->count, SERIAL_TEST_MSG_COUNT);
    }
    
    /* delete the map */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}
