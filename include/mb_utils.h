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
 * In DEBUG mode, unmap routines are replaced by a wrapper routine that
 * performs the necessary checks
 * 
 * In PRODUCTION mode, unmap routines are translated directly to
 * MBI_objmap_getobj()
 * 
 */
#ifndef MB_UTILS_H_
#define MB_UTILS_H_

#include "mboard.h"
#include "mb_common.h"
#include "mb_objmap.h"
#include <assert.h>

/* .... see src/utils/unmap.c .... */
/* Dereference Mboard Handle */
void * MBI_getMBoardRef_withasserts(MBt_Board mb);
/* Dereference Iterator Handle */
void * MBI_getIteratorRef_withasserts(MBt_Iterator iter);
/* Dereference Filter Handle */
void * MBI_getFilterRef_withasserts(MBt_Filter fh);
/* Dereference IndexMap Handle */
void * MBI_getIndexMapRef_withasserts(MBt_IndexMap ih);
/* Dereference SearchTree Handle */
void * MBI_getSearchTreeRef_withasserts(MBt_IndexMap sh);

#ifdef _EXTRA_CHECKS /* DEBUG mode */

#define MBI_getMBoardRef(mb)     MBI_getMBoardRef_withasserts(mb)
#define MBI_getIteratorRef(iter) MBI_getIteratorRef_withasserts(iter)
#define MBI_getFilterRef(ft)     MBI_getFilterRef_withasserts(ft)
#define MBI_getIndexMapRef(ih)   MBI_getIndexMapRef_withasserts(ih)
#define MBI_getSearchTreeRef(sh)   MBI_getSearchTreeRef_withasserts(sh)

#else /* PRODUCTION mode */

/*! \brief maps MessageBoard handle to object reference  */
#define MBI_getMBoardRef(mb)     MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb)
/*! \brief maps Iterator handle to object reference  */
#define MBI_getIteratorRef(iter) MBI_objmap_getobj(MBI_OM_iterator, (OM_key_t)iter)
/*! \brief maps Filter handle to object reference  */
#define MBI_getFilterRef(ft)     MBI_objmap_getobj(MBI_OM_filter, (OM_key_t)ft)
/*! \brief maps IndexMap handle to object reference  */
#define MBI_getIndexMapRef(ih)   MBI_objmap_getobj(MBI_OM_indexmap, (OM_key_t)ih)
/*! \brief maps SearchTree handle to object reference  */
#define MBI_getSearchTreeRef(sh)   MBI_objmap_getobj(MBI_OM_searchtree, (OM_key_t)sh)
#endif /* _EXTRA_CHECKS */

#endif /*MB_UTILS_H_*/
