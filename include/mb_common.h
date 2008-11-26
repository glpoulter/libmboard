/* $Id$ */
/*!
 * \file mb_common.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Header file common to both serial and parallel implementation
 * 
 */

#ifndef MB_COMMON_H_
#define MB_COMMON_H_

#ifdef HAVE_CONFIG_H
#include "mb_config.h"
#endif

#ifdef _LOG_MEMORY_USAGE
#include "mb_memlog.h"
#endif

#include "mb_objmap.h"
#include "mb_pooled_list.h"
#include <assert.h>
#include <stdlib.h>

/* status variables (initialised and documented in env_init.c) */
extern int MBI_STATUS_initialised;
extern int MBI_STATUS_finalised;

/*! \brief Number of elements per memory block (for serial lib)
 * 
 * This value is passed as \c pool_size to pl_create() when 
 * MessageBoards are created using MB_Create() .
 * 
 * This value (halved) is also used when creating an Filtered
 * Iterator (MB_Iterator_CreateFiltered()).
 */
#define MB_CONFIG_SERIAL_POOLSIZE    256

/*! \brief Number of elements per memory block (for parallel lib)
 * 
 * This value is passed as \c pool_size to pl_create() when 
 * MessageBoards are created using MB_Create() .
 * 
 * This value (halved) is also used when creating an Filtered
 * Iterator (MB_Iterator_CreateFiltered()).
 */
#define MB_CONFIG_PARALLEL_POOLSIZE  512

/* global variables (initialised and documented in env_init.c) */
extern int MBI_CommRank;
extern int MBI_CommSize;

/*! \brief Data structure of an Iterator instance */
typedef struct {
    /*! \brief flag to indicate if iteration has started */
    unsigned int iterating :1; 
    /*! \brief size of message being referenced */
    int msgsize; 
    /*! \brief handle to corresponding MessageBoard */
    MBt_Board mb;  

    /*! \brief pooled-list to hold MBIt_addrnode which references messages */
    pooled_list *data;
    /*! \brief pointer to current address node */
    pl_address_node *cursor;

} MBIt_Iterator;

/*! \brief Function pointer to used-defined filter function */
typedef int (*MBIt_filterfunc)(const void *, const void *);

/*! \brief Wrapper for pointer to filter function 
 * 
 * This is needed so the function pointer can be placed into 
 * the ObjectMap. Using the filter pointer directly is not possible
 * as ISO C forbids conversion of function pointers to object 
 * pointers (and vice versa).
 * 
 * */
typedef struct {
    /*! \brief Function pointer to used-defined filter function */
    MBIt_filterfunc func;
} MBIt_filterfunc_wrapper;

/* ---- Object Maps ----- */
/*! \brief Constant representing Object Type: MessageBoard */
#define OM_TYPE_MBOARD    (0x0a01)
/*! \brief Constant representing Object Type: Iterator */
#define OM_TYPE_ITERATOR  (0x0a02)
/*! \brief Constant representing Object Type: Function */
#define OM_TYPE_FUNCTION  (0x0a03)

/* (initialised and documented in env_init.c) */
extern MBIt_objmap *MBI_OM_mboard;
extern MBIt_objmap *MBI_OM_iterator;
extern MBIt_objmap *MBI_OM_function;

#endif /*MB_COMMON_H_*/
