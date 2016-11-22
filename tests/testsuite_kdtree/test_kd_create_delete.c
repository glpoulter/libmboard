/* $Id: test_kd_create_delete.c 2925 2012-07-20 14:12:17Z lsc $ */
/*
 * Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * Author: Shawn Chin
 * Date  : July 2012
 *
 */
#include "header_kdtree.h"

static double (*extract_2d[])(void*) = { extract_x, extract_y };
static double (*extract_3d[])(void*) = { extract_x, extract_y, extract_z };

/* test creation and deletion of an empty kd tree */
void test_kd_create_delete_empty(void) {
	int rc;
	pooled_list *pl = NULL;
	MBIt_KDtree *tree = NULL;

	/* create empty pooled list */
	rc = pl_create(&pl, sizeof(my_message_t), DEFAULT_POOL_SIZE);
	CU_ASSERT_EQUAL_FATAL(rc, PL_SUCCESS);

	/* create a 2d tree using from an empty pool */
	rc = MBI_KDtree_Create(&tree, pl, 2, extract_2d);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL(tree);
	MBI_KDtree_Destroy(&tree);
	/* destroy */
	CU_ASSERT_PTR_NULL(tree);

	/* create a 3d tree using from an empty pool */
	rc = MBI_KDtree_Create(&tree, pl, 3, extract_3d);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL(tree);
	/* destroy */
	MBI_KDtree_Destroy(&tree);
	CU_ASSERT_PTR_NULL(tree);

	/* clean up */
	pl_delete(&pl);
}

/* test creation and deletion of an empty kd tree */
void test_kd_create_delete(void) {
	int rc, i;
	const int count = 1001;
	void *ptr;
	my_message_t *new;
	pooled_list *pl = NULL;
	MBIt_KDtree *tree = NULL;

	/* create pooled list and populate */
	rc = pl_create(&pl, sizeof(my_message_t), DEFAULT_POOL_SIZE);
	CU_ASSERT_EQUAL_FATAL(rc, PL_SUCCESS);
	for (i = 0; i < count; i++) {
		pl_newnode(pl, &ptr);
		new = (my_message_t*)ptr;
		new->x = random_range(0.0, (double)count);
		new->y = random_range(0.0, (double)count);
		new->z = random_range(0.0, (double)count);
		new->value = i;
	}

	/* create a 2d tree using from an empty pool */
	rc = MBI_KDtree_Create(&tree, pl, 2, extract_2d);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL(tree);
	MBI_KDtree_Destroy(&tree);
	/* destroy */
	CU_ASSERT_PTR_NULL(tree);

	/* create a 3d tree using from an empty pool */
	rc = MBI_KDtree_Create(&tree, pl, 3, extract_3d);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL(tree);
	/* destroy */
	MBI_KDtree_Destroy(&tree);
	CU_ASSERT_PTR_NULL(tree);

	/* clean up */
	pl_delete(&pl);
}
