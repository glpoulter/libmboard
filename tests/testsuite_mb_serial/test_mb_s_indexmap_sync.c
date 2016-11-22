/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_mb_serial.h"

void test_mb_s_indexmap_sync(void) {

    int rc;
    MBt_IndexMap im, ori_im;
    
    /* syncing an invalid map */
    ori_im = im = -99;
    rc = MB_IndexMap_Sync(im);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(im, ori_im);
    
    /* syncing a null map */
    ori_im = im = MB_NULL_INDEXMAP;
    rc = MB_IndexMap_Sync(im);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(im, ori_im);
    
    /* create a map */
    rc = MB_IndexMap_Create(&im, "testmap");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL(im, MB_NULL_INDEXMAP);
    
    /* sync */
    ori_im = im;
    rc = MB_IndexMap_Sync(im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(im, ori_im);
    
    /* now delete it */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
}
