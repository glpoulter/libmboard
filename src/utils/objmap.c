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

#include "uthash.h"
#include "mb_objmap.h"
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

/* splint directive needed due to uthash implementation */
/*@+ignoresigns@*/


/*! \brief datatype used as node in \c uthash Hashtable */
typedef struct {
    /*! \brief Key used for indexing object in hashtable */
    OM_key_t key;
    /*! \brief Pointer to object being mapped */
    void *obj;
    /*! \brief metadata required by \c uthash */
    UT_hash_handle hh;
} mymap_t;

/* internal routine to deallocate map memory */
static void delete_map_data(mymap_t *ht);


/*!
 * \brief Creates a new ObjectMap
 * \return Pointer to newly created ObjectMap (or \c NULL if unable to allocate 
 *         required memory)
 * 
 * The required memory is allocated and its initial values written in.
 * 
 */
MBIt_objmap* MBI_objmap_new(void) {
    
#ifdef MB_THREADSAFE
	int rc;
#endif /* MB_THREADSAFE */
	
    MBIt_objmap *mymap = NULL;
    
    mymap = (MBIt_objmap *)malloc(sizeof(MBIt_objmap));
    assert(mymap != NULL);
    if (mymap == NULL) /* on malloc error */
    {
        return NULL;
    }
    
    mymap->top  = 0;
    mymap->type = 0;
    
#ifdef OBJMAP_CYCLE_KEY
    mymap->key_wrapped = 0;
#endif
    
    /* ->map must be initialised to NULL as required by uthash */
    mymap->map  = NULL;
    
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
    
#ifdef MB_THREADSAFE
	int rc;
#endif /* MB_THREADSAFE */
	
    OM_key_t handle;
    mymap_t *entry;
    mymap_t *ht;

#ifdef OBJMAP_CYCLE_KEY
    mymap_t *temp;
#endif
    
    /* return error code if NULL arguments given */
    if (!map) return OM_ERR_INVALID;
    if (!obj) return OM_ERR_INVALID;
        
    /* allocate struct for hastable entry */
    entry = (mymap_t *)malloc(sizeof(mymap_t));
    assert(entry != NULL);
    if (entry == NULL) return OM_ERR_MEMALLOC;
    
#ifdef MB_THREADSAFE
    /* capture mutex lock before proceeding */
    rc = pthread_mutex_lock(&(map->lock));
    assert(0 == rc);
#endif /* MB_THREADSAFE */
    
    /* get ref to hash table */
    ht = (mymap_t *)(map->map);
    /* assert(ht); */
    
    handle = map->top; 
    entry->key = handle; /* assign handle as key */
    entry->obj = obj;    /* assign obj address   */
    
    /* add to ut_hashtable */
    HASH_ADD(hh, ht, key, sizeof(OM_key_t), entry);
    
    /* update map */
    map->map = ht;
    map->top++; /* increment value for next handle */
    
#ifdef OBJMAP_CYCLE_KEY
    
    if (map->top > OM_MAX_INDEX)
    {
        /* wrap back to starting index */
        map->key_wrapped = 1;
        map->top = 0;
    }
    
    /* if key has wrapped round, we must always check if key is already
     * being used 
     */
    if (1 == map->key_wrapped)
    {
        while (1 == 1) /* infinite loop. Wheeeeee! */
        {
            HASH_FIND(hh, ht, &map->top, sizeof(OM_key_t), temp);
            
            if (temp != NULL) break;/* found available key */
            
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
    
    mymap_t *entry = NULL;
    void  *obj   = NULL;
    mymap_t *ht;
    
#ifdef MB_THREADSAFE 
    int rc;
#endif
    
    if (!map) return NULL;
    
#ifdef MB_THREADSAFE
    /* capture mutex lock before proceeding */
    rc = pthread_mutex_lock(&(map->lock));
    assert(0 == rc);
#endif /* MB_THREADSAFE */
    
    /* get ref to hash table */
    ht = (mymap_t *)(map->map);
    /* assert(ht); */
    
    /* retrieve item from ut_hashtable */
    HASH_FIND(hh, ht, &handle, sizeof(OM_key_t), entry);
    
#ifdef MB_THREADSAFE
	/* release mutex lock before proceeding */
	rc = pthread_mutex_unlock(&(map->lock));
	assert(0 == rc);
#endif /* MB_THREADSAFE */
	
    /* if obj found, get ptr to our object */
    if (entry != NULL)
    {
        obj = entry->obj;
        assert(obj != NULL);
    }
    
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
    
    mymap_t *entry = NULL;
    void  *obj   = NULL;
    mymap_t *ht;
    
#ifdef MB_THREADSAFE 
    int rc;
#endif
    
    if (!map) return NULL;
    
#ifdef MB_THREADSAFE
    /* capture mutex lock before proceeding */
    rc = pthread_mutex_lock(&(map->lock));
    assert(0 == rc);
#endif /* MB_THREADSAFE */
    
    /* get ref to hash table */
    ht = (mymap_t *)(map->map);
    /* assert(ht); */
    
    /* retrieve item from ut_hashtable */
    HASH_FIND(hh, ht, &handle, sizeof(OM_key_t), entry);
    
    /* if obj found, remove from ut_hashtable */
    if (entry != NULL)
    {
        /* get ptr to our object */
        obj = entry->obj;
        assert(obj != NULL);
        
        /* remove hash entry */
        HASH_DEL(ht, entry);
        
        /* update map reference to hash table */
        map->map = ht;
        free(entry);
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
    
    delete_map_data((mymap_t *)(mytmp->map));

#ifdef MB_THREADSAFE
    /* destroy mutex obj */
    pthread_mutex_destroy(&(mytmp->lock));
#endif /* MB_THREADSAFE */
    
    free(mytmp);
    
    
}

/* --- internal routines --- */

/*! 
 * \brief Deletes all memory associated to object hashtable
 * \param[in] ht pointer to hashtable
 * 
 * Treats \c ht as a linked-list (which is possible with \c uthash)
 * and delete all nodes as we traverse the list
 */
static void delete_map_data(mymap_t *ht) {
    mymap_t *first;
    
    while (ht) /* repeat till table empty */
    {
        /* get first element */
        first = ht;
        
        /* this elem should have obj != NULL */
        assert(first->obj != NULL);
        
        /* remove first element from hash */
        HASH_DEL(ht, first);
        
        /* deallocate memory associated to elem */
        free(first->obj);
        free(first);
    }
    
}
