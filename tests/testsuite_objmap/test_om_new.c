/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_objmap.h"

/* test object creation and deletion */
void test_om_new_destroy(void) {
    
    MBIt_objmap *mymap = NULL;
    
    mymap = MBI_objmap_new();
    
    CU_ASSERT_PTR_NOT_NULL_FATAL(mymap);
    /* CU_ASSERT_PTR_NULL_FATAL(mymap->map); */
    CU_ASSERT_EQUAL(mymap->top, 0);
    
    MBI_objmap_destroy(&mymap);
    CU_ASSERT_PTR_NULL(mymap);
}

