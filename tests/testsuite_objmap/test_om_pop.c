/* $Id: test_om_pop.c 2991 2013-04-12 17:38:40Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_objmap.h"

/* Test popping object from map */
void test_om_pop(void) {
    
    OM_key_t handle;
    dummy_obj *obj, *ori_obj;
    MBIt_objmap *nullmap = NULL;
    MBIt_objmap *mymap = create_om_obj();
    const double trouble = 0.1235445953;
    
    /* create obj */
    obj = (dummy_obj*)malloc(sizeof(dummy_obj));
    CU_ASSERT_PTR_NOT_NULL_FATAL(obj);
    obj->ernet = 30;
    obj->john  = 123456789;
    obj->jeopardy = trouble;
    ori_obj = obj;
    
    /* add obj to map */
    handle = MBI_objmap_push(mymap, obj);
    
    /* popping unregistered objects */
    obj = MBI_objmap_pop(mymap, 999);
    CU_ASSERT_PTR_NULL(obj);
    
    /* loose reference to object */
    obj = NULL;
    
    /* pop item from map */
    obj = MBI_objmap_pop(mymap, handle);
    
    CU_ASSERT_PTR_NOT_NULL(obj);
    CU_ASSERT_EQUAL(obj->ernet, 30);
    CU_ASSERT_EQUAL(obj->john, 123456789);
    CU_ASSERT_EQUAL(obj->jeopardy, trouble);
    
    /* object should no longer be there */
    obj = MBI_objmap_pop(mymap, handle);
    CU_ASSERT_PTR_NULL(obj);
    
    /* popping from NULL map */
    obj = MBI_objmap_pop(nullmap, handle);
    CU_ASSERT_PTR_NULL(obj);
    
    /* pop should also clear cache */
    obj = ori_obj;
    handle = MBI_objmap_push(mymap, obj);
    obj = MBI_objmap_getobj(mymap, handle);
    CU_ASSERT_PTR_EQUAL(obj, ori_obj);
    obj = MBI_objmap_pop(mymap, handle);
    CU_ASSERT_PTR_EQUAL(obj, ori_obj);
    obj = MBI_objmap_getobj(mymap, handle);
    CU_ASSERT_PTR_NULL(obj);
    
    if (ori_obj) free(ori_obj);
    
    delete_om_obj(&mymap);
}
