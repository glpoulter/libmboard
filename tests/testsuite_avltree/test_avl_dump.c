/* $Id: test_avl_dump.c 1871 2009-06-12 13:58:18Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2009
 * 
 */
#include "header_avltree.h"

void test_avl_dump(void) {
    
    int i, rc;
    int sum;
    int size;
    int *dumparray;
    MBIt_AVLtree *tree = NULL;
    
    /* checking with NULL tree */
    rc = MBI_AVLtree_dump(tree, &dumparray, &size);
    CU_ASSERT_EQUAL(rc, AVL_ERR_INVALID);
    CU_ASSERT_EQUAL(size, 0);
    CU_ASSERT_PTR_NULL(dumparray);
    
    tree = MBI_AVLtree_create();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tree);
    CU_ASSERT_EQUAL(tree->count, 0);
    CU_ASSERT_PTR_NULL(tree->root);
    
    /* checking with EMPTY tree */
    rc = MBI_AVLtree_dump(tree, &dumparray, &size);
    CU_ASSERT_EQUAL(rc, AVL_SUCCESS);
    CU_ASSERT_EQUAL(size, 0);
    CU_ASSERT_PTR_NULL(dumparray);
    
    sum = 0;
    for (i = 0; i < TEST_AVL_TREESIZE; i++) 
    { 
        MBI_AVLtree_insert(tree, i, NULL);
        sum += i;
    }
    
    rc = MBI_AVLtree_dump(tree, &dumparray, &size);
    CU_ASSERT_EQUAL(rc, AVL_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(dumparray);
    CU_ASSERT_EQUAL(size, TEST_AVL_TREESIZE);

    for (i = 0; i < size; i++) 
    {
        sum -= dumparray[i];
    }
    CU_ASSERT_EQUAL(sum, 0);
    if (dumparray) free(dumparray);
    
    /* destroy tree */
    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
    
}

/* TODO: check that the order of the dumped data minimised rotation */
