/* $Id$ */
/*!
 * \file mb_string_map.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Header file for string_map.c, a utility to keep track of a list of
 * strings and check for duplicates
 * 
 */
#ifndef MB_STRING_MAP_H_
#define MB_STRING_MAP_H_

/*! \defgroup STRINGMAP Utility: String map
 * 
 * String map is used by MB_IndexMap_Create() to keep track of names used
 * for maps and detect duplicate entries. 
 * 
 * It uses khash as the backend datastructure with a the hash of the target
 * string as the hash key, and a pointer to the target string as the hash
 * values.
 * 
 * A custom hash function is used. See __str_hash().
 * 
 * @{*/

typedef struct {
    /*! \brief Pointer to hashtable object */
    void *map;
} MBIt_stringmap;


MBIt_stringmap* MBI_stringmap_Create(void);
int MBI_stringmap_AddString(MBIt_stringmap *sm, const char *str);
int MBI_stringmap_Contains(MBIt_stringmap *sm, const char *str);
int MBI_stringmap_RemoveString(MBIt_stringmap *sm, const char *str);
int MBI_stringmap_Delete(MBIt_stringmap **sm_ptr);

/*! @} */
#endif /*MB_STRING_MAP_H_*/
