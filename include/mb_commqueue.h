/* $Id$ */
/*!
 * \file mb_commqueue.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Header file for Communication Queue
 * 
 */

#ifndef MB_COMMQUEUE_H
#define MB_COMMQUEUE_H

#ifdef HAVE_CONFIG_H
#include "mb_config.h"
#endif

#include "mboard.h"
#include "uthash.h"

/* conditionally include MPI library so CommQueue can be
 * testing without compiling with mpicc
 */
#include <mpi.h>

/* \brief Temp int array used by comm routines */
extern int *MBI_comm_indices;

/*! \brief Used to define the different communication stages
 * a board synchronisation process can be in
 */
enum MBIt_CommStage {
    /*! \brief Message board requires tagging */
    PRE_TAGGING,
    /*! \brief Function parameters are being propagated */
    TAGINFO_SENT,
    /*! \brief Message board ready for tagging */
    TAGGING,
    /*! \brief Messages ready for propagation */
    PRE_PROPAGATION,
    /*! \brief Messages are being propagated */
    PROPAGATION
};

/*! \brief datatype used as node for CommQueue hashtable */
struct MBIt_commqueue {
    
    /*! \brief Message Board handle. Also used for indexing hashtable */
    MBt_Board mb; 

    /*! \brief Number of pending receives */
    unsigned int pending_in;
    /*! \brief Number of pending sends */
    unsigned int pending_out;
    
    /*! \brief Array of number of messages to expect from remote boards */
    int *incount;
    
    /*! \brief Array of input buffers */
    void **inbuf;
    /*! \brief Array of output buffers */
    void **outbuf;
    /*! \brief Array of send requests */
    MPI_Request *sendreq;
    /*! \brief Array of receive requests */
    MPI_Request *recvreq;
    /*! \brief Stage in which communication process is in */
    enum MBIt_CommStage stage;

    /*! \brief metadata required by \c uthash */
    UT_hash_handle hh;
};


/* Communication Queue management */
/* ... see util_commqueue.c ... */
int MBI_CommQueue_isEmpty(void);
int MBI_CommQueue_Init(void);
int MBI_CommQueue_Delete(void);
int MBI_CommQueue_Pop(MBt_Board mb);
struct MBIt_commqueue* MBI_CommQueue_GetFirstNode(void);
int MBI_CommQueue_Push(MBt_Board mb, enum MBIt_CommStage startstage);

/* ... see comm.c ... */
int MBIt_Comm_InitTagging(struct MBIt_commqueue *node);
int MBIt_Comm_WaitTagInfo(struct MBIt_commqueue *node);
int MBIt_Comm_TagMessages(struct MBIt_commqueue *node);
int MBIt_Comm_InitPropagation(struct MBIt_commqueue *node);
int MBIt_Comm_CompletePropagation(struct MBIt_commqueue *node);

#endif /*MB_COMMQUEUE_H*/
