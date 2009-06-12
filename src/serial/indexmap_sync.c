/* $Id$ */
/*!
 * \file serial/indexmap_sync.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_IndexMap_Sync()
 * 
 */

#include "mb_serial.h"
#include "mb_objmap.h"

/*!
 * \brief Distributes/gathers the map content across/from all processors
 * \ingroup MB_API
 * \param[in] im Index Map handle
 * 
 * When running in serial, there is nothing to sync. We just ensure that
 * \c im is a valid handle not ::MB_ERR_INVALID. While this is not compulsory,
 * checking this will aid users in porting their code the parallel 
 * implementation.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c im is null or invalid)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 */
int MB_IndexMap_Sync(MBt_IndexMap im) {
    
    MBIt_IndexMap *im_obj;
    
    /* check for null boards */
    if (im == MB_NULL_INDEXMAP) return MB_ERR_INVALID;
    
    /* get reference to indexmap object */
    im_obj = (MBIt_IndexMap *)MBI_getIndexMapRef(im);
    if (im_obj == NULL) return MB_ERR_INVALID;
    assert(im_obj->tree != NULL);
    
    /* in serial, there is nothing to sync */
	return MB_SUCCESS;
}
