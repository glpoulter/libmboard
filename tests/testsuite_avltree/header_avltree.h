/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2009
 * 
 */

#ifndef HEADER_AVLTREE_H_
#define HEADER_AVLTREE_H_

#include "CUnit/CUnit.h"
#include "mb_avltree.h"
#include <limits.h> /* for INT_MAX and INT_MIN */
#include <time.h>   /* for time() to seed RNG */
#include <stdlib.h> /* for rand() and srand() */
#include <math.h>   /* for log2() */


/* number of nodes to add when testing a tree */
/* You should use a value much smaller than RANDMAX */
#define TEST_AVL_TREESIZE 1111

#define TEST_AVL_TRUE  (1 == 1)
#define TEST_AVL_FALSE (0 == 1)

/* --- testsuite_avl_utils.c --- */
void generate_random_unique_ints(int *array, int size);
void print_node(MBIt_AVLnode *node);
void check_tree_integrity(MBIt_AVLtree *tree);

/* --- test_avl_create_delete.c --- */
/* Testing simple creation/deletion */
void test_avl_create_delete(void);

/* --- test_avl_insert.c --- */
/* Testing simple node insertion */
void test_avl_insert_simple(void);
void test_avl_insert_zigzag(void);
void test_avl_insert_duplicate(void);
void test_avl_insert_withptr(void);

/* --- test_avl_getmin_getmax.c --- */
void test_avl_getmin_getmax(void);

/* --- test_avl_getnode.c --- */
void test_avl_getnode(void);
void test_avl_getnode_invalid(void);
void test_avl_getnode_modvalue(void);

/* --- test_avl_getdata.c --- */
void test_avl_getdata(void);

/* --- test_avl_dump.c --- */
void test_avl_dump(void);

/* --- test_avl_walk.c --- */
void test_avl_walk(void);

/* --- test_avl_memberof.c --- */
void test_avl_memberof(void);

#endif /*HEADER_AVLTREE_H_*/
