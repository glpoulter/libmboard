/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#ifndef HEADER_OBJMAP_H_
#define HEADER_OBJMAP_H_

#include "CUnit/CUnit.h"
#include "mb_objmap.h"
#include <stdlib.h>

/* test message */
typedef struct {
    int ernet;
    long john, silver;
    float away;
    double jeopardy;
    
} dummy_obj;

/* ==== Internal routines ===== */
/* create and text OM object */
MBIt_objmap* create_om_obj();

/* delete OM object */
void delete_om_obj(MBIt_objmap **mymap);


/* ===== Begin Function prototype for test cases ===== */

/* test object creation and deletion */
void test_om_new_destroy(void);

/* Test pushing object into map */
void test_om_push(void);

/* Test popping object from map */
void test_om_pop(void);

/* Test getting object from map */
void test_om_getobj(void);

#endif /*HEADER_OBJMAP_H_*/
