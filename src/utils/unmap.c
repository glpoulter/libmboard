/* $Id$ */
/*!
 * \file unmap.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief wrapper routines for mapping Handles to object pointers
 * 
 */

#include "mboard.h"
#include "mb_utils.h"
#include "mb_common.h"
#include "mb_objmap.h"
#include <assert.h>

/*! 
 * \brief wrapper routine for mapping MessageBoard handle to object reference 
 * \param[in] mb MessageBoard handle
 * \return pointer to MessageBoard object (\c NULL if handle invalid)
 * 
 * Performs the necessary assertions (if compiled in debug mode) and obtains
 * the associated board reference from the appropriate ObjectMap.
 */
void * MBI_getMBoardRef(MBt_Board mb) {
    
    /* make sure mboard object map valid */
    assert(MBI_OM_mboard != NULL);
    assert((int)MBI_OM_mboard->type == OM_TYPE_MBOARD);
    
    /* return object mapped to mb handle */
    return MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb);

}


/*! 
 * \brief wrapper routine for mapping Iterator handle to object reference 
 * \param[in] iter Iterator handle
 * \return pointer to Iterator object (\c NULL if handle invalid)
 * 
 * Performs the necessary assertions (if compiled in debug mode) and obtains
 * the associated Iterator reference from the appropriate ObjectMap.
 */
void * MBI_getIteratorRef(MBt_Iterator iter) {
    
    /* make sure mboard object map valid */
    assert(MBI_OM_iterator != NULL);
    assert((int)MBI_OM_iterator->type == OM_TYPE_ITERATOR);
    
    /* return object mapped to mb handle */
    return MBI_objmap_getobj(MBI_OM_iterator, (OM_key_t)iter);

}


/*! 
 * \brief wrapper routine for mapping Function handle to object reference 
 * \param[in] fh Function handle
 * \return pointer to Function wrapper (\c NULL if handle invalid)
 * 
 * Performs the necessary assertions (if compiled in debug mode) and obtains
 * the associated Function reference from the appropriate ObjectMap.
 */
void * MBI_getFunctionRef(MBt_Function fh) {
    
    /* make sure mboard object map valid */
    assert(MBI_OM_function != NULL);
    assert((int)MBI_OM_function->type == OM_TYPE_FUNCTION);
    
    /* return object mapped to mb handle */
    return MBI_objmap_getobj(MBI_OM_function, (OM_key_t)fh);

}
