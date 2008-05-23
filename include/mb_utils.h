/* $Id$ */
/*!
 * \file mb_utils.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Header file for utility routines (used by both serial and
 *        parallel implementations)
 * 
 */

#ifndef MB_UTILS_H_
#define MB_UTILS_H_

#include "mb_common.h"

/* ----- Internal datatypes ----- */


/* ------ Routines internal to libmboard ----- */
/* from unmap.c */

/* Dereference Mboard Handle */
void * MBI_getMBoardRef(MBt_Board mb);

/* Dereference Iterator Handle */
void * MBI_getIteratorRef(MBt_Iterator iter);

/* Dereference Function Handle */
void * MBI_getFunctionRef(MBt_Function fh);

#endif /*MB_UTILS_H_*/
