/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_objmap.h"

/* Test pushing object into map */
void test_om_push(void) {
    OM_key_t handle;
    dummy_obj *obj;
    MBIt_objmap *nullmap = NULL;
    MBIt_objmap *mymap = create_om_obj();
    
    /* create obj */
    obj = (dummy_obj*)malloc(sizeof(dummy_obj));
    CU_ASSERT_PTR_NOT_NULL_FATAL(obj);
    obj->ernet = 30;
    obj->john  = 123456789;
    obj->jeopardy = 0.1235445953;
    
    
    /* add obj to map */
    handle = MBI_objmap_push(mymap, obj);
    
    CU_ASSERT_EQUAL(handle, 1);
    CU_ASSERT_EQUAL(mymap->top, 2);
    CU_ASSERT_PTR_NOT_NULL(mymap->map);
    
    /* adding to NULL map */
    handle = MBI_objmap_push(nullmap, obj);
    CU_ASSERT_EQUAL(handle, OM_ERR_INVALID);
    
    /* adding NULL obj */
    obj = NULL;
    handle = MBI_objmap_push(mymap, obj);
    CU_ASSERT_EQUAL(handle, OM_ERR_INVALID);
    
    delete_om_obj(&mymap);
    
}

