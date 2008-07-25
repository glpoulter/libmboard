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

#include <mpi.h>
#include <pthread.h>

#define MASTERNODE (0 == MBI_CommRank)

/* uncomment to skip expensive checks. */
/* #define MB_CONFIG_NOCHECKS */

/*! \brief Data structure for managing message tagging  */
typedef struct {
    /*! \brief number of MPI Tasks */
    int pcount; 
    /*! \brief number of messages in board */
    int mcount; 
    /*! \brief Tagging status store as binary table 
     * 
     * \c tt[msg] will store a list of binary values representing
     * tags per MPI task. Bit X set to '1' if message tagged for 
     * process X, '0' otherwise.
     * 
     * List of binary values will be access in blocks of 1 byte:
     * tt[msg][block] = 0x00000000 
     */
    void *tt; 
    /*! \brief Array storing number of messages tagged per proc */ 
    int *tagged;
    
} MBIt_TagTable;

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
#define MBI_TAG_BASE     (0x01000000)
#define MBI_TAG_FHDATA   (0x01000000)
#define MBI_TAG_MSGDATA  (0x02000000)

/* datatypes for storing params for pending creates */
typedef struct t_MBIt_pendingCreate_node {
    
    /* params for MB_Create */
    size_t     msgsize;
    MBt_Board *mb_ptr;
    
    /* pointer to next node */
    struct t_MBIt_pendingCreate_node *next;
} MBIt_pendingCreate_node;

/* global variables (initialised and documented in env_init.c) */
extern MPI_Comm MBI_CommWorld;
extern int MBI_CommRank;
extern int MBI_CommSize;

/* Communication thread routines */
/* ... see commthread.c ... */
int MBI_CommThread_Init(void);
int MBI_CommThread_Finalise(void);

#endif /*MB_PARALLEL_H_*/
