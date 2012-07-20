/* $Id$ */
/*
 * Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * Author: Shawn Chin
 * Date  : July 2012
 *
 */

#ifndef HEADER_KDTREE_H_
#define HEADER_KDTREE_H_

#include "../testing.h"
#include "mb_pooled_list.h"
#include "mb_kdtree.h"

#define DEFAULT_POOL_SIZE 100

/* test message */
typedef struct  {
    double x;
    double y;
    double z;
    int value;
} my_message_t;

/* setup_suite_kdtree.c */
int init_kd(void);
int clean_kd(void);

/* testsuite_kd_utils.c */
double extract_x(void *msg_ptr);
double extract_y(void *msg_ptr);
double extract_z(void *msg_ptr);
double random_range(double from, double to);
int validate_content(size_t count, pooled_list* pl, const int check[]);

/* test_kd_create_delete.c */
void test_kd_create_delete_empty(void);
void test_kd_create_delete(void);

/* test_kd_search_1d.c */
void test_kd_search_1d_empty(void);
void test_kd_search_1d_single_item(void);
void test_kd_search_1d(void);

/* test_kd_search_2d.c */
void test_kd_search_2d_empty(void);
void test_kd_search_2d_single_item(void);
void test_kd_search_2d(void);

/* test_kd_search_3d.c */
void test_kd_search_3d_empty(void);
void test_kd_search_3d_single_item(void);
void test_kd_search_3d(void);

#endif /*HEADER_KDTREE_H_*/
