/* $Id: test_avl_getmin_getmax.c 1871 2009-06-12 13:58:18Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2009
 * 
 */
#include "header_avltree.h"
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

void test_avl_getmin_getmax(void) {
    
    int i, j;
    int rnd;
    int minval = INT_MAX;
    int maxval = INT_MIN;
    
    MBIt_AVLtree *tree = NULL;
    
    /* check with invalid tree */
    CU_ASSERT_EQUAL(MBI_AVLtree_getmin(tree), AVL_CONTROL_VAL);
    CU_ASSERT_EQUAL(MBI_AVLtree_getmax(tree), AVL_CONTROL_VAL);
    
    tree = MBI_AVLtree_create();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tree);
    CU_ASSERT_EQUAL(tree->count, 0);
    CU_ASSERT_PTR_NULL(tree->root);
    
    /* check with empty tree */
    CU_ASSERT_EQUAL(MBI_AVLtree_getmin(tree), AVL_CONTROL_VAL);
    CU_ASSERT_EQUAL(MBI_AVLtree_getmax(tree), AVL_CONTROL_VAL);
    
    /* repeat whole thing thrice, unless test fails */
    for (j = 0; j < 3; j++)
    {
        for (i = 0; i < TEST_AVL_TREESIZE; i++)
        {
            rnd = rand(); /* get a random number */
            MBI_AVLtree_insert(tree, rnd, NULL); /* insert node with rand value */
            
            /* calculate our own min and max */
            minval = MIN(minval, rnd);
            maxval = MAX(maxval, rnd);
        }
        
        check_tree_integrity(tree);
        
        CU_ASSERT_EQUAL(MBI_AVLtree_getmin(tree), minval);
        CU_ASSERT_EQUAL(MBI_AVLtree_getmax(tree), maxval);
        
        if ((MBI_AVLtree_getmin(tree) != minval) || 
                (MBI_AVLtree_getmax(tree) != maxval))
        {
            break;
        }
    }
    
    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
    
}
