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

/* we want an even number here! */
#define PARALLEL_TEST_MSG_COUNT 2048
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

/* get list of random unique integers */
void generate_random_unique_ints(int *array, int size);

int is_in_array(int *array, int size, int value);



/* MB Environment initialisation */
void test_mb_p_init(void);

/* test finalise */
void test_mb_p_finalise(void);

/* Test message board creation */
void test_mb_p_create(void);

/* test clearing message board */
void test_mb_p_clear(void);

/* test adding message to board */
void test_mb_p_addmessage(void);

/* test adding many message to board (beyond single pool block) */
void test_mb_p_addmessage_many(void);

/* Test MB_Iterator_Create */
void test_mb_p_iter_create(void);

/* Test MB_Iterator_GetMessage */
void test_mb_p_iter_getmsg(void);

/* Test MB_Iterator_Delete */
void test_mb_p_iter_delete(void);

/* Test MB_Iterator_CreateSorted */
void test_mb_p_iter_create_sorted(void);

/* Test MB_Iterator_GetMessage on Sorted Iterator */
void test_mb_p_iter_sorted_getmsg(void);

/* Test MB_Iterator_CreateFiltered */
void test_mb_p_iter_create_filtered(void);

/* Test MB_Iterator_GetMessage on Filtered Iterator */
void test_mb_p_iter_filtered_getmsg(void);

/* Test MB_Iterator_Rewind */
void test_mb_p_iter_rewind(void);

/* Test MB_Iterator_Randomise */
void test_mb_p_iter_randomise(void);

/* Test MB_Iterator_CreateFilteredSorted */
void test_mb_p_iter_create_filteredsorted(void);

/* Test MB_Iterator_GetMessage on Filtered+Sorted Iterator */
void test_mb_p_iter_filteredsorted_getmsg(void);

/* testing MB_SetAccessMode() */
void test_mb_p_setaccessmode(void);

/* testing MB_SetSyncPattern() */
void test_mb_p_setsyncpattern(void);

/* testing filter operations */
void test_mb_p_filter_create(void);
void test_mb_p_filter_assign(void);
void test_mb_p_filter_delete(void);

/* sync */
void test_mb_p_sync_basic(void);
void test_mb_p_sync_checkcontent(void);
void test_mb_p_sync_withfilter(void);
void test_mb_p_sync_withfilter_fdr(void);
void test_mb_p_sync_indexmap(void);
void test_mb_p_sync_accessmode(void);
void test_mb_p_sync_pattern(void);
void test_mb_p_sync_resync(void);
void test_mb_p_sync_resync_filtered(void);

/* testing index map */
void test_mb_p_indexmap_create(void);
void test_mb_p_indexmap_delete(void);
void test_mb_p_indexmap_sync(void);
void test_mb_p_indexmap_memberof(void);
void test_mb_p_indexmap_memberof_withdups(void);
void test_mb_p_indexmap_memberof_randomvals(void);
void test_mb_p_indexmap_memberof_someoverlaps(void);
void test_mb_p_indexmap_memberof_nooverlaps(void);
void test_mb_p_indexmap_addentry(void);
void test_mb_p_indexmap_addentry_withdups(void);
void test_mb_p_indexmap_addentry_randomvals(void);


#endif /*HEADER_MB_PARALLEL_H_*/
