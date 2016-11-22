/* $Id: test_om_getobj.c 2927 2012-07-20 15:33:14Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_objmap.h"

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
    for (i = 1; i <= count; i++)
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
    for (i = 1; i <= count; i += 3)
    {
        obj = NULL;
        obj = MBI_objmap_getobj(mymap, (OM_key_t)i);
        if (obj == NULL) { errcount++; continue; }
        if (obj->ernet != i) errcount2++;
        if (obj->jeopardy != (double)i) errcount3++;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    CU_ASSERT_EQUAL(errcount2, 0);
    CU_ASSERT_EQUAL(errcount3, 0);
    
    /* try again to make sure objects still there */
    errcount = errcount2 = errcount3 = 0;
    for (i = 1; i <= count; i += 3)
    {
        obj = NULL;
        obj = MBI_objmap_getobj(mymap, (OM_key_t)i);
        if (obj == NULL) { errcount++; continue; }
        if (obj->ernet != i) errcount2++;
        if (obj->jeopardy != (double)i) errcount3++;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    CU_ASSERT_EQUAL(errcount2, 0);
    CU_ASSERT_EQUAL(errcount3, 0);
    
    delete_om_obj(&mymap);
}
