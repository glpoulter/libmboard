/* $Id$ */
/*!
 * \file serial/indexmap_create.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_IndexMap_Create()
 * 
 */

#include "mb_serial.h"
#include "mb_objmap.h"


#define __name_exists(name) MBI_stringmap_Contains(MBI_indexmap_nametable,name)
#define __name_add(name)   MBI_stringmap_AddString(MBI_indexmap_nametable,name)

/*!
 * \brief Instantiates a new Index Map object
 * \ingroup MB_API
 * \param[in] name Unique string identifying the map (max of 127 chars)
 * \param[out] im_ptr Address of Index Map handle
 * 
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_OVERFLOW (too many maps created)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 *  - ::MB_ERR_DUPLICATE (\c name already exists)
 *  - ::MB_ERR_INVALID (\c name is NULL)
 *  - ::MB_ERR_OVERFLOW (\c name is too long)
 *  - ::MB_ERR_ENV (Message Board environment not yet initialised)
 */
int MB_IndexMap_Create(MBt_IndexMap *im_ptr, const char *name) {
    
    MBIt_IndexMap *im_obj;
    MBIt_AVLtree *tree;
    OM_key_t rc_om;
    
    /* set im to NULL in case return under error conditions */ 
	*im_ptr = MB_NULL_INDEXMAP;
	
    /* make sure MB environment has been set up */
    assert(MB_Env_Initialised() == MB_SUCCESS);
    if (MB_Env_Initialised() != MB_SUCCESS) return MB_ERR_ENV;
    
    /* make sure name is acceptable */
    if (name == NULL) return MB_ERR_INVALID;
    if (strlen(name) == 0) return MB_ERR_INVALID;
    if (strlen(name) > MB_INDEXMAP_NAMELENGTH) return MB_ERR_OVERFLOW;
    
    /* make sure name is not already used */
    assert(MBI_indexmap_nametable != NULL);
    if (__name_exists(name)) return MB_ERR_DUPLICATE;
   
	/* allocate memory for index map object */
	im_obj = (MBIt_IndexMap *)malloc(sizeof(MBIt_IndexMap));
	assert(im_obj != NULL);
	if (im_obj == NULL)
	{
	    return MB_ERR_MEMALLOC;
	}
	
	/* create an avl tree object */
	tree = MBI_AVLtree_create();
	assert(tree != NULL);
	if (tree == NULL)
    {
        return MB_ERR_INTERNAL;
    }
	
	/* assign tree to im obj */
	im_obj->tree = tree;
	
	/* register object */
    assert(MBI_OM_indexmap != NULL);
    assert(MBI_OM_indexmap->type == OM_TYPE_INDEXMAP);
    rc_om = MBI_objmap_push(MBI_OM_indexmap, (void*)im_obj);
    if (rc_om > OM_MAX_INDEX)
    {
        if (rc_om == OM_ERR_MEMALLOC)
        {
            return MB_ERR_MEMALLOC;
        }
        else
        {
            return MB_ERR_INTERNAL;
        }
    }
    
    /* add name to indexmap name table */
    __name_add(name);
    
    /* add name to object */
    im_obj->name = name;
    
    /* return handle via ptr */
    *im_ptr = (MBt_IndexMap)rc_om;
    
    return MB_SUCCESS;
}
