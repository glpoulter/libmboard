/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_pooled_list.h"

pooled_list* create_and_populate_pl(int memblock_size, int node_count) {
    
    int i, rc;
    void *ptr;
    pooled_list *pl;
    my_message_t *new;
    
    /* create PL obj with memblock size 10 */
    rc = pl_create(&pl, sizeof(my_message_t), memblock_size);
        
    /* make sure object creation successful before proceeding */
    CU_ASSERT_FATAL(rc == PL_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(pl);
    
    /* add nodes to pl obj */
    for (i = 0; i < node_count; i++)
    {
        rc = pl_newnode(pl, &ptr);
        new = (my_message_t *)ptr;
        
        assert(rc == PL_SUCCESS); /* quit if addnode failed */
        assert(new != NULL);
        
        /* add some value to nodes */
        new->ernet = i;
    }
    
    return pl;
}

void destroy_pl_object(pooled_list **pl) {
    
    /* Delete object */
    CU_ASSERT(PL_SUCCESS == pl_delete(pl));
    CU_ASSERT_PTR_NULL_FATAL(*pl);
    
}
