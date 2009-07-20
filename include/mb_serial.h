/* $Id$ */
/*!
 * \file mb_serial.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Header file used by serial implementation
 * 
 * Uncomment the definition of \c MB_CONFIG_RECYCLE_MEMPOOL in this file 
 * to use pl_recycle() instead of pl_reset() when clearing MessageBoards 
 * with MB_Clear()
 * 
 */
#ifndef MB_SERIAL_H_
#define MB_SERIAL_H_

#include "mboard.h"
#include "mb_common.h"
#include "mb_utils.h"

/*! \brief Data structure of a MessageBoard instance */
typedef struct {
    /*! \brief flag indicating 'locked' status */
    unsigned int locked :1;
    
    /*! \brief pooled-list to hold messages */
    pooled_list *data;
} MBIt_Board;

/*! \brief Data structure of an IndexMap instance */
typedef struct {
    
    /*! \brief map name */
    const char *name;
    
    /*! \brief AVL tree of elements */
    MBIt_AVLtree *tree;

} MBIt_IndexMap;

/* Uncomment to use pl_recycle() instead of pl_reset() */
/* #define MB_CONFIG_RECYCLE_MEMPOOL */

#endif /*MB_SERIAL_H_*/
