/* $Id: indexmap_memberof.c 2113 2009-09-15 14:42:01Z lsc $ */
/*!
 * \file serial/indexmap_memberof.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_IndexMap_MemberOf()
 * 
 */

#include "mb_serial.h"
#include "mb_objmap.h"

/*!
 * \brief Query the map to determine if a value exists on a particular processor
 * \ingroup MB_API
 * \param[in] im Index Map handle
 * \param[in] pid Target processor ID
 * \param[in] value Value of entry to query for
 * 
 * 
 * Possible return codes:
 *  - ::MB_TRUE (\c value exists in the specified map)
 *  - ::MB_FALSE (\c value does not exist in the specified map)
 *  - ::MB_ERR_INVALID (\c pid or \c im is invalid or null)
 */
int MB_IndexMap_MemberOf(MBt_IndexMap im, int pid, int value) {
    
    MBIt_IndexMap *map;
    
    /* acknowledge+hide "unused parameter" compiler warnings */
    ACKNOWLEDGED_UNUSED(pid);
    
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
    if (map == NULL) 
    {
        P_FUNCFAIL("Invalid map handle (%d)", (int)im);
        return MB_ERR_INVALID;
    }
    assert(map->tree != NULL);
    
    /* query AVL tree */
    if (MBI_AVLtree_memberof(map->tree, value)) return MB_TRUE;
    else return MB_FALSE;
}
