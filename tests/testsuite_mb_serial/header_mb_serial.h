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
#include <stdlib.h>

#define SERIAL_TEST_MSG_COUNT (int)(MB_CONFIG_SERIAL_POOLSIZE * 3.5)

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


/* ----- Source File : test_mb_s1.c ----- */

/* MB Environment initialisation */
void test_mb_s_init(void);

/* test message board creation and deletion */
void test_mb_s_create_delete(void);

/* test creation of multiple boards */
void test_mb_s_create_multiple(void);

/* ----- Source File : test_mb_s2.c ----- */

/* test adding message to board */
void test_mb_s_addmessage(void);

/* test adding many message to board (beyond single pool block) */
void test_mb_s_addmessage_many(void);

/* ----- Source File : test_mb_s3.c ----- */

/* test clearing message board */
void test_mb_s_clear(void);

/* ----- Source File : test_mb_s4.c ----- */

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

/* ----- Source File : test_mb_s5.c ----- */

/* Test MB_Iterator_Create */
void test_mb_s_iter_create(void);

/* Test MB_Iterator_GetMessage */
void test_mb_s_iter_getmsg(void);

/* Test MB_Iterator_Delete */
void test_mb_s_iter_delete(void);

/* ----- Source File : test_mb_s6.c ----- */

/* Test MB_Iterator_CreateSorted */
void test_mb_s_iter_create_sorted(void);

/* Test MB_Iterator_GetMessage on Sorted Iterator */
void test_mb_s_iter_sorted_getmsg(void);

/* ----- Source File : test_mb_s7.c ----- */

/* Test MB_Iterator_CreateFiltered */
void test_mb_s_iter_create_filtered(void);

/* Test MB_Iterator_GetMessage on Filtered Iterator */
void test_mb_s_iter_filtered_getmsg(void);

/* ----- Source File : test_mb_s8.c ----- */

/* Test MB_Iterator_Rewind */
void test_mb_s_iter_rewind(void);

/* ----- Source File : test_mb_s9.c ----- */

/* Test MB_Iterator_Randomise */
void test_mb_s_iter_randomise(void);

/* ----- Source File : test_mb_s10.c ----- */

/* Test MB_Iterator_CreateFilteredSorted */
void test_mb_s_iter_create_filteredsorted(void);

/* Test MB_Iterator_GetMessage on Filtered+Sorted Iterator */
void test_mb_s_iter_filteredsorted_getmsg(void);

/* ----- Source File : test_mb_s12.c ----- */

/* testing MB_Function_Register() */
void test_mb_s_function_register(void);

/* testing MB_Function_Assign() */
void test_mb_s_function_assign(void);

/* testing MB_Function_Free() */
void test_mb_s_function_free(void);
#endif /*HEADER_MB_SERIAL_H_*/
