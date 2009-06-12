/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_mb_parallel.h"

void test_mb_p_indexmap_delete(void) {
    
    int rc;
    MBt_IndexMap im;

    /* deleting an invalid map */
    im = -99;
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(im, (MBt_IndexMap)-99); /* im should remain unchanged */
    
    /* deleting a null map */
    im = MB_NULL_INDEXMAP;
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP); /* im should remain unchanged */
    
    /* create a map */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL(im, MB_NULL_INDEXMAP);
    
    /* now delete it */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    
    /* name should now be re-usable for new map */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL(im, MB_NULL_INDEXMAP);
    
    /* now delete it */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
}
