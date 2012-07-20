/* $Id$ */
/*
 * Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * Author: Lee-Shawn Chin
 * Date  : July 2012
 *
 */
#include <float.h>
#include "header_mb_serial.h"

#define TEST_DATA_SIZE 1000

/* dummy message type */
typedef struct {
	double x;
	double y;
	double z;
	int value;
} message_t;

/* function to extract x value from anonymous msg pointer */
static double extract_x(void *msg_ptr) {
	return ((message_t*)msg_ptr)->x;
}

/* function to extract x value from anonymous msg pointer */
static double extract_y(void *msg_ptr) {
	return ((message_t*)msg_ptr)->y;
}

/* function to extract x value from anonymous msg pointer */
static double extract_z(void *msg_ptr) {
	return ((message_t*)msg_ptr)->z;
}

static void populate_board(MBt_Board *mb_ptr) {
	int i, rc;
	MBt_Board mb;
	message_t msg;

	rc = MB_Create(&mb, sizeof(message_t));
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);

	for (i = 0; i < TEST_DATA_SIZE; i++) {
		msg.x = (double)i;
		msg.y = (double)i;
		msg.z = (double)i;
		msg.value = i;

		MB_AddMessage(mb, (void*)&msg);
	}

	*mb_ptr = mb;
}

/* test error conditions */
void test_mb_s_searchtree_null(void) {
	int rc;
	MBt_Board mb = (MBt_Board)MB_NULL_MBOARD;
	MBt_SearchTree tree;
	MBt_Iterator iter;
	MBIt_Board *board;

	/* create search tree from null board */
	mb = MB_NULL_MBOARD;
	rc = MB_SearchTree_Create1D(mb, &tree, extract_x);
	CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
	CU_ASSERT_EQUAL(tree, MB_NULL_SEARCHTREE);
	rc = MB_SearchTree_Create2D(mb, &tree, extract_x, extract_y);
	CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
	CU_ASSERT_EQUAL(tree, MB_NULL_SEARCHTREE);
	rc = MB_SearchTree_Create3D(mb, &tree, extract_x, extract_y, extract_z);
	CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
	CU_ASSERT_EQUAL(tree, MB_NULL_SEARCHTREE);

	/* create search tree from invalid board */
	mb = (MBt_Board)9999;
	rc = MB_SearchTree_Create1D(mb, &tree, extract_x);
	CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
	CU_ASSERT_EQUAL(tree, MB_NULL_SEARCHTREE);

	/* searching an null search tree */
	tree = MB_NULL_SEARCHTREE;
	rc = MB_SearchTree_Search1D(tree, &iter, 0.0, 10.0);
	CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
	CU_ASSERT_EQUAL(iter, MB_NULL_ITERATOR);

	/* searching an invalid search tree */
	tree = (MBt_SearchTree)999;
	rc = MB_SearchTree_Search1D(tree, &iter, 0.0, 10.0);
	CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
	CU_ASSERT_EQUAL(iter, MB_NULL_ITERATOR);

	/* create empty board */
	rc = MB_Create(&mb, sizeof(message_t));
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);

	/* fiddle with the internals of the board so we can test for
	 * locked and unreadable states
	 */
	board = MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb);
	CU_ASSERT_EQUAL_FATAL(board->locked, MB_FALSE);
	board->locked = MB_TRUE;
	rc = MB_SearchTree_Create1D(mb, &tree, extract_x);
	CU_ASSERT_EQUAL_FATAL(rc, MB_ERR_LOCKED);
	board->locked = MB_FALSE;

	CU_ASSERT_EQUAL_FATAL(board->is_reader, MB_TRUE);
	board->is_reader = MB_FALSE;
	rc = MB_SearchTree_Create1D(mb, &tree, extract_x);
	CU_ASSERT_EQUAL_FATAL(rc, MB_ERR_DISABLED);
	board->is_reader = MB_TRUE;

	/* create a valid board */
	rc = MB_SearchTree_Create1D(mb, &tree, extract_x);
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);

	/* searching a locked board */
	board->locked = MB_TRUE;
	rc = MB_SearchTree_Search1D(tree, &iter, 0.0, 10.0);
	CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
	board->locked = MB_FALSE;

	/* check for mismatching dimensions */
	rc = MB_SearchTree_Search2D(tree, &iter, 0.0, 10.0, 0.0, 10.0);
	CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
	CU_ASSERT_EQUAL(iter, MB_NULL_ITERATOR);

	rc = MB_SearchTree_Search3D(tree, &iter, 0.0, 10.0, 0.0, 10.0, 0.0, 10.0);
	CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
	CU_ASSERT_EQUAL(iter, MB_NULL_ITERATOR);

	rc = MB_SearchTree_Delete(&tree);
	CU_ASSERT_EQUAL(rc, MB_SUCCESS);
	MB_Delete(&mb);

	/* board no longer valid */
	rc = MB_SearchTree_Create1D(mb, &tree, extract_x);
	CU_ASSERT_EQUAL_FATAL(rc, MB_ERR_INVALID);

	/* deleting a null tree */
	tree = (MBt_SearchTree)MB_NULL_SEARCHTREE;
	rc = MB_SearchTree_Delete(&tree);
	CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}

