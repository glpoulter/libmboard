/* $Id: test_avl_memberof.c 1871 2009-06-12 13:58:18Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2009
 * 
 */
#include "header_avltree.h"

static int _in_array(int* array, int size, int val);

void test_avl_memberof(void) {
    
    int i, errcount, v;
    int testvals[TEST_AVL_TREESIZE];
    MBIt_AVLtree *tree = NULL;
    
    tree = MBI_AVLtree_create();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tree);
    CU_ASSERT_EQUAL(tree->count, 0);
    CU_ASSERT_PTR_NULL(tree->root);
    
    /* get list of unique random ints */
    generate_random_unique_ints(testvals, TEST_AVL_TREESIZE);
    
    /* add nodes */
    for (i = 0; i < TEST_AVL_TREESIZE; i++)
    {
        MBI_AVLtree_insert(tree, testvals[i], NULL);
    }
    
    check_tree_integrity(tree);
    
    /* try checking with 2 * TEST_AVL_TREESIZE random values */
    errcount = 0;
    for (i = 0; i < TEST_AVL_TREESIZE * 2; i++)
    {
        v = rand();
        if (_in_array(testvals, TEST_AVL_TREESIZE, v) != 
            MBI_AVLtree_memberof(tree, v))  errcount++;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    
    /* check againts all actual values */
    for (i = 0; i < TEST_AVL_TREESIZE; i++)
    {
        v = testvals[i];
        if (! MBI_AVLtree_memberof(tree, v)) errcount++;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    
    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
}

static int _in_array(int* array, int size, int val) {
    int i;
    
    for (i = 0; i < size; i++)
        if (array[i] == val) return TEST_AVL_TRUE; 
    
    return TEST_AVL_FALSE; 
}
