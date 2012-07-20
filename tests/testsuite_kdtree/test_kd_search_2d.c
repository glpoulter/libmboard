/* $Id$ */
/*
 * Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * Author: Shawn Chin
 * Date  : July 2012
 *
 */
#include "header_kdtree.h"

static double (*extract_2d[])(void*) = { extract_x, extract_y };

/* perform 2d search on an empty tree */
void test_kd_search_2d_empty(void) {
	int rc;
	pooled_list *result;
	pooled_list *pl = NULL;
	MBIt_KDtree *tree = NULL;

	/* create empty pooled list */
	rc = pl_create(&pl, sizeof(my_message_t), DEFAULT_POOL_SIZE);
	CU_ASSERT_EQUAL_FATAL(rc, PL_SUCCESS);

	/* create a 2d tree from an empty pool */
	rc = MBI_KDtree_Create(&tree, pl, 2, extract_2d);
	CU_ASSERT_EQUAL_FATAL(rc, KDTREE_SUCCESS);

	/* do search with mismatching dims */
	double b0[] = { 0.0, 0.0 };
	rc = MBI_KDtree_Search(tree, &result, 1, b0);
	CU_ASSERT_PTR_NULL(result);
	CU_ASSERT_EQUAL(rc, KDTREE_ERR_INVALID);
	double b0a[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	rc = MBI_KDtree_Search(tree, &result, 3, b0a);
	CU_ASSERT_PTR_NULL(result);
	CU_ASSERT_EQUAL(rc, KDTREE_ERR_INVALID);

	/* do actual search */
	double b1[] = { 0.0, 10.0, 1.0, 2.0 };
	rc = MBI_KDtree_Search(tree, &result, 2, b1);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL(result);
	if (result != NULL) {
		CU_ASSERT_EQUAL(result->count_current, 0);
	}

	/* clean up */
	MBI_KDtree_Destroy(&tree);
	pl_delete(&result);
	pl_delete(&pl);
}

/* perform 2d search on an empty tree */
void test_kd_search_2d_single_item(void) {
	int rc;
	void *ptr;
	my_message_t *new;
	pooled_list *result;
	pooled_list *pl = NULL;
	MBIt_KDtree *tree = NULL;
	pl_address_node *pl_itr;

	/* create pooled list */
	rc = pl_create(&pl, sizeof(my_message_t), DEFAULT_POOL_SIZE);
	CU_ASSERT_EQUAL_FATAL(rc, PL_SUCCESS);

	/* add one item */
	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = -3.0;
	new->y = 5.0;
	new->value = 123;

	/* create a 2d tree */
	rc = MBI_KDtree_Create(&tree, pl, 2, extract_2d);
	CU_ASSERT_EQUAL_FATAL(rc, KDTREE_SUCCESS);

	/* do search (match x, not y) */
	double b0[] = { -4.0, 10, 1.0, 3.0 };
	rc = MBI_KDtree_Search(tree, &result, 2, b0);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 0);
	pl_delete(&result);

	/* do search (match y, not x) */
	double b1[] = { -40.0, -10.0, 2.2, 5.5 };
	rc = MBI_KDtree_Search(tree, &result, 2, b1);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 0);
	pl_delete(&result);

	/* do search (match) */
	double b2[] = { -10.0, 99.0, 0.0, 1000.0 };
	rc = MBI_KDtree_Search(tree, &result, 2, b2);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 1);
	pl_itr = PL_ITERATOR(result);
	if (pl_itr != NULL) {
		new = *((my_message_t**)PL_NODEDATA(pl_itr));
		CU_ASSERT_EQUAL(new->value, 123);
	}
	pl_delete(&result);

	/* do search (match boundary) */
	double b3[] = { -3.0, 0.0, 5.0, 6.0 };
	rc = MBI_KDtree_Search(tree, &result, 2, b3);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 1);
	pl_itr = PL_ITERATOR(result);
	if (pl_itr != NULL) {
		new = *((my_message_t**)PL_NODEDATA(pl_itr));
		CU_ASSERT_EQUAL(new->value, 123);
	}
	pl_delete(&result);


	/* clean up */
	MBI_KDtree_Destroy(&tree);
	pl_delete(&pl);
}


