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
#include "mb_parallel.h"
#include "mboard.h"

#define SMALLNUM 10
/* must be an even number! */
#define TEST_MESSAGE_COUNT (int)(MB_CONFIG_PARALLEL_POOLSIZE * 2 + SMALLNUM)

/* global vars */
extern MBt_Board mb_nofilter, mb_filter, mb_filter_fdr;
extern MBt_Board mb_empty, mb_empty_filter;
extern MBt_Filter filter_map, filter_fdr;
extern MBt_IndexMap indexmap;

extern struct MBIt_commqueue *node_nofilter;
extern struct MBIt_commqueue *node_filter;
extern struct MBIt_commqueue *node_filter_fdr;
extern struct MBIt_commqueue *node_empty;
extern struct MBIt_commqueue *node_empty_filter;

/* ... in test_cr_utils.c */
int filter_func_map(const void *m, int pid);
int filter_func_fdr(const void *m, int pid);
int _initialise_map_values(MBt_IndexMap map);

/* setup and teardown routines */
int init_cr(void);
int clean_cr(void);

/* test routines */
void test_cr_tagmessages(void);
void test_cr_sendbufinfo(void);
void test_cr_waitbufinfo(void);
void test_cr_initpropagation(void);
void test_cr_completepropagation(void);

#endif /*HEADER_COMMQUEUE_H_*/
