/* $Id: test_sm_delete.c 1871 2009-06-12 13:58:18Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_string_map.h"

void test_sm_delete(void) {
    
    int rc;
    MBIt_stringmap *sm;
    
    /* delete a NULL object */
    sm = NULL;
    rc = MBI_stringmap_Delete(&sm);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    CU_ASSERT_PTR_NULL(sm);
    
    /* create a map */
    sm = MBI_stringmap_Create();
    CU_ASSERT_PTR_NOT_NULL(sm);
    if (sm != NULL) CU_ASSERT_PTR_NOT_NULL(sm->map);
    
    /* delete the map */
    rc = MBI_stringmap_Delete(&sm);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(sm);
}
