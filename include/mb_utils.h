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


/* ------ Routines internal to libmboard ----- */
/* .... see src/utils/unmap.c .... */

/* Dereference Mboard Handle */
extern inline void * MBI_getMBoardRef(MBt_Board mb);

/* Dereference Iterator Handle */
extern inline void * MBI_getIteratorRef(MBt_Iterator iter);

/* Dereference Filter Handle */
extern inline void * MBI_getFilterRef(MBt_Filter fh);

/* Dereference Function Handle */
extern inline void * MBI_getIndexMapRef(MBt_IndexMap ih);

/* .... see src/utils/banner.c .... */
extern void MBI_print_banner(void);

#endif /*MB_UTILS_H_*/
