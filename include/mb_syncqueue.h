/* $Id$ */
/*!
 * \file mb_syncqueue.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Header file for Sync Request Queue
 * 
 */

#ifndef MB_SYNCQUEUE_H
#define MB_SYNCQUEUE_H

#ifdef HAVE_CONFIG_H
#include "mb_config.h"
#endif

#include "mboard.h"
#include <pthread.h>

/* Sync Request queue management */
/* ... see src/parallel/util_syncqueue.c ... */
extern pthread_mutex_t MBI_SRQLock;
extern pthread_cond_t  MBI_SRQCond;
int MBI_SyncQueue_isEmpty(void);
int MBI_SyncQueue_Init(void);
int MBI_SyncQueue_Delete(void);
int MBI_SyncQueue_Push(MBt_Board mb);
int MBI_SyncQueue_Pop(MBt_Board *mb);

#endif /*MB_SYNCQUEUE_H*/
