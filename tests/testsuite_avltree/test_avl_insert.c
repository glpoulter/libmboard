/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2009
 * 
 * TODO: Add test to check that tree is balanced.
 */
#include "header_avltree.h"

/* test simple insertion */
void test_avl_insert_simple(void) {
    
    int i, errcount, rc;
    
    MBIt_AVLtree *tree = NULL;
    
    /* add to invalid tree */
    rc = MBI_AVLtree_insert(tree, 10, NULL);
    CU_ASSERT_EQUAL(rc, AVL_ERR_INVALID);
    
    tree = MBI_AVLtree_create();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tree);
    CU_ASSERT_EQUAL(tree->count, 0);
    CU_ASSERT_PTR_NULL(tree->root);
    
    errcount = 0;
    for (i = 0; i < TEST_AVL_TREESIZE; i++)
    {
        rc = MBI_AVLtree_insert(tree, i, NULL);
        if (rc != AVL_SUCCESS) {errcount += 1; continue;} 
        if (tree->count != i + 1) errcount += 1;
        if (tree->root == NULL) errcount += 1;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    
    check_tree_integrity(tree);
    
    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
    
}

void test_avl_insert_zigzag(void) {
    
    int i, errcount, rc, c, mid;
    
    MBIt_AVLtree *tree = NULL;
    
    /* add to invalid tree */
    rc = MBI_AVLtree_insert(tree, 10, NULL);
    CU_ASSERT_EQUAL(rc, AVL_ERR_INVALID);
    
    tree = MBI_AVLtree_create();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tree);
    CU_ASSERT_EQUAL(tree->count, 0);
    CU_ASSERT_PTR_NULL(tree->root);
    
    errcount = 0;
    c = 0;
    mid = TEST_AVL_TREESIZE / 2 + 1;
    for (i = 0; i < mid; i++)
    {
        rc = MBI_AVLtree_insert(tree, i, NULL); c++;
        if (rc != AVL_SUCCESS) {errcount += 1; continue;} 
        if (tree->count != c) errcount += 1;
        
        rc = MBI_AVLtree_insert(tree, TEST_AVL_TREESIZE - i, NULL); c++;
        if (rc != AVL_SUCCESS) {errcount += 1; continue;} 
        if (tree->count != c) errcount += 1;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    
    check_tree_integrity(tree);
    
    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
    
}
void test_avl_insert_duplicate(void) {

    MBIt_AVLtree *tree = NULL;
    tree = MBI_AVLtree_create();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tree);
    CU_ASSERT_EQUAL(tree->count, 0);
    CU_ASSERT_PTR_NULL(tree->root);
    
    /* add first node */
    MBI_AVLtree_insert(tree, 10, NULL);
    CU_ASSERT_EQUAL(tree->count, 1);
    MBI_AVLtree_insert(tree, 10, NULL); /* dup */
    CU_ASSERT_EQUAL(tree->count, 1);
    
    /* add next node */
    MBI_AVLtree_insert(tree, 12, NULL);
    CU_ASSERT_EQUAL(tree->count, 2);
    MBI_AVLtree_insert(tree, 10, NULL); /* dup */
    CU_ASSERT_EQUAL(tree->count, 2);
    MBI_AVLtree_insert(tree, 12, NULL); /* dup */
    CU_ASSERT_EQUAL(tree->count, 2);
    
    /* add next node */
    MBI_AVLtree_insert(tree, 13, NULL);
    CU_ASSERT_EQUAL(tree->count, 3);
    MBI_AVLtree_insert(tree, 10, NULL); /* dup */
    CU_ASSERT_EQUAL(tree->count, 3);
    MBI_AVLtree_insert(tree, 12, NULL); /* dup */
    CU_ASSERT_EQUAL(tree->count, 3);
    MBI_AVLtree_insert(tree, 13, NULL); /* dup */
    CU_ASSERT_EQUAL(tree->count, 3);

    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
    
}

/* we expect _getnode() to have been tested before this test is called */
void test_avl_insert_withptr(void) {
    int i, errcount;
    int *data;
    int testvals[TEST_AVL_TREESIZE];
    MBIt_AVLtree *tree = NULL;
    MBIt_AVLnode *node = NULL;
    
    /* create tree */
    tree = MBI_AVLtree_create();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tree);
    CU_ASSERT_EQUAL(tree->count, 0);
    CU_ASSERT_PTR_NULL(tree->root);
    
    /* it shouldn't matter if we do get duplicate values in the tree */
    for (i = 0; i < TEST_AVL_TREESIZE; i++) 
    { 
        testvals[i] = i * 10; /* remember values used */
        
        MBI_AVLtree_insert(tree, i, &testvals[i]);
    }

    /* use getnode to retrieve node pointer and inspect key value */
    errcount = 0;
    for (i = 0; i < TEST_AVL_TREESIZE; i++) 
    { 
        node = MBI_AVLtree_getnode(tree, i);
        if (node == NULL)
        {
            errcount += 1;
            continue;
        }
        
        if (node->key != i) errcount += 1;
        if (node->data == NULL) {errcount += 1; continue;}
        data = (int*)node->data;
        if (*data != (i * 10)) errcount += 1;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    
    
    /* destroy tree */
    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
}


