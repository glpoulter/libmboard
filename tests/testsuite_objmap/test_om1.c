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
    CU_ASSERT_PTR_NULL_FATAL(mymap->map);
    CU_ASSERT_EQUAL(mymap->top, 0);
    
    MBI_objmap_destroy(&mymap);
    CU_ASSERT_PTR_NULL(mymap);
}

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
    
    CU_ASSERT_EQUAL(handle, 0);
    CU_ASSERT_EQUAL(mymap->top, 1);
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

/* Test popping object from map */
void test_om_pop(void) {
    
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
    CU_ASSERT_EQUAL(obj->jeopardy, 0.1235445953);
    if (obj) free(obj);
    
    /* object should no longer be there */
    obj = MBI_objmap_pop(mymap, handle);
    CU_ASSERT_PTR_NULL(obj);
    
    /* popping from NULL map */
    obj = MBI_objmap_pop(nullmap, handle);
    CU_ASSERT_PTR_NULL(obj);
    
    delete_om_obj(&mymap);
}

/* Test getting object from map */
void test_om_getobj(void) {
    
    OM_key_t handle;
    int count, i;
    int errcount, errcount2, errcount3;
    dummy_obj *obj;
    MBIt_objmap *mymap = create_om_obj();
    count = 30;
    
    /* create and add objects */
    errcount = errcount2 = 0;
    for (i = 0; i < count; i++)
    {
        obj = (dummy_obj*)malloc(sizeof(dummy_obj));
        if (obj == NULL) errcount++;
        
        obj->ernet = i;
        obj->jeopardy = (double)i;
        
        handle = MBI_objmap_push(mymap, obj);
        if (handle != (OM_key_t)i) errcount2++;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    CU_ASSERT_EQUAL(errcount2, 0);
    
    /* pick off some objects for testings */
    errcount = errcount2 = errcount3 = 0;
    for (i = 0; i < count; i += 3)
    {
        obj = NULL;
        obj = MBI_objmap_getobj(mymap, (OM_key_t)i);
        if (obj == NULL) errcount++;
        if (obj->ernet != i) errcount2++;
        if (obj->jeopardy != (double)i) errcount3++;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    CU_ASSERT_EQUAL(errcount2, 0);
    CU_ASSERT_EQUAL(errcount3, 0);
    
    /* try again to make sure objects still there */
    errcount = errcount2 = errcount3 = 0;
    for (i = 0; i < count; i += 3)
    {
        obj = NULL;
        obj = MBI_objmap_getobj(mymap, (OM_key_t)i);
        if (obj == NULL) errcount++;
        if (obj->ernet != i) errcount2++;
        if (obj->jeopardy != (double)i) errcount3++;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    CU_ASSERT_EQUAL(errcount2, 0);
    CU_ASSERT_EQUAL(errcount3, 0);
    
    delete_om_obj(&mymap);
}
