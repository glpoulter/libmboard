/* $Id: test_pl_reset_recycle.c 2925 2012-07-20 14:12:17Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_pooled_list.h"

/* test pl_recycle() */
void test_pl_recycle(void) {
    
    int i, rc;
    int number_of_blocks_found;
    int memblock_size;
    int nodes_to_add;
    int target_blocks;
    int errcount, errcount2, errcount3, errcount4;
    pooled_list *pl = NULL;
    pl_address_node *addrnode;
    my_message_t *new;
    
    /* first, try recycling an invalid pointer */
    CU_ASSERT(pl_recycle(pl) == PL_ERR_INVALID);
    
    
    
    /* make sure we have multiple blocks */
    memblock_size = 7;
    nodes_to_add  = 23;
    
    /* create and populate object */
    pl = create_and_populate_pl(memblock_size, nodes_to_add);
    
    /* recycle the object. this should reset the list, but leave mem intact */
    rc = pl_recycle(pl);
    CU_ASSERT_EQUAL_FATAL(rc, PL_SUCCESS);
    
    /* How many memory blocks should we have? */
    /* (+ 1.0) because we allocate new memblock when current one filled */
    target_blocks = (int)ceil(((float)nodes_to_add + 1.0) / (float)memblock_size);
    
    number_of_blocks_found = 0;
    addrnode = pl->addr_list;
    errcount = 0;
    while(addrnode) /* count address nodes */
    {
        if (addrnode->addr == NULL) errcount++;
        number_of_blocks_found++;
        addrnode = addrnode->next;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    CU_ASSERT_EQUAL(number_of_blocks_found, target_blocks);
    
    /* Check all default internal values */
    CU_ASSERT((int)pl->elem_size     == (int)sizeof(my_message_t));
    CU_ASSERT((int)pl->elem_count    == memblock_size);
    
    /* count_free/total reflects status in current memory block */ 
    CU_ASSERT((int)pl->count_free    == memblock_size);
    CU_ASSERT((int)pl->count_total   == memblock_size);
    CU_ASSERT((int)pl->count_current == 0);
    
    /* Check address node list */
    CU_ASSERT_PTR_NOT_NULL(pl->addr_list);   /* address node exists */
    CU_ASSERT_PTR_EQUAL(pl->active_memblock, pl->addr_list); /*active blk set*/
    
    /* check datablock pointers */
    CU_ASSERT_PTR_NULL(pl->head); /* head pointer unset */
    CU_ASSERT_PTR_NULL(pl->tail); /* tail pointer unset */
    CU_ASSERT_PTR_EQUAL(pl->next_free, pl->addr_list->addr); /* next_free set */ 
    
    
    
    /* add a few nodes, just for laughs */
    errcount = errcount2 = errcount3 = errcount4 = 0;
    for (i = 0; i < memblock_size*2; i++)
    {
        rc = pl_newnode(pl, (void *)&new);
        
        if (new == NULL) errcount++;
        if (rc != PL_SUCCESS) errcount2++;
        
        if (rc == PL_SUCCESS) new->ernet = i;
        
    }
    CU_ASSERT_EQUAL(errcount, 0);
    CU_ASSERT_EQUAL(errcount2, 0);
    CU_ASSERT_EQUAL(errcount3, 0);
    CU_ASSERT_EQUAL(errcount4, 0);
    
    destroy_pl_object(&pl);
}

/* test pl_reset() */
void test_pl_reset(void) {
    
    int i, rc;
    int number_of_blocks_found;
    int memblock_size;
    int nodes_to_add;
    int target_blocks;    
    int errcount, errcount2, errcount3, errcount4;
    pooled_list *pl = NULL;
    pl_address_node *addrnode;
    my_message_t *new;
    void *ptr;
    
    /* first, try recycling an invalid pointer */
    CU_ASSERT(pl_reset(pl) == PL_ERR_INVALID);
    
    
    
    /* make sure we have multiple blocks */
    memblock_size = 7;
    nodes_to_add  = 23;
    
    /* create and populate object */
    pl = create_and_populate_pl(memblock_size, nodes_to_add);
    
    /* recycle the object. this should reset the list, but leave mem intact */
    rc = pl_reset(pl);
    CU_ASSERT_EQUAL_FATAL(rc, PL_SUCCESS);
    
    /* How many memory blocks should we have? */
    target_blocks = 1;
    
    number_of_blocks_found = 0;
    addrnode = pl->addr_list;
    errcount = 0;
    while(addrnode) /* count address nodes */
    {
        if (addrnode->addr == NULL) errcount++;
        number_of_blocks_found++;
        addrnode = addrnode->next;
    }
    CU_ASSERT_EQUAL(errcount, 0);
    CU_ASSERT_EQUAL(number_of_blocks_found, target_blocks);
    
    /* Check all default internal values */
    CU_ASSERT((int)pl->elem_size     == (int)sizeof(my_message_t));
    CU_ASSERT((int)pl->elem_count    == memblock_size);
    
    /* count_free/total reflects status in current memory block */ 
    CU_ASSERT((int)pl->count_free    == memblock_size);
    CU_ASSERT((int)pl->count_total   == memblock_size);
    CU_ASSERT((int)pl->count_current == 0);
    
    /* Check address node list */
    CU_ASSERT_PTR_NOT_NULL(pl->addr_list);   /* address node exists */
    CU_ASSERT_PTR_EQUAL(pl->active_memblock, pl->addr_list); /*active blk set*/
    
    /* check datablock pointers */
    CU_ASSERT_PTR_NULL(pl->head); /* head pointer unset */
    CU_ASSERT_PTR_NULL(pl->tail); /* tail pointer unset */
    CU_ASSERT_PTR_EQUAL(pl->next_free, pl->addr_list->addr); /* next_free set */ 
    
    
    /* add a few nodes, just for laughs */
    errcount = errcount2 = errcount3 = errcount4 = 0;
    for (i = 0; i < memblock_size*2; i++)
    {
        rc = pl_newnode(pl, &ptr);
        new = (my_message_t *)ptr;
        
        if (new == NULL) errcount++;
        if (rc != PL_SUCCESS) errcount2++;
        
        if (rc == PL_SUCCESS) new->ernet = i;
        
    }
    CU_ASSERT_EQUAL(errcount, 0);
    CU_ASSERT_EQUAL(errcount2, 0);
    CU_ASSERT_EQUAL(errcount3, 0);
    CU_ASSERT_EQUAL(errcount4, 0);
    
    destroy_pl_object(&pl);
}
