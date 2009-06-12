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

/*! \brief shortcut to access khash map */
#define MAP (khash_t(objmap) *)map->map

/* internal routine */
static void delete_map_data(MBIt_objmap *map);

/*!
 * \brief Creates a new ObjectMap
 * \return Pointer to newly created ObjectMap (or \c NULL if unable to allocate 
 *         required memory)
 * 
 * The required memory is allocated and its initial values written in.
 * 
 */
MBIt_objmap* MBI_objmap_new(void) {
    
    int i;
#ifdef MB_THREADSAFE
    int rc;
#endif /* MB_THREADSAFE */
    
    MBIt_objmap *mymap = NULL;
    
    /* allocate memory for object map */
    mymap = (MBIt_objmap *)malloc(sizeof(MBIt_objmap));
    assert(mymap != NULL);
    if (mymap == NULL) /* on malloc error */
    {
        return NULL;
    }
    
    /* initialise values */
    mymap->top  = 0;
    mymap->type = 0;
#ifdef OBJMAP_CYCLE_KEY
    mymap->key_wrapped = 0;
#endif
    
    /* initialise directmap */
    for (i = 0; i < OBJMAP_DMAP_SIZE; i++) mymap->dmap[i] = NULL;
    
    /* initialise hash table for indirect mapping */
    mymap->map  = (void *) kh_init(objmap);
    assert(mymap->map != NULL);
    
    /* initialise cache */
    mymap->cache_id  = -1;
    mymap->cache_obj = NULL;
    
#ifdef MB_THREADSAFE
    rc = pthread_mutex_init(&(mymap->lock), NULL);
    assert(0 == rc);
    if (0 != rc)
    {
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
 * 
 * \note If routine returns with ::OM_ERR_OVERFLOW, we might have run out of
 *       numbers for assigning keys. Uncomment the definition of 
 *       \c OBJMAP_CYCLE_KEY in objmap.h to allow MBI_objmap_push() to recycle
 *       used keys. This will allow ObjectMap to scale better, but with the overhead
 *       ofchecking key validity before assignment (hits the hashtable for each
 *       check). If this is enabled, and you still get OM_ERR_OVERFLOW, it means
 *       something has gone really wrong. Is the user not deleting objects that 
 *       are no longer needed? Is the key type too small for the number of objects 
 *       the user needs to simultaneously store?
 */
OM_key_t MBI_objmap_push(MBIt_objmap *map, void *obj) {
    
    int rc;
    int index;
    OM_key_t handle;
    khiter_t k;
    
#ifdef OBJMAP_CYCLE_KEY
    mymap_t *temp;
#endif
    
    /* return error code if NULL arguments given */
    if (!map) return OM_ERR_INVALID;
    if (!obj) return OM_ERR_INVALID;
        
    
#ifdef MB_THREADSAFE
    /* capture mutex lock before proceeding */
    rc = pthread_mutex_lock(&(map->lock));
    assert(0 == rc);
#endif /* MB_THREADSAFE */

    /* index to add obj to in map */
    index = (int)map->top;
    
    /* if direct map within range, use that */
    if (index < OBJMAP_DMAP_SIZE)
    {
        map->dmap[index] = obj;
    }
    else /* else, use hash table for indirect mapping */
    {
        /* add to hashtable */
        k = kh_put(objmap, MAP, index, &rc);
        assert(rc);
        kh_value(MAP, k) = obj;
    }
    
    /* set return val */
    handle = map->top;
    
    /* update map */
    map->top++; /* increment value for next handle */
    
#ifdef OBJMAP_CYCLE_KEY
    
    if (map->top > OM_MAX_INDEX)
    {
        /* wrap back to starting index */
        map->key_wrapped = 1;
        map->top = OBJMAP_DMAP_SIZE;
    }
    
    /* if key has wrapped round, we must always check if key is already
     * being used 
     */
    if (1 == map->key_wrapped)
    {
        while (1 == 1) /* infinite loop. Wheeeeee! */
        {
            k = kh_get(objmap, MAP, map->top);
            if (k == kh_end(MAP)) break;/* found available key */
            
            /* else, try next index */
            map->top++;
            
            /* have we run out of keys?? */
            if (map->top > OM_MAX_INDEX) 
            {
                handle = OM_ERR_OVERFLOW;
                break;
            }
        }
    }

#else
    
    assert(map->top <= OM_MAX_INDEX); 
    if (map->top > OM_MAX_INDEX)
    {
        /* return error code */
        handle = OM_ERR_OVERFLOW;
    }
    
#endif
    
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
    
    int index;
    void  *obj = NULL;
    khiter_t k;
    
#ifdef MB_THREADSAFE 
    int rc;
#endif
    
    if (!map) return NULL;
    
#ifdef MB_THREADSAFE
    /* capture mutex lock before proceeding */
    rc = pthread_mutex_lock(&(map->lock));
    assert(0 == rc);
#endif /* MB_THREADSAFE */

    index = (int)handle;
    
    if (index == map->cache_id)
    {
        obj = map->cache_obj;
    }
    else if (index < OBJMAP_DMAP_SIZE && index >= 0)
    {
        obj = map->dmap[index];
        map->cache_obj = obj;
        map->cache_id  = index;
    }
    else
    {
        /* retrieve item from hashtable */
        k = kh_get(objmap, MAP, index);
        
        if (index >= OBJMAP_DMAP_SIZE && k < kh_end(MAP) && kh_exist(MAP, k))
        {
            obj = kh_value(MAP, k);
            assert(obj != NULL);
            
            map->cache_obj = obj;
            map->cache_id  = index;
        }
    }
    
#ifdef MB_THREADSAFE
    /* release mutex lock before proceeding */
    rc = pthread_mutex_unlock(&(map->lock));
    assert(0 == rc);
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
    
    int index;
    khiter_t k;
    void  *obj   = NULL;
    
#ifdef MB_THREADSAFE 
    int rc;
#endif
    
    if (!map) return NULL;
    
#ifdef MB_THREADSAFE
    /* capture mutex lock before proceeding */
    rc = pthread_mutex_lock(&(map->lock));
    assert(0 == rc);
#endif /* MB_THREADSAFE */
    
    index = (int)handle;
   
    if (index < OBJMAP_DMAP_SIZE && index >= 0)
    {
        obj = map->dmap[index];
        map->dmap[index] = NULL;
    }
    else
    {
        /* retrieve item from ut_hashtable */
        /* HASH_FIND(hh, ht, &handle, sizeof(OM_key_t), entry); */
        k = kh_get(objmap, MAP, index);
        
        /* if obj found, remove from ut_hashtable */
        if (k < kh_end(MAP) && kh_exist(MAP, k))
        {
            /* get ptr to our object */
            obj = kh_value(MAP, k);
            assert(obj != NULL);
            
            /* remove hash entry */
            kh_del(objmap, MAP, k);
            
        }
    }
    
    /* we may also need to clear the cache */
    if (index == map->cache_id)
    {
        map->cache_id  = -1;
        map->cache_obj = NULL;
    }
#ifdef MB_THREADSAFE
    /* release mutex lock before proceeding */
    rc = pthread_mutex_unlock(&(map->lock));
    assert(0 == rc);
#endif /* MB_THREADSAFE */
    
    return obj;
}

/*!
 * \brief Deletes an ObjectMap
 * \param[in] map Reference to ObjectMap
 * 
 * Deallocates all memory, including those of objects referenced by the map.
 * 
 * \warning Deleting an ObjectMap will invalidate all pointers to objects
 *    returned by MBI_objmap_getobj() and MBI_objmap_pop().
 */
void MBI_objmap_destroy(MBIt_objmap **map) {
    
    MBIt_objmap *mytmp;
    
    if (*map == NULL) return;
    
    mytmp = *map;
    *map  = NULL;
    
    delete_map_data(mytmp);
    
    kh_destroy(objmap, (khash_t(objmap) *)mytmp->map);

#ifdef MB_THREADSAFE
    /* destroy mutex obj */
    pthread_mutex_destroy(&(mytmp->lock));
#endif /* MB_THREADSAFE */
    
    free(mytmp);
    
    
}

/* --- internal routines --- */

/*! 
 * \brief Deletes all memory associated to object hashtable
 * \param[in] map pointer to hashtable
 * 
 */
static void delete_map_data(MBIt_objmap *map) {
    
    int i;
    khiter_t k;
    void * obj;
    
    /* first, destroy objects in directmap */
    for (i = 0; i < OBJMAP_DMAP_SIZE; i++)
    {
        if (map->dmap[i] != NULL) free(map->dmap[i]);
    }
    
    /* detroy objects stored in indirect map */
    for (k = kh_begin(MAP); k != kh_end(MAP); ++k)
    {
        if (kh_exist(MAP, k))
        {
            obj = kh_value(MAP, k);
            free(obj);
        }

    }
}