/* test search with various conditions */
void test_kd_search_2d(void) {
	int rc;
	void *ptr;
	my_message_t *new;
	pooled_list *result;
	pooled_list *pl = NULL;
	MBIt_KDtree *tree = NULL;

	/* create empty pooled list */
	rc = pl_create(&pl, sizeof(my_message_t), DEFAULT_POOL_SIZE);
	CU_ASSERT_EQUAL_FATAL(rc, PL_SUCCESS);

	/* add the following points
	 * { (-2,6), (-1,0), (0,0), (0,0), (0,0), (-1,-3), (1,-2), (2,-2) }
	 *      0      1       2      3      4       5       6       7
	 *
	 * This ensures that we also test the case where multiple points
	 * fall on the median point and end up getting split across
	 * two regions.
	 */
	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = -2.0;
	new->y = 6.0;
	new->value = 0;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = -1.0;
	new->y = 0.0;
	new->value = 1;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = 0.0;
	new->y = 0.0;
	new->value = 2;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = 0.0;
	new->y = 0.0;
	new->value = 3;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = 0.0;
	new->y = 0.0;
	new->value = 4;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = -1.0;
	new->y = -3.0;
	new->value = 5;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = 1.0;
	new->y = -2.0;
	new->value = 6;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = 2.0;
	new->y = -2.0;
	new->value = 7;


	/* create a 2d tree */
	rc = MBI_KDtree_Create(&tree, pl, 2, extract_2d);
	CU_ASSERT_EQUAL_FATAL(rc, KDTREE_SUCCESS);

	/* ------ try various searches ------------------ */

	/* no match (bottom left) */
	double b0[] = { -100.0, -1.01, -1000.0, 5.999 };
	rc = MBI_KDtree_Search(tree, &result, 2, b0);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 0);
	pl_delete(&result);

	/* no match (top right) */
	double b1[] = { -1.999, 1000.0, 0.0001, 1000.0 };
	rc = MBI_KDtree_Search(tree, &result, 2, b1);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 0);
	pl_delete(&result);

	/* all match (exactly on boundaries) */
	double b2[] = { -2.0, 2.0, -3.0, 6.0 };
	rc = MBI_KDtree_Search(tree, &result, 2, b2);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 8);
	const int check1[] = {0, 1, 2, 3, 4, 5, 6, 7};
	CU_ASSERT(validate_content(8, result, check1));
	pl_delete(&result);

	/* all match (beyond boundaries) */
	double b3[] = { -5.0, 100.0, -10.0, 10.0 };
	rc = MBI_KDtree_Search(tree, &result, 2, b3);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 8);
	const int check2[] = {0, 1, 2, 3, 4, 5, 6, 7};
	CU_ASSERT(validate_content(8, result, check2));
	pl_delete(&result);

	/* match top left subset */
	double b4[] = { -2.0, 0.0, 0.0, 6.0 };
	rc = MBI_KDtree_Search(tree, &result, 2, b4);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 5);
	const int check3[] = {0, 1, 2, 3, 4};
	CU_ASSERT(validate_content(5, result, check3));
	pl_delete(&result);

	/* match bottom right subset */
	double b5[] = { -1.0, 2.0, -3.0, 0.0 };
	rc = MBI_KDtree_Search(tree, &result, 2, b5);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 7);
	const int check4[] = {1, 2, 3, 4, 5, 6, 7};
	CU_ASSERT(validate_content(7, result, check4));
	pl_delete(&result);

	/* clean up */
	MBI_KDtree_Destroy(&tree);
	pl_delete(&pl);
}


