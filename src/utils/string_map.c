/* $Id: string_map.c 1871 2009-06-12 13:58:18Z lsc $ */
/*!
 * \file string_map.c
 * 
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief A utility to keep track of a list of strings and check for duplicates
 * 
 * If used in a multi-threaded environment, you will need to manually put in place
 * locking mechanisms if there will be concurrent access to any maps. The following
 * routines are not implicitly thread-safe. 
 * 
 * We use quick hash function (h = c + (h << 6) + (h << 16) -h) to map 
 * strings into int. It should give a good distribution of keys but naturally,
 * there may still be a chance for key collisions.
 * 
 * We use a KHASH map instead of set so we can store the original 
 * string pointer which allows us to check if matching keys are actual
 * string duplicates or key clashes. Of course, since we only store string
 * pointers, this can go wrong if users allocate their own string 
 * buffer instead of using string literals to provide the map name.
 * 
 */
/* splint directive needed due to khash implementation */
/*@+matchanyintegral -fcnuse@*/

#include <string.h>
#include <assert.h>
#include "khash.h"
#include "mboard.h"
#include "mb_string_map.h"

/* our custom string hash function */
static inline khint_t __str_hash(const char *s)
{
    khint_t h = (int)*s;
    if (h) for (++s ; *s; ++s) h = (khint_t)*s + (h << 6) + (h << 16) - h;
    return h;
}
#define __str_equal(a, b) (strcmp(a, b) == 0)

/* initialise khash */
/* params = (name, key_type, value_type, map=1/set=0, hash_func, cmp_func) */
KHASH_INIT(strmap, const char*, const char*, 1, __str_hash, __str_equal)

#define MAP(m) (khash_t(strmap) *)m->map

/*!
 * \brief Initialise String Map
 * \return Reference to new string map object
 * 
 * If an error condition is met, \c NULL will be returned.
 */
MBIt_stringmap * MBI_stringmap_Create(void) {
    
    MBIt_stringmap *sm;
    
    sm = (MBIt_stringmap *)malloc(sizeof(MBIt_stringmap));
    assert(sm != NULL);
    if (sm == NULL) return NULL;
    
    /* initialise khash set */
    sm->map  = (void *) kh_init(strmap);
    assert(sm->map != NULL);
    if (sm->map == NULL) 
    {
        free(sm);
        return NULL;
    }
    
    return sm;
}

/*!
 * \brief Add a new string to the map
 * \param[in] sm Reference to string map object
 * \param[in] str String to add to map
 * \return ::MB_SUCCESS or error code
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 *  - ::MB_ERR_DUPLICATE (string already exist in map)
 *  - ::MB_ERR_INVALID (\c sm is \c NULL or str is empty)
 *  - ::MB_ERR_INTERNAL (could not add string to map. out of memory?)
 */
int MBI_stringmap_AddString(MBIt_stringmap *sm, const char *str) {
    
    int rc;
    khiter_t k;
    
    /* checks for parameters done in _Contains */
    rc = MBI_stringmap_Contains(sm, str);
    if (rc == MB_TRUE) return MB_ERR_DUPLICATE; /* string already exists */
    else if (rc != MB_FALSE) return rc;         /* error condition */

    /* add string to khash set */
    k = kh_put(strmap, MAP(sm), str, &rc);
    kh_value(MAP(sm), k) = str;
    
#ifdef _EXTRA_CHECKS
    rc = MBI_stringmap_Contains(sm, str);
    assert(rc == MB_TRUE);
#endif
    
    return MB_SUCCESS;
}

/*!
 * \brief Check if map contains a particular string
 * \param[in] sm Reference to string map object
 * \param[in] str String to search for
 * \return ::MB_TRUE, ::MB_FALSE, or error code
 * 
 * Possible return codes:
 *  - ::MB_TRUE (string found in map)
 *  - ::MB_TRUE (string not found in map)
 *  - ::MB_ERR_INVALID (\c sm is \c NULL, or \c str is empty)
 *  - ::MB_ERR_INTERNAL (bug: hash-key clash. Please inform maintainer.)
 */
int MBI_stringmap_Contains(MBIt_stringmap *sm, const char *str) {
    
    const char *stored_str;
    unsigned int len;
    khiter_t k;
    int string_equal;
    
    /* check that input params are not NULL */
    if (sm == NULL)  return MB_ERR_INVALID;
    if (str == NULL) return MB_ERR_INVALID;
    
    /* check input string length */
    len = (unsigned int)strlen(str);
    if (len == 0) return MB_ERR_INVALID;
    
    /* sanity check. Will be tripped if user allocated their own map
     * object instead of using _Create() */
    assert(sm != NULL);
    assert(sm->map != NULL); 
    
    /* retrieve value from khash set */
    k = kh_get(strmap, MAP(sm), str);
    if (k < kh_end(MAP(sm)) && kh_exist(MAP(sm), k))
    {
        /* get string pointer stored in map */
        stored_str = kh_value(MAP(sm), k);
        
        /* compare store value with entered value to detect hashed key clash */
        string_equal = (strcmp(stored_str, str) == 0);
        assert(string_equal); /* ensure not key clash */
        if (!string_equal) return MB_ERR_INTERNAL; /* CLASH! */
        
        return MB_TRUE; /* FOUND. string and hashed key matches */
    }
    else return MB_FALSE; /* not in map */
}

/*!
 * \brief Remove string from map
 * \param[in] sm Reference to string map object
 * \param[in] str String to remove from map
 * \return ::MB_SUCCESS or error code
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 *  - ::MB_ERR_NOT_FOUND (string does exist in map)
 *  - ::MB_ERR_INVALID (\c sm is \c NULL or corrupted)
 */
int MBI_stringmap_RemoveString(MBIt_stringmap *sm, const char *str) {
    
    int rc;
    khiter_t k;
    
    /* checks for parameters done in _Contains */
    rc = MBI_stringmap_Contains(sm, str);
    if (rc == MB_FALSE) return MB_ERR_NOT_FOUND; /* string not in map */
    else if (rc != MB_TRUE) return rc;           /* error condition */
    
    /* retrieve position in map */
    k = kh_get(strmap, MAP(sm), str);
    /* delete it */
    kh_del(strmap, MAP(sm), k);

#ifdef _EXTRA_CHECKS
    rc = MBI_stringmap_Contains(sm, str);
    assert(rc == MB_FALSE);
#endif
    
    return MB_SUCCESS;
}

/*!
 * \brief Initialise String Map
 * \param[in] sm_ptr Address of reference to string map object
 * \return ::MB_SUCCESS or error code
 * 
 * If an error condition is met, \c sm_ptr will remain unchanged.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 *  - ::MB_ERR_INVALID (\c sm is \c NULL)
 */
int MBI_stringmap_Delete(MBIt_stringmap **sm_ptr) {
    
    MBIt_stringmap *sm;
    
    if (sm_ptr == NULL) return MB_ERR_INVALID;
    
    sm = *sm_ptr;
    if (sm == NULL) return MB_ERR_INVALID;
    assert(sm->map != NULL);
    
    /* destroy khash set */
    kh_destroy(strmap, (khash_t(strmap) *)sm->map);
    /* free object memory */
    free(sm);
    
    /* set return pointer */
    *sm_ptr = NULL;
    
    return MB_SUCCESS;
}

