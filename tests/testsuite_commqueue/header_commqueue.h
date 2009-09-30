/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */

#ifndef HEADER_COMMQUEUE_H_
#define HEADER_COMMQUEUE_H_

#include "../testing.h"
#include "mb_commqueue.h"
#include "mb_commroutines.h"

int init_cq(void);
int clean_cq(void);

/* ----- Source file: test_cq1.c ----- */

/* Testing Initial state */
void test_cq_init(void);

/* Testing addition of new node to CommQueue */
void test_cq_newnode(void);

/* Testing deletion of nodes from CommQueue */
void test_cq_deletenode(void);

#endif /*HEADER_COMMQUEUE_H_*/
