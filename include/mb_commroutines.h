/* $Id$ */
/*!
 * \file mb_commroutines.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Aug 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Header file for Communication Routines
 * 
 * Every communication strategy is made up of several communication routines
 * Each communication routine represents the transition function of each stage 
 * (stage) of the lifecycle of the comm node.
 * 
 * Each stage is given a unique stage id (see ::MBIt_CommStage) and this id is
 * as a reference to the appropriate transition function (see ::MBI_trans_func).
 * 
 * Different communication strategies can be chosen by setting the appropriate
 * initial stage of new comm nodes. The main reason is to simply the code in 
 * commthread.c. However, it also gives us the opportunity to apply different
 * strategies to different message boards (if we see a need to do so).
 * 
 * Only non-blocking MPI routines are used so the transition functions are
 * never blocked by MPI calls. Furthermore, only synchronous (and non-
 * blocking) sends -- Issend() -- are used to avoid additional buffering by
 * the MPI library.
 * 
 */
#ifndef MB_COMMROUTINES_H
#define MB_COMMROUTINES_H

#ifdef HAVE_CONFIG_H
#include "mb_config.h"
#endif
#include "mb_parallel.h"
#include "mb_commqueue.h"

/*! \brief IDs to enumerate the communication routines that make up the
 * different communication strategies.
 * 
 * The different strategies are:
 *  - Old comm routines (OLD): Comm routine pre 0.2.1. Similar to STAGED
 *    but does not consider board access modes.
 *  - Staged Issends (STAGED): ... more info ...
 */
enum MBIt_CommStage {
    
    /*!\brief (HANDSHAKE) Preparing for message propagation */
    MB_COMM_HANDSHAKE_PRE_PROP,
    /*!\brief (HANDSHAKE) Initialising message propagation */
    MB_COMM_HANDSHAKE_START_PROP,
    /*!\brief (HANDSHAKE) Finalising message propagation */
    MB_COMM_HANDSHAKE_COMPLETE_PROP,
    
    /*!\brief (OLD) Message board requires tagging */
    MB_COMM_OLD_PRE_TAGGING, 
    /*!\brief (OLD) Board is ready to be synchronised */
    MB_COMM_OLD_READY_FOR_PROP,
    /*!\brief (OLD) Buffer requirements are being transmitted */
    MB_COMM_OLD_BUFINFO_SENT,
    /*!\brief (OLD) Messages ready for propagation */
    MB_COMM_OLD_PRE_PROPAGATION,
    /*!\brief (OLD) Messages are being propagated */
    MB_COMM_OLD_PROPAGATION,

    /*!\brief Number of stages that needs mapping to transition functions */
    MB_COMM_STAGES_TOTAL,
    /*!\brief Communication process completed*/
    MB_COMM_END
};

/*!\brief alias of a pointer to a comm routine (transition function) */
typedef int (*MBIt_commroutine)(struct MBIt_commqueue*);

/*!\brief map of stage ID to transition function */
extern MBIt_commroutine MBI_trans_func[MB_COMM_STAGES_TOTAL];

/*!\brief Assign transition functions to each stage
 * 
 * Declare as a macro so they can be define in the same place (this header
 * file) as the definition of stages in ::MBIt_CommStage.
 */
#define MBI_initialise_commroutine_mapping() \
    MBI_trans_func[MB_COMM_HANDSHAKE_PRE_PROP]      = &MBI_CommRoutine_HANDSHAKE_AgreeBufSizes; \
    MBI_trans_func[MB_COMM_HANDSHAKE_START_PROP]    = &MBI_CommRoutine_HANDSHAKE_PropagateMessages; \
    MBI_trans_func[MB_COMM_HANDSHAKE_COMPLETE_PROP] = &MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers; \
    \
    MBI_trans_func[MB_COMM_OLD_PRE_TAGGING]     = &MBI_CommRoutine_OLD_TagMessages; \
    MBI_trans_func[MB_COMM_OLD_READY_FOR_PROP]  = &MBI_CommRoutine_OLD_SendBufInfo; \
    MBI_trans_func[MB_COMM_OLD_BUFINFO_SENT]    = &MBI_CommRoutine_OLD_WaitBufInfo; \
    MBI_trans_func[MB_COMM_OLD_PRE_PROPAGATION] = &MBI_CommRoutine_OLD_InitPropagation; \
    MBI_trans_func[MB_COMM_OLD_PROPAGATION]     = &MBI_CommRoutine_OLD_CompletePropagation; 

/* ... see src/parallel/comm_routines_HANDSHAKE.c ... */
int MBI_CommRoutine_HANDSHAKE_AgreeBufSizes(struct MBIt_commqueue *node);
int MBI_CommRoutine_HANDSHAKE_PropagateMessages(struct MBIt_commqueue *node);
int MBI_CommRoutine_HANDSHAKE_LoadAndFreeBuffers(struct MBIt_commqueue *node);

/* ... see src/parallel/comm_routines_OLD.c ... */
int MBI_CommRoutine_OLD_TagMessages(struct MBIt_commqueue *node);
int MBI_CommRoutine_OLD_SendBufInfo(struct MBIt_commqueue *node);
int MBI_CommRoutine_OLD_WaitBufInfo(struct MBIt_commqueue *node);
int MBI_CommRoutine_OLD_InitPropagation(struct MBIt_commqueue *node);
int MBI_CommRoutine_OLD_CompletePropagation(struct MBIt_commqueue *node);

/* comm utilities */
int MBI_CommUtil_UpdateCursor(MBIt_Board *board);
int MBI_CommUtil_TagMessages(MBIt_Board *board, int *outcount);
int MBI_CommUtil_LoadBuffer(MBIt_Board *board, void *buf, size_t size);
int MBI_CommUtil_BuildBuffers_Tagged(MBIt_Board *board, void **bufarray, 
                                     int *outcount);
int MBI_CommUtil_BuildBuffer_All(MBIt_Board *board, void **bufptr, 
                                 unsigned int flag_fdr, size_t *size);

#endif /*MB_COMMQUEUE_H*/
