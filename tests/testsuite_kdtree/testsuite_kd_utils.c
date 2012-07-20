/* $Id$ */
/*
 * Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * Author: Shawn Chin
 * Date  : July 2012
 *
 */
#include "header_kdtree.h"

/* function to extract x value from anonymous msg pointer */
double extract_x(void *msg_ptr) {
	return ((my_message_t*)msg_ptr)->x;
}

/* function to extract x value from anonymous msg pointer */
double extract_y(void *msg_ptr) {
	return ((my_message_t*)msg_ptr)->y;
}

/* function to extract x value from anonymous msg pointer */
double extract_z(void *msg_ptr) {
	return ((my_message_t*)msg_ptr)->z;
}

/* returns random value from range [from, to] */
double random_range(double from, double to) {
  double span = to - from;
  double scaled_rand = (double)rand() / RAND_MAX;
  return scaled_rand * span + from;
}

static int cmp(const void* v1, const void* v2) {
	const int a = *((const int*)v1);
	const int b = *((const int*)v2);
	return ((a > b) ? 1 : ((a < b) ? -1 : 0));
}

/* compare values of messages with a give SORTED array */
int validate_content(size_t count, pooled_list* pl, const int check[]) {
	size_t i = 0;
	my_message_t *msg;
	pl_address_node *pl_itr;
	int *values;

	/* check if number of messages match */
	if (count != pl->count_current) return 0;

	/* get data from iterator */
	values = malloc(sizeof(int) * count);
	for (pl_itr = PL_ITERATOR(pl); pl_itr; pl_itr = pl_itr->next) {
		msg = *((my_message_t**)PL_NODEDATA(pl_itr));
		values[i++] = msg->value;
	}

	/* sort */
	qsort(values, count, sizeof(int), cmp);
	for (i = 0; i < count; i++) {
		if (values[i] != check[i]) {
			free(values);
			return 0;
		}
	}

	free(values);
	return 1;
}
