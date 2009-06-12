/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2009
 * 
 */
#include "header_avltree.h"

void test_avl_getnode(void) {
    
    int i, v, errcount;
    int temp = 0;
    int testvals[TEST_AVL_TREESIZE];
    MBIt_AVLtree *tree = NULL;
    MBIt_AVLnode *node = NULL;
    
    /* test NULL tree */
    node = MBI_AVLtree_getnode(tree, 0);
    CU_ASSERT_PTR_NULL(node);
    
    /* create tree */
    tree = MBI_AVLtree_create();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tree);
    CU_ASSERT_EQUAL(tree->count, 0);
    CU_ASSERT_PTR_NULL(tree->root);
    
    /* test empty tree */
    node = MBI_AVLtree_getnode(tree, 0);
    CU_ASSERT_PTR_NULL(node);
    
    /* it shouldn't matter if we do get duplicate values in the tree */
    for (i = 0; i < TEST_AVL_TREESIZE; i++) 
    { 
        v = rand();
        testvals[i] = v; /* remember values used */
        
        MBI_AVLtree_insert(tree, v, NULL);
    }

    check_tree_integrity(tree);
    
    /* use getnode to retrieve node pointer and inspect key value */
    errcount = 0;
    for (i = 0; i < TEST_AVL_TREESIZE; i++) 
    { 
        v = testvals[i]; /* recall values used */
        
        node = MBI_AVLtree_getnode(tree, v);
        if (node == NULL)
        {
            errcount += 1;
            continue;
        }
        
        if (node->key != v || node->data != NULL) errcount += 1;
        
        /* attach data to node */
        node->data = &temp;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    
    /* change value of temp */
    temp = 55699;
    
    /* use getnode to retrieve node pointer and inspect new data value */
    errcount = 0;
    for (i = 0; i < TEST_AVL_TREESIZE; i++) 
    { 
        v = testvals[i]; /* recall values used */
        
        node = MBI_AVLtree_getnode(tree, v);
        if (node == NULL)
        {
            errcount += 1;
            continue;
        }
        
        if (node->key != v) errcount += 1;
        if (node->data != &temp) {errcount += 1; continue;}
        if (*(int*)node->data != 55699) errcount += 1;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    
    
    /* destroy tree */
    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
}

void test_avl_getnode_invalid(void) {
    
    int i, v;
    MBIt_AVLtree *tree = NULL;
    
    /* create tree */
    tree = MBI_AVLtree_create();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tree);
    CU_ASSERT_EQUAL(tree->count, 0);
    CU_ASSERT_PTR_NULL(tree->root);
    
    /* test with empty tree */
    for (i = 0; i < 5; i++) 
    {
        v = rand();
        CU_ASSERT_PTR_NULL(MBI_AVLtree_getnode(tree, v));
    }   
    
    /* test when tree has some content */
    for (i = 0; i < 5; i++) 
    {
        MBI_AVLtree_insert(tree, i, NULL);
        
        v = rand();
        while (v > i && v < 5) v = rand();
        
        CU_ASSERT_PTR_NULL(MBI_AVLtree_getnode(tree, v));
        CU_ASSERT_PTR_NOT_NULL(MBI_AVLtree_getnode(tree, i));
    }   
    
    /* destroy tree */
    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
}

/* we expect _insert(tree, key, ptr) to have been tested before this 
 * test is called */
void test_avl_getnode_modvalue(void) {
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

    /* update data directly via node poitner */
    for (i = 0; i < TEST_AVL_TREESIZE; i++) 
    {
        node = MBI_AVLtree_getnode(tree, i);
        if (node == NULL)
        {
            CU_FAIL("Could not retrieve node pointer");
            break;
        }
        
        if (node->data == NULL)
        {
            CU_FAIL("node->data is NULL");
            break;
        }
        data = (int*)node->data;
        *data = i*99; /* update value */
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
        data = (int*)(node->data);
        if (*data != (i * 99)) errcount += 1;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    
    /* destroy tree */
    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
}
