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
 */
#ifndef MB_SERIAL_H_
#define MB_SERIAL_H_

#include "mboard.h"
#include "mb_common.h"

/*! \brief Data structure of a MessageBoard instance */
typedef struct {
    /*! \brief flag indicating 'locked' status */
    bitfield_t locked :1;
    
    /*! \brief flag indicating that this board will be read by users */
    bitfield_t is_reader :1;
    /*! \brief flag indicating that this board will written to by users */
    bitfield_t is_writer :1;
    
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

#endif /*MB_SERIAL_H_*/
