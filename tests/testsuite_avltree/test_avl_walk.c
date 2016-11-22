/* $Id: test_avl_walk.c 1871 2009-06-12 13:58:18Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2009
 * 
 */
#include "header_avltree.h"

static int walkfunc(MBIt_AVLnode *node) {
    
    if (node == NULL) CU_FAIL("walk func should never see a NULL node");
    if (node->data != NULL) 
    {
        free(node->data);
        node->data = NULL;
    }
    return 0;
}

static void manual_walk(MBIt_AVLnode *node) {
    
    if (node == NULL) return;
    if (node->data != NULL) 
    {
        CU_FAIL("First walk did not work!");
        return;
    }
    manual_walk(node->child[AVL_LEFT]);
    manual_walk(node->child[AVL_RIGHT]);
}

static void manual_cleanup(MBIt_AVLnode *node) {
    
    if (node == NULL) return;
    if (node->data != NULL)  free(node->data);
    manual_cleanup(node->child[AVL_LEFT]);
    manual_cleanup(node->child[AVL_RIGHT]);
}

void test_avl_walk(void) {
    
    int i, rc;
    MBIt_AVLtree *tree = NULL;
    
    /* test NULL tree */
    rc = MBI_AVLtree_walk(tree, &walkfunc);
    CU_ASSERT_EQUAL(rc, AVL_ERR_INVALID);
    
    /* create tree */
    tree = MBI_AVLtree_create();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tree);
    CU_ASSERT_EQUAL(tree->count, 0);
    CU_ASSERT_PTR_NULL(tree->root);
    
    /* test empty tree */
    rc = MBI_AVLtree_walk(tree, &walkfunc);
    CU_ASSERT_EQUAL(rc, AVL_SUCCESS);
    
    /* add nodes */
    for (i = 0; i < TEST_AVL_TREESIZE; i++) 
    { 
        MBI_AVLtree_insert(tree, i, malloc(10));
    }

    /* walk the tree */
    rc = MBI_AVLtree_walk(tree, &walkfunc);
    CU_ASSERT_EQUAL(rc, AVL_SUCCESS);
    
    /* check results manually */
    manual_walk(tree->root);
    /* clean up */
    manual_cleanup(tree->root);
    
    /* destroy tree */
    MBI_AVLtree_destroy(&tree);
    CU_ASSERT_PTR_NULL(tree);
}
