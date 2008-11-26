/* $Id$ */
/*!
 * \file mb_parallel.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Header file used by parallel implementation
 * 
 */

#ifndef MB_PARALLEL_H_
#define MB_PARALLEL_H_

#include "mboard.h"
#include "mb_utils.h"
#include "mb_common.h"
#include "mb_tag_table.h"

/* we require MPI and pthreads support */
#include <mpi.h>
#include <pthread.h>

/*! \brief Shortcut to enable checks for masternode as 
 * <tt>if MASTERNODE; do_something()</tt>
 */
#define MASTERNODE (0 == MBI_CommRank)

/*! \brief Rename TagTable Data structure to fit our naming scheme  */
typedef tag_table MBIt_TagTable;

/*! \brief Data structure of a MessageBoard instance  */
typedef struct {
    /*! \brief flag indicating synchronisation completion status 
     * 
     * Only used in parallel version, and when \c locked = ::MB_TRUE
     */
    unsigned int syncCompleted :1;
    /*! \brief flag indicating 'locked' status */
    unsigned int locked :1;
    /*! \brief flag indicating if messages were tagged */
    unsigned int tagging :1;
    
    /*! \brief filter function used for tagging messages we need from 
     * remote nodes */
    MBt_Function fh;
    /* \brief parameters to be passed to fh */
    void * fparams;
    /* \brief size of \c fparams data to be passed to fh */
    size_t fparams_size;
    
    /*! \brief table for managing message tagging */
    MBIt_TagTable *tt;
    
    /*! \brief mutex lock used when checking sync completion status */
    pthread_mutex_t syncLock;
    /*! \brief thread condition variable for main thread to 
     * wait on sync completion
     */
    pthread_cond_t  syncCond;
   
    /*! \brief pooled-list to hold messages */
    pooled_list *data;
    
} MBIt_Board;

/* constants for calculating MPI tags (for labelling communication) */
/* LAM MPI supports a rather small MAX TAG value, and does not have the 
 * courtesy to set the MPI_TAG_UB. Boooo!
 * Let's be conservative with our values...
 */
/* We use up to 15 bits. First 12 bits for boards (so we support 4092
 * boards), and the remaining 3 for communication type (max 8)
 */ 
#define MBI_TAG_MAX      (0x7fff)
#define MBI_TAG_BASE     (0x0fff)
#define MBI_TAG_FHDATA   (0x1000)
#define MBI_TAG_MSGDATA  (0x2000)
/* max MBI_TAG_*DATA = (0x7000) */

/*The number of available simulataneous comms limits the number of 
 * boards we can have
 */
/*! \brief Maximum number of boards we can support */
#define MBI_MAX_BOARDS MBI_TAG_MAX

/* global variables (initialised and documented in src/parallel/env_init.c) */
extern MPI_Comm MBI_CommWorld;

/* Communication thread routines */
/* ... see src/parallel/commthread.c ... */
int MBI_CommThread_Init(void);
int MBI_CommThread_Finalise(void);

#endif /*MB_PARALLEL_H_*/
