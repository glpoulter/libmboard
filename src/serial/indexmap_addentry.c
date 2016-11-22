/* $Id: indexmap_addentry.c 1917 2009-06-29 16:44:44Z lsc $ */
/*!
 * \file serial/indexmap_addentry.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_IndexMap_AddEntry()
 * 
 */

#include "mb_serial.h"
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
    if (im == MB_NULL_INDEXMAP) 
    {
        P_FUNCFAIL("Cannot add entry to null map (MB_NULL_INDEXMAP)");
        return MB_ERR_INVALID;
    }
    
    /* get reference to index map object */
    map = (MBIt_IndexMap *)MBI_getIndexMapRef(im);
    if (map == NULL) 
    {
        P_FUNCFAIL("Invalid index map handle (%d)", (int)im);
        return MB_ERR_INVALID;
    }
    assert(map->tree != NULL);
    
    /* add value to map */
    rc = MBI_AVLtree_insert(map->tree, value, NULL);
    if (rc != AVL_SUCCESS)
    {
        if (rc == AVL_ERR_MEMALLOC) 
        {
            P_FUNCFAIL("Could not allocate required memory");
            return MB_ERR_MEMALLOC;
        }
        else 
        {
            P_FUNCFAIL("MBI_AVLtree_insert() returned with err code %d", rc);
            return MB_ERR_INTERNAL;
        }
    }
    
	return MB_SUCCESS;
}
