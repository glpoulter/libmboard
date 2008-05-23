/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_pooled_list.h"
#include <math.h>

/* test linked list traversal */
void test_pl_traverse(void) {
    
    int i;
    int memblock_size = 10;
    int nodes_to_add  = 5;
    int errcount_ptr, errcount_val;
    pooled_list *pl;
    pl_address_node *current;

    pl = create_and_populate_pl(memblock_size, nodes_to_add);
    
    /* try traversing the linked list */
    current = PL_ITERATOR(pl);
    errcount_ptr = errcount_val = 0;
    for (i = 0; i < nodes_to_add; i++)
    {
        if (current == NULL) errcount_ptr++;
        if (((my_message_t*)PL_NODEDATA(current))->ernet != i) errcount_val++;;
        current = current->next;
    }
    CU_ASSERT_EQUAL(errcount_ptr, 0);
    CU_ASSERT_EQUAL(errcount_val, 0);
    
    destroy_pl_object(&pl);
}

/* test linked list traversal across multiple memblock */
void test_pl_traverse2(void) {
    
    int i;
    int memblock_size = 10;
    int nodes_to_add  = 105;
    int errcount_ptr, errcount_val;
    pooled_list *pl;
    pl_address_node *current;
    
    pl = create_and_populate_pl(memblock_size, nodes_to_add);
    
    /* try traversing the linked list */
    current = PL_ITERATOR(pl);
    errcount_ptr = errcount_val = 0;
    for (i = 0; i < nodes_to_add; i++)
    {
        if (current == NULL) errcount_ptr++;
        if (((my_message_t*)PL_NODEDATA(current))->ernet != i) errcount_val++;;
        current = current->next;
    }
    CU_ASSERT_EQUAL(errcount_ptr, 0);
    CU_ASSERT_EQUAL(errcount_val, 0);
    
    destroy_pl_object(&pl);
}

/* test getting node by index */
void test_pl_getnode(void) {
    
    int rc, index;
    pooled_list *pl;
    int memblock_size = 10;
    int nodes_to_add  = 105;
    my_message_t *msg = NULL;
    
    
    /* create and populate list */
    pl = create_and_populate_pl(memblock_size, nodes_to_add);
    
    /* test invalid values */
    rc = pl_getnode(pl, -1, (void *)&msg);
    CU_ASSERT_EQUAL(rc, PL_ERR_INVALID);
    CU_ASSERT_PTR_NULL(msg);
    
    rc = pl_getnode(pl, nodes_to_add, (void *)&msg);
    CU_ASSERT_EQUAL(rc, PL_ERR_INVALID);
    CU_ASSERT_PTR_NULL(msg);
    
    rc = pl_getnode(pl, nodes_to_add + 1, (void *)&msg);
    CU_ASSERT_EQUAL(rc, PL_ERR_INVALID);
    CU_ASSERT_PTR_NULL(msg);
    
    rc = pl_getnode(pl, nodes_to_add + 1, (void *)&msg);
    CU_ASSERT_EQUAL(rc, PL_ERR_INVALID);
    CU_ASSERT_PTR_NULL(msg);
    
    index = (int)(floor((double)nodes_to_add / memblock_size) + 1)* memblock_size;
    rc = pl_getnode(pl, index + 1, (void *)&msg);
    CU_ASSERT_EQUAL(rc, PL_ERR_INVALID);
    CU_ASSERT_PTR_NULL(msg);
    
    
    /* first node */
    index = 0;
    rc = pl_getnode(pl, index, (void *)&msg);
    CU_ASSERT_EQUAL(rc, PL_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(msg);
    if (msg) CU_ASSERT_EQUAL(msg->ernet, index);
    
    
    /* last node */
    index = nodes_to_add - 1;
    rc = pl_getnode(pl, index, (void *)&msg);
    CU_ASSERT_EQUAL(rc, PL_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(msg);
    if (msg) CU_ASSERT_EQUAL(msg->ernet, index);
    
    /* last node of first block */
    index = memblock_size - 1;
    if (index < nodes_to_add)
    {
        rc = pl_getnode(pl, index, (void *)&msg);
        CU_ASSERT_EQUAL(rc, PL_SUCCESS);
        CU_ASSERT_PTR_NOT_NULL(msg);
        if (msg) CU_ASSERT_EQUAL(msg->ernet, index);
    }
    
    /* first node of second block */
    index = memblock_size;
    if (index < nodes_to_add)
    {
        rc = pl_getnode(pl, index, (void *)&msg);
        CU_ASSERT_EQUAL(rc, PL_SUCCESS);
        CU_ASSERT_PTR_NOT_NULL(msg);
        if (msg) CU_ASSERT_EQUAL(msg->ernet, index);
    }
    
    /* first node of last block */
    index = (int)floor((double)nodes_to_add / memblock_size) * memblock_size;
    if (index < nodes_to_add)
    {
        rc = pl_getnode(pl, index, (void *)&msg);
        CU_ASSERT_EQUAL(rc, PL_SUCCESS);
        CU_ASSERT_PTR_NOT_NULL(msg);
        if (msg) CU_ASSERT_EQUAL(msg->ernet, index);
    }
    
    /* Destroy list */
    destroy_pl_object(&pl);
}
