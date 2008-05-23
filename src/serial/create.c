/* $Id$ */
/*!
 * \file serial/create.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_Create()
 * 
 */

#include "mb_serial.h"

/*!
 * \brief Instantiates a new MessageBoard
 * \ingroup MB_API
 * \param[out] mb_ptr Address of MessageBoard handle
 * \param[in] msgsize Size of message that this MessageBoard will be used for
 * 
 * The MessageBoard object is allocated and registered  with the ::MBI_OM_mboard
 * ObjectMap. The reference ID returned by ObjectMap is then written to 
 * \c mb_ptr as the handle.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 *  - ::MB_ERR_INVALID (invalid or null board given) 
 *  - ::MB_ERR_MEMALLOC (error allocating memory for memory pool or ObjectMap entry)
 *  - ::MB_ERR_INTERNAL (possible bug. Recompile and run in debug mode for hints)
 *  - ::MB_ERR_ENV (MessageBoard environment not yet initialised)
 */
int MB_Create(MBt_Board *mb_ptr, size_t msgsize) {
    
    int rc;
    OM_key_t rc_om;
    MBIt_Board *mb_obj;

    /* first, set board to NULL in case we quit on error */
    *mb_ptr = MB_NULL_MBOARD;
    
    /* make sure MB environment has been set up */
    assert(MB_Env_Initialised() == MB_SUCCESS);
    if (MB_Env_Initialised() != MB_SUCCESS) return MB_ERR_ENV;
    
    /* check for invalid input size */
    if ((int)msgsize <= 0) return MB_ERR_INVALID;
    
    /* allocate message board object */
    mb_obj = (MBIt_Board *)malloc(sizeof(MBIt_Board));
    assert(mb_obj != NULL);
    if (errno != 0 || mb_obj == NULL) /* on error */
    {
        return MB_ERR_MEMALLOC;
    }
    
    mb_obj->locked = MB_FALSE; /* initialise lock */
    
    /* allocate pooled list */
    rc = pl_create(&(mb_obj->data), msgsize, MB_CONFIG_SERIAL_POOLSIZE);
    if (rc != PL_SUCCESS)
    {
        free(mb_obj);
        if (rc == PL_ERR_MALLOC) return MB_ERR_MEMALLOC;
        else return MB_ERR_INTERNAL;
    }
    
    /* register new mboard object */
    assert(MBI_OM_mboard != NULL);
    assert(MBI_OM_mboard->type == OM_TYPE_MBOARD);
    rc_om = MBI_objmap_push(MBI_OM_mboard, (void*)mb_obj);
    if (rc_om > OM_MAX_INDEX)
    {
        if (rc_om == OM_ERR_MEMALLOC)
        {
            return MB_ERR_MEMALLOC;
        }
        else
        {
            return MB_ERR_INTERNAL;
        }
    }
    
    /* assign handle to new mboard */
    *mb_ptr  = (MBt_Board)rc_om;
    
    return MB_SUCCESS;
}
