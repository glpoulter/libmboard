/* $Id: header_commroutines_handshake.h 2172 2009-09-30 10:24:15Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Sept 2009
 * 
 */
#ifndef HEADER_COMMROUTINE_HANDSHAKE_H_
#define HEADER_COMMROUTINE_HANDSHAKE_H_

#include "../testing.h"
#include "mb_commqueue.h"
#include "mb_commroutines.h"
#include "mb_parallel.h"
#include "mboard.h"

/* ----- global vars ------- */

#define SMALLNUM 10
/* must be an even number! */
#define TEST_MESSAGE_COUNT (int)(2048 + SMALLNUM)

/* RW on all procs */
extern MBt_Board mb_rw_hs;
extern struct MBIt_commqueue *node_rw_hs;
/* RW on all procs. No messages */
extern MBt_Board mb_rw_empty_hs;
extern struct MBIt_commqueue *node_rw_empty_hs;
/* RW on all procs. Has filter that uses indexmap */
extern MBt_Board mb_rw_fmap_hs;
extern struct MBIt_commqueue *node_rw_fmap_hs;
/* RW on all procs. Has filter that will cause fallback to full data
 * replication when procs > 3 */
extern MBt_Board mb_rw_fdr_hs;
extern struct MBIt_commqueue *node_rw_fdr_hs;
/* RW on even procs. IDLE on odd */
extern MBt_Board mb_rw_id_hs;
extern struct MBIt_commqueue *node_rw_id_hs;
/* RO on even procs. WO on odd */
extern MBt_Board mb_ro_wo_hs;
extern struct MBIt_commqueue *node_ro_wo_hs;

/* filters */
extern MBt_Filter fl_map_hs, fl_fdr_hs;
/* indexmap for use with filter_map */
extern MBt_IndexMap imap_hs;


/* -------- function declarations ------ */

/* in test_cr_handshake_utils.c */
int fl_func_map_hs(const void *m, int pid);
int fl_func_fdr_hs(const void *m, int pid);
int _initialise_map_values_hs(MBt_IndexMap map);

/* setup and teardown routines */
int init_cr_handshake(void);
int clean_cr_handshake(void);

/* test routines */
void test_cr_handshake_agreebufsizes_rw(void);
void test_cr_handshake_agreebufsizes_rw_empty(void);
void test_cr_handshake_agreebufsizes_rw_fmap(void);
void test_cr_handshake_agreebufsizes_rw_fdr(void);
void test_cr_handshake_agreebufsizes_rw_id(void);
void test_cr_handshake_agreebufsizes_ro_wo(void);
void test_cr_handshake_propagatemessages_rw(void);
void test_cr_handshake_propagatemessages_rw_empty(void);
void test_cr_handshake_propagatemessages_rw_fmap(void);
void test_cr_handshake_propagatemessages_rw_fdr(void);
void test_cr_handshake_propagatemessages_rw_id(void);
void test_cr_handshake_propagatemessages_ro_wo(void);
void test_cr_handshake_loadandfreebuffers_rw(void);
void test_cr_handshake_loadandfreebuffers_rw_empty(void);
void test_cr_handshake_loadandfreebuffers_rw_fmap(void);
void test_cr_handshake_loadandfreebuffers_rw_fdr(void);
void test_cr_handshake_loadandfreebuffers_rw_id(void);
void test_cr_handshake_loadandfreebuffers_ro_wo(void);

#endif /*HEADER_COMMROUTINE_HANDSHAKE_H_*/
