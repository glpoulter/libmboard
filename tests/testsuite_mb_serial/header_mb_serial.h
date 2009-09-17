/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#ifndef HEADER_MB_SERIAL_H_
#define HEADER_MB_SERIAL_H_

#include "CUnit/CUnit.h"
#include "mb_serial.h"
#include "mb_objmap.h"
#include <stdlib.h>

#define SERIAL_TEST_MSG_COUNT 2048

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
void test_mb_s_init(void);

/* test message board creation and deletion */
void test_mb_s_create_delete(void);

/* test creation of multiple boards */
void test_mb_s_create_multiple(void);

/* test adding message to board */
void test_mb_s_addmessage(void);

/* test adding many message to board (beyond single pool block) */
void test_mb_s_addmessage_many(void);

/* test clearing message board */
void test_mb_s_clear(void);

/* Test MB_SyncStart */
void test_mb_s_sync_start(void);

/* Test MB_SyncStartGroup */
void test_mb_s_sync_startgroup(void);

/* Test MB_SyncTest */
void test_mb_s_sync_test(void);

/* Test MB_SyncTestGroup */
void test_mb_s_sync_testgroup(void);

/* Test MB_SyncComplete */
void test_mb_s_sync_complete(void);

/* Test MB_SyncCompleteGroup */
void test_mb_s_sync_completegroup(void);

/* Test MB_Iterator_Create */
void test_mb_s_iter_create(void);

/* Test MB_Iterator_GetMessage */
void test_mb_s_iter_getmsg(void);

/* Test MB_Iterator_Delete */
void test_mb_s_iter_delete(void);

/* Test MB_Iterator_CreateSorted */
void test_mb_s_iter_create_sorted(void);

/* Test MB_Iterator_GetMessage on Sorted Iterator */
void test_mb_s_iter_sorted_getmsg(void);

/* Test MB_Iterator_CreateFiltered */
void test_mb_s_iter_create_filtered(void);

/* Test MB_Iterator_GetMessage on Filtered Iterator */
void test_mb_s_iter_filtered_getmsg(void);

/* Test MB_Iterator_Rewind */
void test_mb_s_iter_rewind(void);

/* Test MB_Iterator_Randomise */
void test_mb_s_iter_randomise(void);

/* Test MB_Iterator_CreateFilteredSorted */
void test_mb_s_iter_create_filteredsorted(void);

/* Test MB_Iterator_GetMessage on Filtered+Sorted Iterator */
void test_mb_s_iter_filteredsorted_getmsg(void);

/* testing MB_Function_Register() */
void test_mb_s_filter_create(void);

/* testing MB_Function_Assign() */
void test_mb_s_filter_assign(void);

/* testing MB_Function_Free() */
void test_mb_s_filter_delete(void);

/* testing MB_SetAccessMode() */
void test_mb_s_setaccessmode(void);

/* testing MB_SetSyncPattern() */
void test_mb_s_setsyncpattern(void);

/* testing index map */
void test_mb_s_indexmap_create(void);
void test_mb_s_indexmap_delete(void);
void test_mb_s_indexmap_sync(void);
void test_mb_s_indexmap_memberof(void);
void test_mb_s_indexmap_memberof_withdups(void);
void test_mb_s_indexmap_memberof_randomvals(void);
void test_mb_s_indexmap_addentry(void);
void test_mb_s_indexmap_addentry_withdups(void);
void test_mb_s_indexmap_addentry_randomvals(void);

#endif /*HEADER_MB_SERIAL_H_*/
