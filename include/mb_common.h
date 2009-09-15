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

#include "mb_reporting.h"
#include "mb_objmap.h"
#include "mb_pooled_list.h"
#include "mb_string_map.h"
#include "mb_avltree.h"
#include "mb_utils.h"
#include "mb_settings.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*! \brief macro to mask "unused parameters" warning for acknowledged cases */
#define ACKNOWLEDGED_UNUSED(expr) do { (void)(expr); } while (0)

/* status variables (initialised and documented in env_init.c) */
extern int MBI_STATUS_initialised;
extern int MBI_STATUS_finalised;

/*! \brief String lenght of Index Map name */
/* NOTE: IF YOU UPDATE THIS, PLEASE ALSO UPDATE DOXYGEN COMMENTS
 * FOR MB_IndexMap_Create() IN mboard.h
 * -- set all instantances of 'maximum character lenght" to 
 *    that of (MB_INDEXMAP_NAMELENGTH - 1).
 */
#define MB_INDEXMAP_NAMELENGTH 128

/*! \brief string map to names used for creating index maps */
extern MBIt_stringmap *MBI_indexmap_nametable;

/* global variables (initialised and documented in env_init.c) */
extern int MBI_CommRank;
extern int MBI_CommSize;

#ifndef BITFIELD_T_DEFINED
#define BITFIELD_T_DEFINED
/*! \brief Dummy type used to indicate that a specific struct member
 * will be used as a bit field
 */
typedef unsigned int bitfield_t;
#endif

/*! \brief Data structure of an Iterator instance 
 * 
 * This data structure is place in mb_common.h as both the serial and parallel
 * versions use the same definition.
 */
typedef struct {
	
    /*! \brief flag to indicate if iteration has started */
    bitfield_t iterating :1; 
    
    /*! \brief size of message being referenced */
    unsigned int msgsize; 
    
    /*! \brief handle to corresponding MessageBoard */
    MBt_Board mb;  

    /*! \brief pooled-list to hold MBIt_addrnode which references messages */
    pooled_list *data;
    
    /*! \brief pointer to current address node */
    pl_address_node *cursor;

} MBIt_Iterator;

/*! \brief Function pointer to user-defined filter function 
 * 
 * Used as a shortcut for passing function references as arguments to other 
 * functions.
 * 
 * The function that this pointer refers to is takes in <tt>const void *</tt> for
 * both its arguments, and returns <tt>int</tt>.
 * 
 * */
typedef int (*MBIt_filterfunc)(const void *, int);

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
/*! \brief Constant representing Object Type: Filter */
#define OM_TYPE_FILTER    (0x0a03)
/*! \brief Constant representing Object Type: IndexMap */
#define OM_TYPE_INDEXMAP  (0x0a04)

/* (initialised and documented in env_init.c) */
extern MBIt_objmap *MBI_OM_mboard;
extern MBIt_objmap *MBI_OM_iterator;
extern MBIt_objmap *MBI_OM_filter;
extern MBIt_objmap *MBI_OM_indexmap;

#endif /*MB_COMMON_H_*/
