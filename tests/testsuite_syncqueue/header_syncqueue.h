/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */

#ifndef HEADER_SYNCQUEUE_H_
#define HEADER_SYNCQUEUE_H_

#include "CUnit/CUnit.h"
#include "mb_syncqueue.h"

/* ----- Source file: test_sq1.c ----- */

/* test Sync Request Queue initialisation */
void test_sq_init(void);

/* test adding sync requests */
void test_sq_push(void);

/* test popping sync requests */
void test_sq_pop(void);

#endif /*HEADER_SYNCQUEUE_H_*/
