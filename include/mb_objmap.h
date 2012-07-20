/* $Id$ */
/*!
 * \file mb_objmap.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Header file for Object Mapper
 */
#include <stdint.h>
#ifndef MB_OBJMAP_H_
#define MB_OBJMAP_H_

#ifdef HAVE_CONFIG_H
#include "mb_config.h"
#endif

/* if we're compiling a parallel library, and if we have pthreads support */
#ifdef _PARALLEL
#ifdef HAVE_PTHREAD

#include <pthread.h>
#define MB_THREADSAFE

#endif /* HAVE_PTHREAD */
#endif /* _PARALLEL */

#include <limits.h>

/*! \defgroup OBJMAP Utility: Object Mapper 
 * 
 * The Object Mapper allows us to store internal representation of
 * objects (boards and Iterators) as opaque objects. These objects
 * are stored in a hastable and mapped to a handle (of type ::OM_key_t).
 * 
 * In user-space, only the handles are used for representing the opaque
 * objects. User code is isolated from the actual object memory, allowing
 * the library developer more freedom when changing the internal 
 * representation, or using different implementations for the
 * serial and parallel versions.
 * 
 * \note We use the khash Hashtable available from 
 * http://www.freewebs.com/attractivechaos/khash.h.html
 * 
 * @{*/

/*! brief Variable type used as hashtable key */
typedef uint32_t OM_key_t;
/* If you change this, don't forget to change KHASH initialisation (in objmap.c)
 * to match or we may end up using the wrong hashing function for keys */

/*! \brief maximum possible value for key type */
/* make sure OM_key_t is unsigned or this trick won't work */
#define OM_KEY_MAX ((OM_key_t)-1)
/* #define OM_KEY_MAX UINT_MAX */

/* return values */
/*! \brief Null handle */
#define OM_NULL_INDEX     (0)
/*! \brief Dummy handle signifying an Internal Error */
#define OM_ERR_INTERNAL   (OM_KEY_MAX)
/*! \brief Dummy handle signifying a memory allocation Error */
#define OM_ERR_MEMALLOC   (OM_KEY_MAX - 1)
/*! \brief Dummy handle signifying an Error due to invalid input */
#define OM_ERR_INVALID    (OM_KEY_MAX - 2)
/*! \brief Dummy handle signifying an Overflow Error */
#define OM_ERR_OVERFLOW   (OM_KEY_MAX - 3)
/*! \brief Reserved dummy handle for users */
#define OM_DUMMY_INDEX_1  (OM_KEY_MAX - 4)
/*! \brief Reserved dummy handle for users */
#define OM_DUMMY_INDEX_2  (OM_KEY_MAX - 5)
/*! \brief Reserved dummy handle for users */
#define OM_DUMMY_INDEX_3  (OM_KEY_MAX - 6)
/*! \brief Reserved dummy handle for users */
#define OM_DUMMY_INDEX_4  (OM_KEY_MAX - 7)
/*! \brief Reserved dummy handle for users */
#define OM_DUMMY_INDEX_5  (OM_KEY_MAX - 8)
/*! \brief maximum possible value for handle */
#define OM_MAX_INDEX      (OM_KEY_MAX - 9)


/*! \brief Data Structure representing an object map */
typedef struct {
    /*! \brief Next key value to assign */
    OM_key_t top;
    /*! \brief Type of object held by this map */
    int type;
    /*! \brief Pointer to hashtable object */
    void *map;
    
    /*! \brief Handle of cached entry */
    OM_key_t cache_id;
    /*! \brief Value of cached entry */
    void *cache_obj;
    
#ifdef MB_THREADSAFE
    /*! \if paralleldoc
     * \brief mutex lock
     * \endif
     */
    pthread_mutex_t lock;
#endif /* MB_THREADSAFE */
    
} MBIt_objmap;

/* ~~ Implementation and further docs in src/utils/objmap.c ~~ */

/* return a new object map */
MBIt_objmap* MBI_objmap_new(void);

/* adds obj to map. returns handle (OM_ERR_* on failure) */
OM_key_t MBI_objmap_push(MBIt_objmap *map, void *obj);

/* get object pointer refered to by handle (return NULL on failure) */
void* MBI_objmap_getobj(MBIt_objmap *map, OM_key_t handle);

/* delete object from map and return object (NULL on failure) */
void* MBI_objmap_pop(MBIt_objmap *map, OM_key_t handle);

/* delete object map */
void MBI_objmap_destroy(MBIt_objmap **map);

/*! @} */
#endif /*MB_OBJMAP_H_*/
