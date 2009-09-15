/* $Id $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_mb_parallel.h"

/* filter function to be used for testing */
static int filter_even(const void *msg, int pid);

void test_mb_p_filter_delete(void) {
    
    MBIt_filterfunc_wrapper *func;
    MBt_Filter fh, ori_fh;
    int rc;
    
    /* deleting a NULL object */
    fh = MB_NULL_FILTER;
    rc = MB_Filter_Delete(&fh);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(fh, MB_NULL_FILTER);
    
    /* deleting an invalid object */
    fh = 999999;
    rc = MB_Filter_Delete(&fh);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(fh, 999999);
    
    /* entering a NULL pointer */
    rc = MB_Filter_Delete(NULL);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);

    /* entering a an invalid handle pointer */
    rc = MB_Filter_Delete(&fh);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* Create filter */
    rc = MB_Filter_Create(&fh, &filter_even);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_NOT_EQUAL(fh, MB_NULL_FILTER);
    
    /* Query object map to ensure func ptr properly registred */
    func = (MBIt_filterfunc_wrapper*)MBI_getFilterRef(fh);
    CU_ASSERT_PTR_NOT_NULL(func);
    if (func != NULL)
    {
        CU_ASSERT_EQUAL((MBIt_filterfunc)func->func, 
                        (MBIt_filterfunc)&filter_even);
    }
    
    /* Delete the filter object */
    ori_fh = fh;
    rc = MB_Filter_Delete(&fh);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(fh, MB_NULL_FILTER);
    
    /* objects should no longer be in the objmap */
    func = (MBIt_filterfunc_wrapper*)MBI_getFilterRef(ori_fh);
    CU_ASSERT_PTR_NULL(func);
}


static int filter_even(const void *msg, int pid) {
    dummy_msg *m;
    
    /* acknowledge+hide "unused parameter" compiler warnings */
    ACKNOWLEDGED_UNUSED(pid);
    
    assert(msg != NULL);
    
    m = (dummy_msg*)msg;
    
    if (m->ernet % 2 == 0) return 1;
    else return 0;
}
