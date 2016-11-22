/* $Id: test_kd_search_1d.c 2925 2012-07-20 14:12:17Z lsc $ */
/*
 * Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * Author: Shawn Chin
 * Date  : July 2012
 *
 */
#include "header_kdtree.h"

static double (*extract_1d[])(void*) = { extract_x };


/* perform 1d search on an empty tree */
void test_kd_search_1d_empty(void) {
	int rc;
	pooled_list *result;
	pooled_list *pl = NULL;
	MBIt_KDtree *tree = NULL;

	/* create empty pooled list */
	rc = pl_create(&pl, sizeof(my_message_t), DEFAULT_POOL_SIZE);
	CU_ASSERT_EQUAL_FATAL(rc, PL_SUCCESS);

	/* create a 1d tree from an empty pool */
	rc = MBI_KDtree_Create(&tree, pl, 1, extract_1d);
	CU_ASSERT_EQUAL_FATAL(rc, KDTREE_SUCCESS);

	/* do search with mismatching dims */
	double b0[] = {0.0, 0.0, 0.0, 0.0};
	rc = MBI_KDtree_Search(tree, &result, 2, b0);
	CU_ASSERT_PTR_NULL(result);
	CU_ASSERT_EQUAL(rc, KDTREE_ERR_INVALID);

	/* do actual search */
	double b1[] = {0.0, 10};
	rc = MBI_KDtree_Search(tree, &result, 1, b1);
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

/* perform 1d search on an empty tree */
void test_kd_search_1d_single_item(void) {
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
	new->value = 123;

	/* create a 1d tree */
	rc = MBI_KDtree_Create(&tree, pl, 1, extract_1d);
	CU_ASSERT_EQUAL_FATAL(rc, KDTREE_SUCCESS);

	/* do search (no result) */
	double b1[] = {-2.9, 10.0};
	rc = MBI_KDtree_Search(tree, &result, 1, b1);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 0);
	pl_delete(&result);

	/* do search (match) */
	double b2[] = {-10.0, 99.0};
	rc = MBI_KDtree_Search(tree, &result, 1, b2);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 1);
	pl_itr = PL_ITERATOR(result);
	if (pl_itr != NULL) {
		new = *((my_message_t**)PL_NODEDATA(pl_itr));
		CU_ASSERT_EQUAL(new->value, 123);
	}
	pl_delete(&result);

	/* do search (match - left boundary) */
	double b2a[] = { -3.0, 0.0 };
	rc = MBI_KDtree_Search(tree, &result, 1, b2a);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 1);
	pl_itr = PL_ITERATOR(result);
	if (pl_itr != NULL) {
		new = *((my_message_t**)PL_NODEDATA(pl_itr));
		CU_ASSERT_EQUAL(new->value, 123);
	}
	pl_delete(&result);

	/* do search (match - right boundary) */
	double b3[] = {-100, -3.0};
	rc = MBI_KDtree_Search(tree, &result, 1, b3);
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
void test_kd_search_1d(void) {
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
	 * { -3, -2, 0, 1.0, 1.0, 1.0, 2.0, 2.0, 4.0 }
	 *
	 * This ensures that we also test the case where multiple points
	 * fall on the median point and end up getting split across
	 * two regions.
	 */
	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = -3.0;
	new->value = 0;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = -2.0;
	new->value = 1;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = 0.0;
	new->value = 2;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = 1.0;
	new->value = 3;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = 1.0;
	new->value = 4;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = 1.0;
	new->value = 5;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = 2.0;
	new->value = 6;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = 2.0;
	new->value = 7;

	pl_newnode(pl, &ptr);
	new = (my_message_t*)ptr;
	new->x = 4.0;
	new->value = 8;

	/* create a 1d tree */
	rc = MBI_KDtree_Create(&tree, pl, 1, extract_1d);
	CU_ASSERT_EQUAL_FATAL(rc, KDTREE_SUCCESS);

	/* ------ try various searches ------------------ */

	/* no match (left edge) */
	double b0[] = {-100.0, -3.01};
	rc = MBI_KDtree_Search(tree, &result, 1, b0);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 0);
	pl_delete(&result);

	/* no match (right edge) */
	double b1[] = { 4.00001, 100000.0 };
	rc = MBI_KDtree_Search(tree, &result, 1, b1);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 0);
	pl_delete(&result);

	/* all match (exactly on boundaries) */
	double b2[] = { -3.0, 4,0 };
	rc = MBI_KDtree_Search(tree, &result, 1, b2);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 9);
	const int check1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	CU_ASSERT(validate_content(9, result, check1));
	pl_delete(&result);

	/* all match (beyond boundaries) */
	double b3[] = { -5.0, 100.0 };
	rc = MBI_KDtree_Search(tree, &result, 1, b3);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 9);
	const int check2[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	CU_ASSERT(validate_content(9, result, check2));
	pl_delete(&result);

	/* match left subset */
	double b4[] = { -2.5, 1.0 };
	rc = MBI_KDtree_Search(tree, &result, 1, b4);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 5);
	const int check3[] = {1, 2, 3, 4, 5};
	CU_ASSERT(validate_content(5, result, check3));
	pl_delete(&result);

	/* match right subset */
	double b5[] = { 2.0, 4.5 };
	rc = MBI_KDtree_Search(tree, &result, 1, b5);
	CU_ASSERT_EQUAL(rc, KDTREE_SUCCESS);
	CU_ASSERT_PTR_NOT_NULL_FATAL(result);
	CU_ASSERT_EQUAL(result->count_current, 3);
	const int check4[] = {6, 7, 8};
	CU_ASSERT(validate_content(3, result, check4));
	pl_delete(&result);

	/* clean up */
	MBI_KDtree_Destroy(&tree);
	pl_delete(&pl);
}


