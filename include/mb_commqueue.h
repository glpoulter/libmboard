/* $Id$ */
/*!
 * \file mb_commqueue.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
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

#include "mb_parallel.h"
#include <mpi.h>

/* \brief Temporary int array used by comm routines 
 * 
 * Declaring this as a global pointer means that access to it is not 
 * thread-safe and should only be read/written to by one thread (comm thread).
 * 
 * */
extern int *MBI_comm_indices;

/*! \brief Used to define the different communication stages
 * a board synchronisation process can be in
 */
enum MBIt_CommStage {
    /*! \brief Message board requires tagging */
    PRE_TAGGING,
    /*! \brief Board is ready to be synchronised */
    READY_FOR_PROP,
    /*! \brief Buffer requirements are being transmitted */
    BUFINFO_SENT,
    /*! \brief Messages ready for propagation */
    PRE_PROPAGATION,
    /*! \brief Messages are being propagated */
    PROPAGATION,
    /*! \brief Communication process completed */
    COMM_END
};

/*! \brief datatype used as node for CommQueue hashtable */
struct MBIt_commqueue {
    
    /*! \brief Flag to indicate fallback to full data replication 
     * 
     * Filtering of messages has to be done by recipient
     */
    unsigned int flag_fdrFallback :1;
    unsigned int flag_shareOutbuf :1;
    
    /*! \brief Message Board handle */
    MBt_Board mb; 
    /*! \brief Cache of pointer to board object */
    MBIt_Board *board;

    /*! \brief Number of pending receives */
    unsigned int pending_in;
    /*! \brief Number of pending sends */
    unsigned int pending_out;
    
    /*! \brief Array of number of messages to expect from remote boards */
    int *incount;
    /*! \brief Array of number of messages to send to remote boards */
    int *outcount;
    
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
    
    /*! \brief Pointer to next node in list */
    struct MBIt_commqueue* next;
    /*! \brief Pointer to previous node in list */
    struct MBIt_commqueue* prev;
};


/* Communication Queue management */
/* ... see src/parallel/util_commqueue.c ... */
int MBI_CommQueue_isEmpty(void);
int MBI_CommQueue_Init(void);
int MBI_CommQueue_Delete(void);
int MBI_CommQueue_Pop(struct MBIt_commqueue *);
struct MBIt_commqueue* MBI_CommQueue_GetFirstNode(void);
int MBI_CommQueue_Push(MBt_Board mb, enum MBIt_CommStage startstage);

/* ... see src/parallel/comm.c ... */
int MBI_Comm_TagMessages(struct MBIt_commqueue *node);
int MBI_Comm_SendBufInfo(struct MBIt_commqueue *node);
int MBI_Comm_WaitBufInfo(struct MBIt_commqueue *node);
int MBI_Comm_InitPropagation(struct MBIt_commqueue *node);
int MBI_Comm_CompletePropagation(struct MBIt_commqueue *node);

#endif /*MB_COMMQUEUE_H*/
