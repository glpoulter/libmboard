/* $Id:$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_pooled_list.h"

void test_pl_create_delete(void) {
    
    int rc;
    pooled_list *pl = NULL;
    
    rc = pl_create(&pl, sizeof(my_message_t), ELEM_COUNT_DEFAULT);
    
    /* make sure object creation successful before proceeding */
    CU_ASSERT_EQUAL_FATAL(rc, PL_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(pl);
    
    /* Check all default internal values */
    CU_ASSERT((int)pl->elem_size == (int)sizeof(my_message_t));
    CU_ASSERT(pl->elem_count    == ELEM_COUNT_DEFAULT);
    CU_ASSERT(pl->count_free    == ELEM_COUNT_DEFAULT);
    CU_ASSERT(pl->count_total   == ELEM_COUNT_DEFAULT);
    CU_ASSERT(pl->count_current == 0);
    
    /* Check address node list */
    CU_ASSERT_PTR_NOT_NULL(pl->addr_list);   /* address node exists */
    CU_ASSERT_PTR_NULL(pl->addr_list->next); /* no second node */
    CU_ASSERT_PTR_NOT_NULL(pl->addr_list->addr); /* memory block allocated */
    CU_ASSERT_PTR_EQUAL(pl->active_memblock, pl->addr_list); /*active blk set*/
    
    /* check datablock pointers */
    CU_ASSERT_PTR_NULL(pl->head); /* head pointer unset */
    CU_ASSERT_PTR_NULL(pl->tail); /* tail pointer unset */
    CU_ASSERT_PTR_EQUAL(pl->next_free, pl->addr_list->addr); /* next_free set */ 
    
    destroy_pl_object(&pl);
}


void test_pl_multisize(void) {
    
    int rc;
    int elem_count, elem_count2;
    
    pooled_list *pl  = NULL;
    pooled_list *pl2 = NULL;
    
    /* use different memblock sizes ( >1 ) */
    elem_count = ELEM_COUNT_DEFAULT + 50;
    elem_count2 = (int)(ELEM_COUNT_DEFAULT / 2) + 3;
    
    /* create first object */
    rc = pl_create(&pl, sizeof(my_message_t), elem_count);
    CU_ASSERT_FATAL(rc == PL_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(pl);
    
    /* create object of different size */
    rc = pl_create(&pl2, sizeof(bigger_message_t), elem_count2);
    CU_ASSERT_FATAL(rc == PL_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(pl2);
    
    /* make sure both have memory block allocated */
    CU_ASSERT_PTR_NOT_NULL(pl->addr_list->addr);
    CU_ASSERT_PTR_NOT_NULL(pl2->addr_list->addr);
    
    /* make sure both have correct values assigned values */
    CU_ASSERT((int)pl->count_total == elem_count);
    CU_ASSERT((int)pl->elem_count  == elem_count);
    CU_ASSERT((int)pl->elem_size   == (int)sizeof(my_message_t));
    
    CU_ASSERT((int)pl2->count_total == elem_count2);
    CU_ASSERT((int)pl2->elem_count  == elem_count2);
    CU_ASSERT((int)pl2->elem_size   == (int)sizeof(bigger_message_t));
    
    destroy_pl_object(&pl);
    destroy_pl_object(&pl2);
}

/* test using invalid input */
void test_pl_create_invalid(void) {
    
    int rc;
    pooled_list *pl;
    
    /* trying invalid element size
     * size_t is unsigned. So we don't check for negative size */
    rc = pl_create(&pl, (size_t)0, ELEM_COUNT_DEFAULT);
    CU_ASSERT(rc == PL_ERR_INVALID);
    CU_ASSERT_PTR_NULL(pl);
    pl_delete(&pl);
    
    /* trying invalid element counts */
    rc = pl_create(&pl, sizeof(my_message_t), 0);
    CU_ASSERT(rc == PL_ERR_INVALID);
    CU_ASSERT_PTR_NULL(pl);
    pl_delete(&pl);
    
    rc = pl_create(&pl, sizeof(my_message_t), -1);
    CU_ASSERT(rc == PL_ERR_INVALID);
    CU_ASSERT_PTR_NULL(pl);
    pl_delete(&pl);
}


