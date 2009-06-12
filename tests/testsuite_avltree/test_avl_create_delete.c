/* $Id:$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2009
 * 
 */
#include "header_avltree.h"

/* Testing object creation and deletion */
void test_avl_create_delete(void) {
    
    MBIt_AVLtree *tree = NULL;
    
    tree = MBI_AVLtree_create();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tree);
    
    CU_ASSERT_EQUAL(tree->count, 0);
    CU_ASSERT_PTR_NULL(tree->root);
    
    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
    
    /* check if it it can survive invalid data */
    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
}
