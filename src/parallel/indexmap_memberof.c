/* $Id$ */
/*!
 * \file parallel/indexmap_memberof.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_IndexMap_MemberOf()
 * 
 */

#include "mb_parallel.h"
#include "mb_objmap.h"

/*! \brief A byte with most significant bit set */
#define MSBSET (0x80)
/*! \brief A byte with pit at \c pos set */
#define MASK(pos) (MSBSET >> pos)
/*! \brief Query of bit at \c pos within byte \c octer is set */
#define BIT_IN_BYTE_SET(octet, pos) (MASK(pos) == (octet & MASK(pos)))
/*! \brief retrieve equivalent octet from tag_table row which contains 
 * entry for \c pid */
#define TARGET_BYTE(bb,pid) (*(bb + (pid/8)))
/*! \brief determine if entry \c pid in the bit board (\c bb) is set */
#define BITBOARD_SET(bb, pid) BIT_IN_BYTE_SET(TARGET_BYTE(bb, pid),(pid%8))

 /*!
 * \brief Query the map to determine if a value exists on a particular processor
 * \ingroup MB_API
 * \param[in] im Index Map handle
 * \param[in] pid Target processor ID
 * \param[in] value Value of entry to query for
 * 
 * This is the only IndexMap routine that will be called very frequently during 
 * the simulation. We want it to be fast fast fast!
 * 
 * All checks are only done in debug more. 
 * 
 * Possible return codes:
 *  - ::MB_TRUE (\c value exists in the specified map)
 *  - ::MB_FALSE (\c value does not exist in the specified map)
 *  - ::MB_ERR_NOTREADY (map has not been synchronised using MB_IndexMap_Sync())
 *  - ::MB_ERR_INVALID (\c pid or \c im is invalid or null)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 */
int MB_IndexMap_MemberOf(MBt_IndexMap im, int pid, int value) {
    
    int rc;
    MBIt_IndexMap *map;
    tag_table *tt;
    char *bitboard;

#ifdef _EXTRA_CHECKS
    /* check if im is null */
    if (im == MB_NULL_INDEXMAP)
    {
        P_FUNCFAIL("Cannot query null index map (MB_NULL_INDEXMAP)");
        return MB_ERR_INVALID;
    }
#endif
    
    /* get reference to index map object */
    map = (MBIt_IndexMap *)MBI_getIndexMapRef(im);

#ifdef _EXTRA_CHECKS
    /* check that the map exist */
    if (map == NULL) 
    {
        P_FUNCFAIL("Invalid map handle (%d)", (int)im);
        return MB_ERR_INVALID;
    }
    /* check that it is a valid map object */
    if (map->tree_local == NULL)
    {
        P_FUNCFAIL("Corrupted map object");
        return MB_ERR_INVALID;
    }
    /* if not yet synced */
    if (map->synced == MB_FALSE)
    {
        P_FUNCFAIL("Map (%s) not yet synced", map->name);
        return MB_ERR_NOTREADY;
    }
    /* check the main tree exists */
    if (map->tree == NULL )
    {
        P_FUNCFAIL("Corrupted map object");
        return MB_ERR_INVALID;
    }
#endif
    
    /* check cache */
    if (map->cache_leaf_value == value && map->cache_leaf_ptr != NULL)
    {
        tt = (tag_table *)map->cache_leaf_ptr;
    }
    else /* cache miss */
    {
        /* retrieve bitboard from AVL tree */
        tt = (tag_table *)MBI_AVLtree_getdata(map->tree, value);
        /* node does not exist in tree */
        if (tt == NULL) return MB_FALSE;
        
        /* update cache */
        map->cache_leaf_value = value;
        map->cache_leaf_ptr   = (void *)tt;
    }
    
    /* get reference to bitboard */
    /* we used a tag_table with only one row to store the bitboard */ 
    rc = tt_getrow(tt, 0, &bitboard);
    
#ifdef _EXTRA_CHECKS
    /* check return code */
    if (rc != AVL_SUCCESS)
    {
        P_FUNCFAIL("tt_getrow() returned with err code %d", rc);
        return MB_ERR_INTERNAL;
    }
    if (bitboard == NULL)
    {
        P_FUNCFAIL("tt_getrow() returned a NULL value");
        return MB_ERR_INTERNAL;
    }
#endif

    /* magic! */
    if (BITBOARD_SET(bitboard, pid)) return MB_TRUE;
    else return MB_FALSE;
}
