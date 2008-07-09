/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */

#ifndef HEADER_MB_PARALLEL_H_
#define HEADER_MB_PARALLEL_H_

#include "CUnit/CUnit.h"
#include "mb_parallel.h"
#include "mb_syncqueue.h"
#include "mboard.h"
#include "mb_objmap.h"
#include <mpi.h>
#include <stdlib.h>


#define PARALLEL_TEST_MSG_COUNT (int)(MB_CONFIG_PARALLEL_POOLSIZE * 3.5)
#define NODE_ZERO (0 == testsuite_mpi_rank)

extern MPI_Comm testsuite_mpi_comm;
extern int testsuite_mpi_rank, testsuite_mpi_size;

/* ===== Begin Function prototype for test cases ===== */

/* dummy messages */
struct dummy_msg{
    int ernet;
    long john, silver;
    float away;
    double jeopardy;
    
    /* messages MUST HAVE a next pointer */
    struct dummy_msg *next;
};
typedef struct dummy_msg dummy_msg;

struct dummy_msg2{
    int eger;
    double trouble;
    
    /* messages MUST HAVE a next pointer */
    struct dummy_msg2 *next;
};
typedef struct dummy_msg2 dummy_msg2;

/* ----- Utilities in testsuite_mb_serial_utils.c ----- */

/* initialise message board with content */
int init_mb_with_content(MBt_Board *mb_ptr);

/* map message sequence ID to value */
int get_message_value_from_id(int id);


/* ----- Source File : test_mb_p1.c ----- */

/* MB Environment initialisation */
void test_mb_p_init(void);

/* test finalise */
void test_mb_p_finalise(void);

/* ----- Source File : test_mb_p2.c ----- */

/* Test message board creation */
void test_mb_p_create(void);

/* ----- Source File : test_mb_p3.c ----- */

/* test clearing message board */
void test_mb_p_clear(void);

/* ----- Source File : test_mb_p4.c ----- */

/* test adding message to board */
void test_mb_p_addmessage(void);

/* test adding many message to board (beyond single pool block) */
void test_mb_p_addmessage_many(void);

/* ----- Source File : test_mb_p5.c ----- */

/* Test MB_Iterator_Create */
void test_mb_p_iter_create(void);

/* Test MB_Iterator_GetMessage */
void test_mb_p_iter_getmsg(void);

/* Test MB_Iterator_Delete */
void test_mb_p_iter_delete(void);

/* ----- Source File : test_mb_p6.c ----- */

/* Test MB_Iterator_CreateSorted */
void test_mb_p_iter_create_sorted(void);

/* Test MB_Iterator_GetMessage on Sorted Iterator */
void test_mb_p_iter_sorted_getmsg(void);

/* ----- Source File : test_mb_p7.c ----- */

/* Test MB_Iterator_CreateFiltered */
void test_mb_p_iter_create_filtered(void);

/* Test MB_Iterator_GetMessage on Filtered Iterator */
void test_mb_p_iter_filtered_getmsg(void);

/* ----- Source File : test_mb_p8.c ----- */

/* Test MB_Iterator_Rewind */
void test_mb_p_iter_rewind(void);

/* ----- Source File : test_mb_p9.c ----- */

/* Test MB_Iterator_Randomise */
void test_mb_p_iter_randomise(void);

/* ----- Source File : test_mb_p10.c ----- */

/* Test MB_Iterator_CreateFilteredSorted */
void test_mb_p_iter_create_filteredsorted(void);

/* Test MB_Iterator_GetMessage on Filtered+Sorted Iterator */
void test_mb_p_iter_filteredsorted_getmsg(void);

/* ----- Source File : test_mb_p11.c ----- */

/* test registering a function */
void test_mb_p_function_register(void);

/* test assigning a function to a board */
void test_mb_p_function_assign(void);


#endif /*HEADER_MB_PARALLEL_H_*/
