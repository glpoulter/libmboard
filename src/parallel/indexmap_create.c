/* $Id$ */
/*!
 * \file parallel/indexmap_create.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_IndexMap_Create()
 * 
 */

#include "mb_parallel.h"
#include "mb_objmap.h"

#define __name_exists(name) MBI_stringmap_Contains(MBI_indexmap_nametable,name)
#define __name_add(name)   MBI_stringmap_AddString(MBI_indexmap_nametable,name)

#ifdef _EXTRA_CHECKS
#ifdef _UNSAFE_CHECKS
    static void check_all_name_equal(OM_key_t key, const char *name);
    
    struct namedata_t {
        OM_key_t handle;
        char name[MB_INDEXMAP_NAMELENGTH];
    };
#endif /* _UNSAFE_CHECKS */
#endif /*_EXTRA_CHECKS*/
    
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
    if (MB_Env_Initialised() != MB_SUCCESS) 
    {
        P_FUNCFAIL("Message Board environment not yet initialised");
        return MB_ERR_ENV;
    }
    /* make sure name is acceptable */
    if (name == NULL) 
    {
        P_FUNCFAIL("NULL pointer given as seconds argument (name)");
        return MB_ERR_INVALID;
    }
    if (strlen(name) == 0)
    {
        P_FUNCFAIL("Empty string given as seconds argument (name)");
        return MB_ERR_INVALID;
    }
    if (strlen(name) > MB_INDEXMAP_NAMELENGTH)
    {
        P_FUNCFAIL("String given as seconds argument (name) is too long");
        return MB_ERR_OVERFLOW;
    }
    /* make sure name is not already used */
    assert(MBI_indexmap_nametable != NULL);
    if (__name_exists(name)) 
    {
        P_FUNCFAIL("Map name (%s) already exists", name);
        return MB_ERR_DUPLICATE;
    }
    
    /* allocate memory for index map object */
    im_obj = (MBIt_IndexMap *)malloc(sizeof(MBIt_IndexMap));
    assert(im_obj != NULL);
    if (im_obj == NULL)
    {
        P_FUNCFAIL("Could not allocate required memory");
        return MB_ERR_MEMALLOC;
    }
    
    /* create an avl tree object */
    tree = MBI_AVLtree_create();
    assert(tree != NULL);
    if (tree == NULL)
    {
        P_FUNCFAIL("Could not create AVLtree for map");
        return MB_ERR_INTERNAL;
    }
    
    /* assign tree to im obj */
    im_obj->tree_local = tree;
    
    /* initialise internal values */
    im_obj->tree             = NULL;
    im_obj->synced           = MB_FALSE;
    im_obj->cache_leaf_ptr   = NULL;
    im_obj->cache_leaf_value = -99999;
    
    /* register object */
    assert(MBI_OM_indexmap != NULL);
    assert(MBI_OM_indexmap->type == OM_TYPE_INDEXMAP);
    rc_om = MBI_objmap_push(MBI_OM_indexmap, (void*)im_obj);
    if (rc_om > OM_MAX_INDEX)
    {
        if (rc_om == OM_ERR_MEMALLOC)
        {
            P_FUNCFAIL("Could not allocate required memory");
            return MB_ERR_MEMALLOC;
        }
        else
        {
            P_FUNCFAIL("MBI_objmap_push() failed with err code %d", (int)rc_om);
            return MB_ERR_INTERNAL;
        }
    }
    
    /* debug: make sure same name on all procs */
#ifdef _EXTRA_CHECKS
#ifdef _UNSAFE_CHECKS
    check_all_name_equal(rc_om, name);
#endif /* _UNSAFE_CHECKS */
#endif /*_EXTRA_CHECKS*/
    
    /* add name to indexmap name table */
    __name_add(name);
    
    /* add name to object */
    im_obj->name = name;
    
    /* return handle via ptr */
    *im_ptr = (MBt_IndexMap)rc_om;
    
    P_INFO("Created index map (%d) '%s'", (int)rc_om, name);
    
    return MB_SUCCESS;
}

#ifdef _EXTRA_CHECKS
#ifdef _UNSAFE_CHECKS
/* check that "name" is the same on all processors to ensure that
 * maps are created in the same order and thus have matching 
 * handles 
 */
static void check_all_name_equal(OM_key_t key, const char *name) {
    
    int rc;
    struct namedata_t namedata; 
    
    if (MASTERNODE)
    {
        namedata.handle = key;
        strcpy((namedata.name), name);
    }
    
    rc = MPI_Bcast(&namedata, (int)sizeof(struct namedata_t), MPI_BYTE, 0, MBI_CommWorld);
    assert(rc == MPI_SUCCESS);
    assert(namedata.handle == key);
    rc = strcmp((const char *)namedata.name, name);
    assert(rc == 0);
}
#endif /* _UNSAFE_CHECKS */
#endif /*_EXTRA_CHECKS*/