void test_mb_s_searchtree_empty(void) {
	int rc;
	void *msg;
	MBt_Board mb = (MBt_Board)MB_NULL_MBOARD;
	MBt_SearchTree tree;
	MBt_Iterator iter;

	/* create empty board */
	rc = MB_Create(&mb, sizeof(message_t));
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);

	/* create and search a 1D tree */
	rc = MB_SearchTree_Create1D(mb, &tree, extract_x);
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
	rc = MB_SearchTree_Search1D(tree, &iter, -DBL_MAX, DBL_MAX);
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
	rc = MB_Iterator_GetMessage(iter, &msg);
	CU_ASSERT_EQUAL(rc, MB_SUCCESS);
	CU_ASSERT_PTR_NULL(msg);
	MB_Iterator_Delete(&iter);
	MB_SearchTree_Delete(&tree);

	/* create and search a 2D tree */
	rc = MB_SearchTree_Create2D(mb, &tree, extract_x, extract_y);
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
	rc = MB_SearchTree_Search2D(tree, &iter, -DBL_MAX, DBL_MAX,
											 -DBL_MAX, DBL_MAX);
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
	rc = MB_Iterator_GetMessage(iter, &msg);
	CU_ASSERT_EQUAL(rc, MB_SUCCESS);
	CU_ASSERT_PTR_NULL(msg);
	MB_Iterator_Delete(&iter);
	MB_SearchTree_Delete(&tree);

	/* create and search a 3D tree */
	rc = MB_SearchTree_Create3D(mb, &tree, extract_x, extract_y, extract_z);
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
	rc = MB_SearchTree_Search3D(tree, &iter, -DBL_MAX, DBL_MAX,
											 -DBL_MAX, DBL_MAX,
											 -DBL_MAX, DBL_MAX);
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
	rc = MB_Iterator_GetMessage(iter, &msg);
	CU_ASSERT_EQUAL(rc, MB_SUCCESS);
	CU_ASSERT_PTR_NULL(msg);
	MB_Iterator_Delete(&iter);
	MB_SearchTree_Delete(&tree);

	MB_Delete(&mb);
}

void test_mb_s_searchtree_1D(void) {
	int rc;
	message_t *msg_ptr;
	MBt_Board mb;
	MBt_SearchTree tree;
	MBt_Iterator iter;
	populate_board(&mb);

	/* create 1D Search Tree */
	rc = MB_SearchTree_Create1D(mb, &tree, extract_x);
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);

	/* Simple search. Expect to match 1 item only */
	/* Note: move thorough test of the search algorithm is down
	 * within the testsuite for MBI_KDtree_Search().
	 */
	rc = MB_SearchTree_Search1D(tree, &iter, -0.5, 0.5);
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
	MB_Iterator_GetMessage(iter, (void**)&msg_ptr); /* first result */
	CU_ASSERT_PTR_NOT_NULL_FATAL(msg_ptr);
	CU_ASSERT_EQUAL(msg_ptr->value, 0);
	free(msg_ptr);
	MB_Iterator_GetMessage(iter, (void**)&msg_ptr); /* no more?*/
	CU_ASSERT_PTR_NULL(msg_ptr);

	/* clean up */
	MB_Iterator_Delete(&iter);
	MB_SearchTree_Delete(&tree);
	MB_Delete(&mb);
}

void test_mb_s_searchtree_2D(void) {
	int rc;
	message_t *msg_ptr;
	MBt_Board mb;
	MBt_SearchTree tree;
	MBt_Iterator iter;
	populate_board(&mb);

	/* create 2D Search Tree */
	rc = MB_SearchTree_Create2D(mb, &tree, extract_x, extract_y);
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);

	/* Simple search. Expect to match 1 item only */
	/* Note: move thorough test of the search algorithm is down
	 * within the testsuite for MBI_KDtree_Search().
	 */
	rc = MB_SearchTree_Search2D(tree, &iter, -0.5, 0.5, -0.5, 0.5);
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
	MB_Iterator_GetMessage(iter, (void**)&msg_ptr); /* first result */
	CU_ASSERT_PTR_NOT_NULL_FATAL(msg_ptr);
	CU_ASSERT_EQUAL(msg_ptr->value, 0);
	free(msg_ptr);
	MB_Iterator_GetMessage(iter, (void**)&msg_ptr); /* no more?*/
	CU_ASSERT_PTR_NULL(msg_ptr);

	/* clean up */
	MB_Iterator_Delete(&iter);
	MB_SearchTree_Delete(&tree);
	MB_Delete(&mb);
}

void test_mb_s_searchtree_3D(void) {
	int rc;
	message_t *msg_ptr;
	MBt_Board mb;
	MBt_SearchTree tree;
	MBt_Iterator iter;
	populate_board(&mb);

	/* create 2D Search Tree */
	rc = MB_SearchTree_Create3D(mb, &tree, extract_x, extract_y, extract_z);
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);

	/* Simple search. Expect to match 1 item only */
	/* Note: move thorough test of the search algorithm is down
	 * within the testsuite for MBI_KDtree_Search().
	 */
	rc = MB_SearchTree_Search3D(tree, &iter, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5);
	CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
	MB_Iterator_GetMessage(iter, (void**)&msg_ptr); /* first result */
	CU_ASSERT_PTR_NOT_NULL_FATAL(msg_ptr);
	CU_ASSERT_EQUAL(msg_ptr->value, 0);
	free(msg_ptr);
	MB_Iterator_GetMessage(iter, (void**)&msg_ptr); /* no more?*/
	CU_ASSERT_PTR_NULL(msg_ptr);

	/* clean up */
	MB_Iterator_Delete(&iter);
	MB_SearchTree_Delete(&tree);
	MB_Delete(&mb);
}

