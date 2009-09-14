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
    bitfield_t syncCompleted :1;
    /*! \brief flag indicating 'locked' status */
    bitfield_t locked :1;
    /*! \brief flag indicating that this board will be read by users */
    bitfield_t is_reader :1;
    /*! \brief flag indicating that this board will written to by users */
    bitfield_t is_writer :1;
    
    /*! \brief Postion indicator demarking portion of the board that 
     * has previously been synced
     */
    unsigned int synced_cursor;
    
    /*! \brief List of remote nodes that are readers of this board 
     * 
     * This array will be declared as an array of (MBI_CommSize) values
     * representing remote node ranks (ids). We only need (MBI_CommSize-1)
     * at most, but with at the cost of an extra int, we make the 
     * implementation a lot easier!
     * 
     */
    int *reader_list;
    /*! \brief Number of remote nodes that are readers of this board */
    int  reader_count;
    /*! \brief List of remote nodes that are writers of this board */
    int *writer_list;
    /*! \brief Number of remote nodes that are writers of this board 
     *
     * This array will be declared as an array of (MBI_CommSize) values
     * representing remote node ranks (ids). We only need (MBI_CommSize-1)
     * at most, but with at the cost of an extra int, we make the 
     * implementation a lot easier!
     */
    int  writer_count;
    
    /*! \brief filter function used for tagging messages we need from 
     * remote nodes */
    MBIt_filterfunc filter;
    
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

/*! \brief Data structure of an IndexMap instance */
typedef struct {
    
    /*! \brief flag indicating that the map has been synchronised */
    bitfield_t synced :1;
    
    /*! \brief map name */
    const char *name;
    
    /*! \brief AVL tree of locally added elements (pre-sync) */
    MBIt_AVLtree *tree_local;
    
    /*! \brief AVL tree of complete map space (all procs) */
    MBIt_AVLtree *tree;

    /*! \brief Pointer to cached leaf (most recently accessed) */
    void *cache_leaf_ptr;
    /*! \brief Value cached leaf (most recently accessed) */
    int cache_leaf_value;
    
} MBIt_IndexMap;

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
#define MBI_TAG_MSGDATA  (0x1000)
#define MBI_TAG_PROPDATA  (0x2000)
/* max MBI_TAG_*DATA = (0x6000) */
/* (0x7000 - 0x7fff) reserved for individually set tags */
#define MBI_TAG_INDEXMAX_SYNC   (0x7000)


/*! \brief bitmask for communication buffer header byte: full data replication
 * 
 * The first byte of a communication buffer (for propagating messages) is
 * reserved for header information.
 * 
 * This first bit is used to indicate if filtering was not performed by the
 * sender (fallback to full data replication) and the filtering has to be
 * performed by the recipient.
 * 
 * */
#define MBI_COMM_HEADERBYTE_FDR (0x01)


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
