/* $Id$ */
/*!
 * \file parallel/indexmap_delete.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_IndexMap_Delete()
 * 
 */

#include "mb_parallel.h"
#include "mb_objmap.h"

static int _delete_bitboard_data(MBIt_AVLnode *node);

/*!
 * \brief Deletes an Index Map
 * \ingroup MB_API
 * \param[in,out] im_ptr Address of Index Map handle
 * 
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c im is invalid)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 */
int MB_IndexMap_Delete(MBt_IndexMap *im_ptr) {
    
    int rc;
    void *obj;
    MBIt_IndexMap *im_obj;
    ACKNOWLEDGED_UNUSED(obj); /* only used for assertions */
    
    /* check for null boards */
    if (*im_ptr == MB_NULL_INDEXMAP)
    {
        P_WARNING("Deletion of null index map (MB_NULL_INDEXMAP)");
        return MB_SUCCESS;
    }
    
    /* get reference to indexmap object */
    im_obj = (MBIt_IndexMap *)MBI_getIndexMapRef(*im_ptr);
    if (im_obj == NULL) 
    {
        P_FUNCFAIL("Invalid map handle (%d)", (int)*im_ptr);
        return MB_ERR_INVALID;
    }
    #ifdef _EXTRA_CHECKS
    if (im_obj->synced == MB_TRUE) 
    {
        assert(im_obj->tree != NULL);
    }
    #endif
    
    /* free up name for other maps */
    rc = MBI_stringmap_RemoveString(MBI_indexmap_nametable, im_obj->name);
    if (rc != MB_SUCCESS) 
    {
        P_FUNCFAIL("MBI_stringmap_RemoveString() returned with err code %d", rc);
        return MB_ERR_INTERNAL;
    }
    
    P_INFO("Deleting index map (%d) '%s'", (int)*im_ptr, im_obj->name);
    
    /* Delete object from map*/
    obj = MBI_objmap_pop(MBI_OM_indexmap, (OM_key_t)*im_ptr);
    assert(obj == (void*)im_obj);
    
    /* free avl tree index map object */
    if (im_obj->tree) 
    {
        MBI_AVLtree_walk(im_obj->tree, &_delete_bitboard_data);
        MBI_AVLtree_destroy(&(im_obj->tree));
        assert(im_obj->tree == NULL);
    }
    if (im_obj->tree_local) 
    {
        MBI_AVLtree_destroy(&(im_obj->tree_local));
        assert(im_obj->tree_local == NULL);
    }
    
    /* free indexmap obj */
    free(im_obj);
    
    /* set return handle to null */
    *im_ptr = MB_NULL_INDEXMAP;
    
    return MB_SUCCESS;
}

static int _delete_bitboard_data(MBIt_AVLnode *node) {
    
    tag_table *tt;
    if (node->data != NULL)
    {
        tt = (tag_table*)node->data;
        tt_delete(&tt);
        node->data = NULL;
    }
    
    return MB_SUCCESS;
}
