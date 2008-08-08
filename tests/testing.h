/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#ifndef TESTING_H_
#define TESTING_H_
#include "CUnit/CUnit.h"

/* Test suite for Pooled List */
CU_ErrorCode testsuite_pl(void);

/* Test suite for Object Mapper */
CU_ErrorCode testsuite_om(void);

/* Test suite for Tag Table */
CU_ErrorCode testsuite_tt(void);

/* Test suite for Serial Libmboard */
CU_ErrorCode testsuite_mb_serial(void);

/* Test suite for Communication Queue */
CU_ErrorCode testsuite_commqueue(void);

/* Test suite for Sync Request Queue */
CU_ErrorCode testsuite_syncqueue(void);

/* Test suite for communication routines */
CU_ErrorCode testsuite_commroutines(void);

/* Test suite for Parallel Libmboard */
CU_ErrorCode testsuite_mb_parallel(void);

#endif /*TESTING_H_*/
