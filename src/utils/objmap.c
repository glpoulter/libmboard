/* $Id$ */
/*!
 * \file objmap.c
 * 
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief ADT for mapping arbitrary objects as opaque objects
 * 
 */
/* splint directive needed due to khash implementation */
/*@+matchanyintegral -fcnuse@*/
#include "khash.h"
#include "mb_objmap.h"
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

/*! \brief Khash initialisation */
KHASH_MAP_INIT_INT(objmap, void*)
/* from unsigned long, use KHASH_MAP_INIT_INT64 */

/*! \brief shortcut to access khash map */
#define MAP(om) ((khash_t(objmap)*)om->map)

/*!
 * \brief Creates a new ObjectMap
 * \return Pointer to newly created ObjectMap (or \c NULL if unable to allocate 
 *         required memory)
 * 
 * The required memory is allocated and its initial values written in.
 * 
 */
MBIt_objmap* MBI_objmap_new(void) {
    MBIt_objmap *mymap = NULL;
#ifdef MB_THREADSAFE
    int rc;
#endif

    /* allocate memory for object map */
    mymap = malloc(sizeof(MBIt_objmap));
    assert(mymap != NULL);
    if (mymap == NULL) return NULL; /* on malloc error */

    /* initialise values */
    mymap->top  = 1;
    mymap->type = 0;
    
    /* initialise hash table for indirect mapping */
    mymap->map = (void*)kh_init(objmap);
    assert(mymap->map != NULL);
    
    /* initialise cache */
    mymap->cache_id  = OM_KEY_MAX;
    mymap->cache_obj = NULL;
    
#ifdef MB_THREADSAFE
    rc = pthread_mutex_init(&(mymap->lock), NULL);
    assert(0 == rc);
    if (0 != rc) {
        free(mymap);
        return NULL;
    }
#endif /* MB_THREADSAFE */
    
    return mymap;
}

/*!
 * \brief Adds object to map
 * \param[in] map Reference to ObjectMap
 * \param[in] obj Reference to object
 * \return int representing handle (\c OM_ERR_* on failure)
 * 
 * A suitable key is chosen and associated to \c obj. The key is returned
 * to be used as object handle, and the obj-key pair is added to the hashtable.
 */
OM_key_t MBI_objmap_push(MBIt_objmap *map, void *obj) {
    
    int rc;
    OM_key_t handle;
    khiter_t k;
    khash_t(objmap)* m;
    
    /* return error code if NULL arguments given */
    if (!map) return OM_ERR_INVALID;
    if (!obj) return OM_ERR_INVALID;
        
#ifdef MB_THREADSAFE
    /* capture mutex lock before proceeding */
    rc = pthread_mutex_lock(&(map->lock));
    assert(0 == rc);
#endif /* MB_THREADSAFE */
    
    /* index to add obj to in map */
    handle = map->top++;
    m = MAP(map);
    
    if (handle > OM_MAX_INDEX) { /* make sure we haven't run out of keys */
      handle = OM_ERR_OVERFLOW;
    } else {
      /* add to hashtable */
      k = kh_put(objmap, m, handle, &rc);
      assert(rc);
      kh_value(m, k) = obj;
    }

#ifdef MB_THREADSAFE
    /* release mutex lock before proceeding */
    rc = pthread_mutex_unlock(&(map->lock));
    assert(0 == rc);
#endif /* MB_THREADSAFE */
        
    return handle;
}

/*!
 * \brief Returns pointer to object associated to handle
 * \param[in] map Reference to ObjectMap
 * \param[in] handle Object handle
 * \return pointer to object (\c NULL if handle is invalid)
 * 
 * The associated object pointer is retrived from the ObjectMap and
 * returned to the user. 
 * 
 * The user must NOT deallocate memory associated 
 * with the returned pointer as the same object will still be 
 * referenced by the ObjectMap. Doing so will lead to memory errors
 * when the ObjectMap is deleted, of when the object is retrieved
 * and used in the future.
 */
void* MBI_objmap_getobj(MBIt_objmap *map, OM_key_t handle) {
    void  *obj = NULL;
    khiter_t k;
    khash_t(objmap)* m;
    
    if (!map) return NULL;
    
#ifdef MB_THREADSAFE
    /* capture mutex lock before proceeding */
    pthread_mutex_lock(&(map->lock));
#endif /* MB_THREADSAFE */

    if (handle == map->cache_id) {
        obj = map->cache_obj;
    } else {
        /* retrieve item from hashtable */
        m = MAP(map);
        k = kh_get(objmap, m, handle);
        if (k != kh_end(m)) {  /* found */
            obj = kh_value(m, k);
            assert(obj != NULL);
            
            map->cache_obj = obj;
            map->cache_id  = handle;
        }
    }
    
#ifdef MB_THREADSAFE
    /* release mutex lock before proceeding */
    pthread_mutex_unlock(&(map->lock));
#endif /* MB_THREADSAFE */
    
    return obj;
}

/*!
 * \brief Returns pointer to object associated to handle and deletes object from map
 * \param[in] map Reference to ObjectMap
 * \param[in] handle Object handle
 * \return pointer to object (\c NULL if handle is invalid)
 * 
 * Similar to MBI_objmap_pop() except that the object is removed from the map
 * once it has been returned. The user is expected to deallocate the object when 
 * done with it.
 */
void* MBI_objmap_pop(MBIt_objmap *map, OM_key_t handle) {
    khiter_t k;
    void *obj = NULL;
    khash_t(objmap)* m;
    
    if (!map) return NULL;
    m = MAP(map);
    
#ifdef MB_THREADSAFE
    /* capture mutex lock before proceeding */
    pthread_mutex_lock(&(map->lock));
#endif /* MB_THREADSAFE */
   
    /* retrieve item from ut_hashtable */
    k = kh_get(objmap, m, handle);
       
    /* if obj found, remove from ut_hashtable */
    if (k != kh_end(m)) {
      /* get ptr to our object */
      obj = kh_value(m, k);
      assert(obj != NULL);
           
      /* remove hash entry */
      kh_del(objmap, m, k);
    }

    /* we may also need to clear the cache */
    if (handle == map->cache_id) {
        map->cache_id  = OM_KEY_MAX;
        map->cache_obj = NULL;
    }
    
#ifdef MB_THREADSAFE
    /* release mutex lock before proceeding */
    pthread_mutex_unlock(&(map->lock));
#endif /* MB_THREADSAFE */
    
    return obj;
}

/*!
 * \brief Deletes an ObjectMap
 * \param[in] map_ptr Address of variable storing pointer to ObjectMap
 * 
 * Deallocates all memory, including those of objects referenced by the map.
 * 
 * \warning Deleting an ObjectMap will invalidate all pointers to objects
 *    returned by MBI_objmap_getobj() and MBI_objmap_pop().
 */
void MBI_objmap_destroy(MBIt_objmap **map_ptr) {
    MBIt_objmap *map;
    khash_t(objmap)* m;
    khiter_t k;
    
    if (*map_ptr == NULL) return;
    
    map = *map_ptr;
    *map_ptr  = NULL;
    m = MAP(map);
    
    /* detroy objects stored in hash */
    for (k = kh_begin(m); k != kh_end(m); ++k) {
      if (kh_exist(m, k)) free(kh_value(m, k));
    }
    
    /* destroy hash object */
    kh_destroy(objmap, m);

#ifdef MB_THREADSAFE
    /* destroy mutex obj */
    pthread_mutex_destroy(&(map->lock));
#endif /* MB_THREADSAFE */
    
    free(map);
}
