/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2008
 * 
 */
#ifndef HEADER_COMMQUEUE_H_
#define HEADER_COMMQUEUE_H_

#include "CUnit/CUnit.h"
#include "header_commroutines.h"
#include "mb_commqueue.h"
#include "mb_syncqueue.h"
#include "mb_objmap.h"
#include "mb_parallel.h"
#include <unistd.h> /* for sleep() */
#include <stdio.h>

#define TCR_MCOUNT 10
/* dummy message */
typedef struct  {
    int id;
    double value;
} tcr_msg_t;

/* filter functions to be used for testing */
int tcr_filter_even(const void *msg, const void *params);
int tcr_filter_minimum(const void *msg, const void *params);

/* ----- Source file: test_cr1.c ----- */

/* initialise test environment */
int init_cr(void);

/* clean up test environment */
int clean_cr(void);

/* testing MBIt_Comm_InitTagging() */
void test_cr_inittagging(void);

/* testing MBIt_Comm_WaitTagInfo() */
void test_cr_waittaginfo(void);

/* testing MBIt_Comm_TagMessages() */
void test_cr_tagmessages(void);

/* testing MBIt_Comm_InitPropagation() */
void test_cr_initpropagation(void);

/* testing MBIt_Comm_CompletePropagation() */
void test_cr_completepropagation(void);

#endif /*HEADER_COMMQUEUE_H_*/
