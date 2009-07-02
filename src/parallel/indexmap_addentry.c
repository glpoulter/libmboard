/* $Id$ */
/*!
 * \file parallel/indexmap_addentry.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_IndexMap_AddEntry()
 * 
 */

#include "mb_parallel.h"
#include "mb_objmap.h"

/*!
 * \brief Adds an entry into the Index Map
 * \ingroup MB_API
 * \param[in] im Index Map handle
 * \param[in] value Integer value of entry to add into the map
 * 
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c im is null or invalid)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 */
int MB_IndexMap_AddEntry(MBt_IndexMap im, int value) {
    
    int rc;
    MBIt_IndexMap *map;
    
    /* check if im is null */
    if (im == MB_NULL_INDEXMAP) return MB_ERR_INVALID;
    
    /* get reference to index map object */
    map = (MBIt_IndexMap *)MBI_getIndexMapRef(im);
    if (map == NULL) return MB_ERR_INVALID;
    assert(map->tree_local != NULL);
    
    /* add value to map */
    rc = MBI_AVLtree_insert(map->tree_local, value, NULL);
    if (rc != AVL_SUCCESS)
    {
        if (rc == AVL_ERR_MEMALLOC) return MB_ERR_MEMALLOC;
        else return MB_ERR_INTERNAL;
    }
    
    return MB_SUCCESS;
}
