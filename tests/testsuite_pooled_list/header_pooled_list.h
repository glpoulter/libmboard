/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#ifndef HEADER_POOLED_LIST_H_
#define HEADER_POOLED_LIST_H_

#include "CUnit/CUnit.h"
#include "mb_pooled_list.h"
#include <math.h>

/* default memory block size */
#define ELEM_COUNT_DEFAULT 10

/* test message */
struct my_message_t {
    int ernet;
    long john, silver;
    float away;
    double jeopardy;
    double trouble;
};
typedef struct my_message_t my_message_t;

/* Bigger message to test out different size */
struct bigger_message_t {
    int id;
    double a01, a02, a03, a04, a05, a06, a07, a08, a09;
    double a11, a12, a13, a14, a15, a16, a17, a18, a19;
    double a21, a22, a23, a24, a25, a26, a27, a28, a29;
    
    struct bigger_message_t *next;
};
typedef struct bigger_message_t bigger_message_t;

/* ----- Utilities in testsuite_pl_utils.c ----- */
pooled_list* create_and_populate_pl(int memblock_size, int node_count);
void destroy_pl_object(pooled_list **pl);

/* ===== Begin Function prototype for test cases ===== */

/* ----- Source File : test_pl1.c ----- */

/* test object creation and deletion */
void test_pl_create_delete(void);

/* test using different message sizes */
void test_pl_multisize(void);

/* test using invalid input */
void test_pl_create_invalid(void);

/* ----- Source File: test_pl2.c ----- */

/* test adding nodes */
void test_pl_addnodes(void);

/* test using invalid input */
void test_pl_addnode_invalid(void);

/* test memblock growth */
void test_pl_memblocks(void);

/* ----- Source File: test_pl3.c ----- */

/* test linked list traversal */
void test_pl_traverse(void);

/* test linked list traversal across multiple memblock */
void test_pl_traverse2(void);

/* test getting node by index */
void test_pl_getnode(void);

/* ----- Source File: test_pl4.c ----- */
/* test pl_recycle() */
void test_pl_recycle(void);

/* test pl_reset() */
void test_pl_reset(void);


#endif /*HEADER_POOLED_LIST_H_*/
