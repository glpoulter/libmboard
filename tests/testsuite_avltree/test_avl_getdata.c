/* $Id: test_avl_getdata.c 1871 2009-06-12 13:58:18Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2009
 * 
 */
#include "header_avltree.h"

void test_avl_getdata(void) {
    int i, errcount, v;
    void *data;
    int testvals[TEST_AVL_TREESIZE];
    MBIt_AVLtree *tree = NULL;
    
    /* test NULL tree */
    data = MBI_AVLtree_getdata(tree, 0);
    CU_ASSERT_PTR_NULL(data);
    
    /* create tree */
    tree = MBI_AVLtree_create();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tree);
    CU_ASSERT_EQUAL(tree->count, 0);
    CU_ASSERT_PTR_NULL(tree->root);
    
    /* test empty tree */
    data = MBI_AVLtree_getdata(tree, 0);
    CU_ASSERT_PTR_NULL(data);
    
    /* add nodes */
    for (i = 0; i < TEST_AVL_TREESIZE; i++) 
    { 
        testvals[i] = i * 10; /* remember values used */
        MBI_AVLtree_insert(tree, i, &testvals[i]);
    }

    
    /* use getnode to retrieve node pointer and inspect key value */
    errcount = 0;
    for (i = 0; i < TEST_AVL_TREESIZE; i++) 
    { 
        data = MBI_AVLtree_getdata(tree, i);
        if (data == NULL)
        {
            errcount += 1;
            continue;
        }
        if (*(int*)data != (i * 10)) errcount += 1;
    }
    CU_ASSERT_EQUAL(errcount, 0);
   
    /* hit it with random data :) */
    errcount = 0;
    for (i = 0; i < 1000; i++)
    {
        v = rand();
        data = MBI_AVLtree_getdata(tree, v);
        if (v < TEST_AVL_TREESIZE && data == NULL) errcount ++;
        else if (v >= TEST_AVL_TREESIZE && data != NULL) errcount ++;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    
    /* destroy tree */
    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
}
