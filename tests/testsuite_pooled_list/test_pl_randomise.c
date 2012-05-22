/* $Id: $ */
/* 
 * Copyright (c) 2012 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2012
 * 
 */
 
#include "header_pooled_list.h"
#define MEMBLOCK_SIZE 1000
#define NUM_ENTRIES 1000000

void test_pl_randomise(void) {
  
  int i, rc, val;
  int err_nullptr = 0, err_val = 0, err_entries = 0;
  pooled_list *pl;
  pl_address_node *current;
  
  int *index_counter = calloc(NUM_ENTRIES, sizeof(int));
  CU_ASSERT(index_counter != NULL);

  pl = create_and_populate_pl(MEMBLOCK_SIZE, NUM_ENTRIES);
  rc = pl_randomise(pl);
  CU_ASSERT_EQUAL(rc, PL_SUCCESS);

  /* iterate through pl and check that all entries are accounted for */
  current = PL_ITERATOR(pl);
  for (i = 0; i < NUM_ENTRIES; i++) {
      if (current == NULL) {
        err_nullptr++;
        break;
      }
      
      val = ((my_message_t*)PL_NODEDATA(current))->ernet;
      if (val < 0 || val >= NUM_ENTRIES) err_val++;
      
      index_counter[val]++;
      current = current->next;
  }
  CU_ASSERT_EQUAL(err_nullptr, 0);
  CU_ASSERT_EQUAL(err_val, 0);
  
  for (i = 0; i < NUM_ENTRIES; i++) {
      if (index_counter[i] != 1) err_entries++;
  }
  CU_ASSERT_EQUAL(err_entries, 0);
  
  free(index_counter);
  destroy_pl_object(&pl);
}
