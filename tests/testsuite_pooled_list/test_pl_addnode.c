/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 *  
 */

#include "header_pooled_list.h"

/* test adding nodes */
void test_pl_addnodes(void) {
    
    int rc;
    void *ptr;
    pooled_list *pl;
    my_message_t *firstnode;
    my_message_t *new;
    pl_address_node *dh; /* datablock header */
    const int elem_size = 5;
    const double trouble = 0.1235445953;
    
    rc = pl_create(&pl, sizeof(my_message_t), elem_size);
        
    /* make sure object creation successful before proceeding */
    CU_ASSERT_FATAL(rc == PL_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(pl);
    
    /* Add new node */
    rc = pl_newnode(pl, &ptr);
    new = (my_message_t *)ptr;
    
    CU_ASSERT(rc == PL_SUCCESS);      /* check return code */
    CU_ASSERT_PTR_NOT_NULL(new);      /* check return pointer */
    
    if (rc != PL_SUCCESS || !new) 
    { /* If pl_newnode fails, do no proceed with tests */
        pl_delete(&pl);
        CU_FAIL_FATAL("Could not add node to pooled list!");
    }
    
    /* add values to node */
    new->ernet = 42;
    new->john  = 2738;
    new->trouble = trouble;

    /* check that first block next pointer is null */
    CU_ASSERT_PTR_NULL(((pl_address_node*)pl->head)->next);
    
    /* Check values by going thru "head" pointer */
    firstnode = (my_message_t*)PL_NODEDATA(pl->head);
    CU_ASSERT(firstnode->ernet   == 42);
    CU_ASSERT(firstnode->john    == 2738);
    CU_ASSERT(firstnode->trouble == trouble);
    
 
    
    /* add next message */
    rc = pl_newnode(pl, &ptr);
    new = (my_message_t *)ptr;
    
    CU_ASSERT(rc == PL_SUCCESS);      /* check return code */
    CU_ASSERT_PTR_NOT_NULL(new);      /* check return pointer */
        
    if (rc != PL_SUCCESS || !new) 
    { /* If pl_newnode fails, do no proceed with other tests */
        pl_delete(&pl);
        CU_FAIL_FATAL("Could not add node to pooled list!");
    }
    
    new->ernet = 999;
    
    /* check that second node goes to proper position */
    dh = (pl_address_node*)pl->head;
    CU_ASSERT_PTR_EQUAL(dh->next->addr, (void *)new);
    CU_ASSERT(((my_message_t*)dh->next->addr)->ernet == 999);
    
    /* check that second node got chained in properly */
    CU_ASSERT_PTR_NULL(dh->next->next);
    
    /* check that first node still accessible */
    CU_ASSERT(firstnode->ernet   == 42);
    CU_ASSERT(firstnode->john    == 2738);
    CU_ASSERT(firstnode->trouble == trouble);
        
    destroy_pl_object(&pl);
}

/* test using invalid input */
void test_pl_addnode_invalid(void) {
    
    int rc;
    void *ptr;
    pooled_list *pl = NULL;
    my_message_t *new;

    /* Adding node with invalid pl */
    rc = pl_newnode(pl, &ptr);
    new = (my_message_t *)ptr;
    
    CU_ASSERT(rc == PL_ERR_INVALID); /* check return code */
    CU_ASSERT_PTR_NULL(new);      /* on error, return ptr should be null */

    
}

/* test memblock growth */
void test_pl_memblocks(void) {
   
    int errcount;
    int memblock_size;
    int nodes_to_add;
    int target_blocks;
    int number_of_blocks_found;
    pooled_list *pl;
    pl_address_node *addrnode;
    
    /* ----- half-fill first block ----- */
    memblock_size = 10;
    nodes_to_add  = 5;
    pl = create_and_populate_pl(memblock_size, nodes_to_add);
    
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
    
    destroy_pl_object(&pl);
    
    
    
    /* ----- Fill up first block ----- */
    memblock_size = 10;
    nodes_to_add  = 10;
    pl = create_and_populate_pl(memblock_size, nodes_to_add);
    
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
    
    destroy_pl_object(&pl);
    
    
    /* ----- unfilled ----- */
    memblock_size = 10;
    nodes_to_add  = 0;
    pl = create_and_populate_pl(memblock_size, nodes_to_add);
    
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
    
    destroy_pl_object(&pl);
    
    
    
    /* ----- multiple blocks ----- */
    memblock_size = 5;
    nodes_to_add  = 27;
    pl = create_and_populate_pl(memblock_size, nodes_to_add);
    
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
    
    destroy_pl_object(&pl);
    
    
    /* ----- multiple blocks (with border value) ----- */
    memblock_size = 5;
    nodes_to_add  = 25;
    pl = create_and_populate_pl(memblock_size, nodes_to_add);
    
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
    
    destroy_pl_object(&pl);
}
