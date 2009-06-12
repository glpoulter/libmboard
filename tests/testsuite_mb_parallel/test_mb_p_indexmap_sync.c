/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_mb_parallel.h"

void test_mb_p_indexmap_sync(void) {
    
    int rc;
    MBIt_IndexMap *obj;
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
    
    /* first, make sure returned handle is registered */
    obj = (MBIt_IndexMap *)MBI_getIndexMapRef(im);
    CU_ASSERT_PTR_NOT_NULL(obj);    
    
    /* check that indexmap obj is initialised */
    if (obj != NULL)
    {
        CU_ASSERT_PTR_NOT_NULL(obj->tree_local);
        CU_ASSERT_PTR_NULL(obj->tree);
        CU_ASSERT_EQUAL(obj->synced, MB_FALSE);
        CU_ASSERT_PTR_NULL(obj->cache_leaf_ptr);
    }

    /* sync */
    ori_im = im;
    rc = MB_IndexMap_Sync(im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(im, ori_im);
    
    /* obj should now be updated */
    if (obj != NULL)
    {
        CU_ASSERT_PTR_NOT_NULL(obj->tree_local);
        CU_ASSERT_PTR_NOT_NULL(obj->tree);
        CU_ASSERT_EQUAL(obj->synced, MB_TRUE);
        CU_ASSERT_PTR_NULL(obj->cache_leaf_ptr);
        
        /* counts should still be 0 since nothing added */
        if (obj->tree_local != NULL) CU_ASSERT_EQUAL(obj->tree_local->count, 0);
        if (obj->tree != NULL) CU_ASSERT_EQUAL(obj->tree->count, 0);
    }
    
    
    /* add 2 entries, pid and pid+numprocs */
    rc = MB_IndexMap_AddEntry(im, MBI_CommRank);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_IndexMap_AddEntry(im, MBI_CommRank + MBI_CommSize);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* only local tree should change */
    if (obj != NULL)
    {
        CU_ASSERT_PTR_NOT_NULL(obj->tree_local);
        CU_ASSERT_PTR_NOT_NULL(obj->tree);
        CU_ASSERT_EQUAL(obj->synced, MB_TRUE);
        CU_ASSERT_PTR_NULL(obj->cache_leaf_ptr);
        
        /* counts should still be 0 since nothing added */
        if (obj->tree_local != NULL) CU_ASSERT_EQUAL(obj->tree_local->count, 2);
        if (obj->tree != NULL) CU_ASSERT_EQUAL(obj->tree->count, 0);
    }
    
    /* sync */
    ori_im = im;
    rc = MB_IndexMap_Sync(im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(im, ori_im);
    
    /* obj should now be updated with distributed maps */
    if (obj != NULL)
    {
        CU_ASSERT_PTR_NOT_NULL(obj->tree_local);
        CU_ASSERT_PTR_NOT_NULL(obj->tree);
        CU_ASSERT_EQUAL(obj->synced, MB_TRUE);
        CU_ASSERT_PTR_NULL(obj->cache_leaf_ptr);
        
        /* local tree should now be empty */
        if (obj->tree_local != NULL) CU_ASSERT_EQUAL(obj->tree_local->count, 0);
        /* main tree should contain leaves = 2 * num procs */
        if (obj->tree != NULL) CU_ASSERT_EQUAL(obj->tree->count, MBI_CommSize * 2);
    }
    
    /* add one more entry :  pid + 2*numprocs */
    rc = MB_IndexMap_AddEntry(im, MBI_CommRank + (2 * MBI_CommSize));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* only local tree should change */
    if (obj != NULL)
    {
        CU_ASSERT_PTR_NOT_NULL(obj->tree_local);
        CU_ASSERT_PTR_NOT_NULL(obj->tree);
        CU_ASSERT_EQUAL(obj->synced, MB_TRUE);
        CU_ASSERT_PTR_NULL(obj->cache_leaf_ptr);
        
        /* counts should still be 0 since nothing added */
        if (obj->tree_local != NULL) CU_ASSERT_EQUAL(obj->tree_local->count, 1);
        if (obj->tree != NULL) CU_ASSERT_EQUAL(obj->tree->count, MBI_CommSize * 2);
    }
    
    /* sync */
    ori_im = im;
    rc = MB_IndexMap_Sync(im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(im, ori_im);
    
    /* obj should now be updated with distributed maps */
    if (obj != NULL)
    {
        CU_ASSERT_PTR_NOT_NULL(obj->tree_local);
        CU_ASSERT_PTR_NOT_NULL(obj->tree);
        CU_ASSERT_EQUAL(obj->synced, MB_TRUE);
        CU_ASSERT_PTR_NULL(obj->cache_leaf_ptr);
        
        /* local tree should now be empty */
        if (obj->tree_local != NULL) CU_ASSERT_EQUAL(obj->tree_local->count, 0);
        /* main tree should contain leaves = 2 * num procs */
        if (obj->tree != NULL) CU_ASSERT_EQUAL(obj->tree->count, MBI_CommSize * 3);
    }
    
    /* now delete it */
    rc = MB_IndexMap_Delete(&im);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(im, MB_NULL_INDEXMAP);
    
}
