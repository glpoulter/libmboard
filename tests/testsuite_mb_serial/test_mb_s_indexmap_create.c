/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_mb_serial.h"

void test_mb_s_indexmap_create(void) {
    
    int rc;
    MBIt_IndexMap *obj;
    MBt_IndexMap im, im2;
    
    /* Create a maps */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Create map with duplicate name */
    rc = MB_IndexMap_Create(&im2, "testmap");
    CU_ASSERT_EQUAL(rc, MB_ERR_DUPLICATE);
    MB_IndexMap_Delete(&im2);
    
    /* create map with name that is too long */
    rc = MB_IndexMap_Create(&im2, \
            "12345678901234567890123456789012345678901234567890" /*  50 */
            "12345678901234567890123456789012345678901234567890" /* 100 */
            "12345678901234567890123456789012345678901234567890" /* 150 */
            "12345678901234567890123456789012345678901234567890" /* 200 */
            "12345678901234567890123456789012345678901234567890" /* 250 */
            "12345678901234567890123456789012345678901234567890" /* 300 */
            );
    CU_ASSERT_EQUAL(rc, MB_ERR_OVERFLOW);
    MB_IndexMap_Delete(&im2);
    
    /* make sure returned handle is registered */
    obj = (MBIt_IndexMap *)MBI_getIndexMapRef(im);
    CU_ASSERT_PTR_NOT_NULL(obj);
    
    /* check that indexmap obj is initialised */
    if (obj != NULL)
    {
        CU_ASSERT_PTR_NOT_NULL(obj->tree);
        if (obj->tree) CU_ASSERT_EQUAL(obj->tree->count, 0);
    }
    
    /* make sure name is embedded in object */
    rc = strcmp(obj->name, "testmap");
    CU_ASSERT_EQUAL(rc, 0);
    
    /* delete the map */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);

}
