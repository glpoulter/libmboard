/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : July 2008
 * 
 */

#ifndef HEADER_TAG_TABLE_H_
#define HEADER_TAG_TABLE_H_

#include "../testing.h"
#include "mb_tag_table.h"
#include <math.h>

int init_tt(void);
int clean_tt(void);

/* ===== Begin Function prototype for test cases ===== */

/* ----- Source File : test_tt1.c ----- */

/* test object creation and deletion */
void test_tt_new_destroy(void);

/* test tt_getrow() */
void test_tt_getrow(void);

/* test test_tt_setbits() */
void test_tt_setbits(void);

/* test test_tt_setbyte() */
void test_tt_setbyte(void);

/* test test_tt_getcount_row() */
void test_tt_getcount_row(void);

/* test test_tt_getcount_col() */
void test_tt_getcount_col(void);

#endif /*HEADER_TAG_TABLE_H_*/
