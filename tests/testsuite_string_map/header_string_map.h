/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#ifndef HEADER_STRING_MAP_H_
#define HEADER_STRING_MAP_H_

#include "CUnit/CUnit.h"
#include "mb_string_map.h"
#include "mboard.h"
#include <stdlib.h>

void test_sm_create(void);
void test_sm_delete(void);
void test_sm_contains(void);
void test_sm_addstring(void);
void test_sm_removestring(void);

#endif /*HEADER_STRING_MAP_H_*/
