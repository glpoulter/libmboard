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

void test_mb_p_filter_create(void) {
    
    MBIt_filterfunc_wrapper *func;
    MBt_Filter fh;
    int rc;
    
    /* using a NULL function pointer */
    rc = MB_Filter_Create(&fh, NULL);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_EQUAL(fh, MB_NULL_FILTER);
    
    /* use an actual function */
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
    rc = MB_Filter_Delete(&fh);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(fh, MB_NULL_FILTER);
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
