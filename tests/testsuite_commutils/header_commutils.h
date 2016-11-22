/* $Id: header_commutils.h 2172 2009-09-30 10:24:15Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Aug 2009
 * 
 */
#ifndef HEADER_COMMUTILS_H_
#define HEADER_COMMUTILS_H_

#include "../testing.h"
#include "mb_commqueue.h"
#include "mb_commroutines.h"
#include "mb_parallel.h"
#include "mboard.h"

extern int TCU_CommRank, TCU_CommSize;

/* setup and teardown routines */
int init_cu(void);
int clean_cu(void);

/* utility routines */
int mock_filter (const void *msg, int pid);
int mock_filter2 (const void *msg, int pid);
MBIt_Board* mock_board_create(void);
void mock_board_delete(MBIt_Board **board_ptr);
void mock_board_reset_accessmode(MBIt_Board *board);
void mock_board_addmessage(MBIt_Board *board, void *msg);
void mock_board_populate(MBIt_Board *board, int count);

/* test routines */
void test_cu_tagmessages(void);
void test_cu_updatecursor(void);
void test_cu_buildbuffer_all(void);
void test_cu_buildbuffers_tagged(void);
void test_cu_loadbuffer(void);

#endif /*HEADER_COMMUTILS_H_*/
