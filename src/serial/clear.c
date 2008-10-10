/* $Id$ */
/*!
 * \file serial/clear.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_Clear()
 * 
 * \todo Allow MB_CONFIG_RECYCLE_MEMPOOL to be defined as in env variable
 */

#include "mb_serial.h"
#include "mb_objmap.h"
#include <string.h>

/*!
 * \brief Clears the content of a MessageBoard
 * \ingroup MB_API
 * \param[in] mb MessageBoard handle
 * 
 * if \c MB_CONFIG_RECYCLE_MEMPOOL is defined during compilation, we
 * use pl_recycle() is used instead of pl_reset() . This essentially 
 * resets the internal memory pointers without releasing memory. 
 * This can be useful if message board utilisation is consistent, and
 * we want to save the effort of repeatedly deallocating and allocating
 * memory. However, do take note that this would apply to ALL message
 * boards, and can substantially increase memory usage.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (invalid or null board given) 
 *  - ::MB_ERR_LOCKED (message board is locked for synchronisation)
 *  - ::MB_ERR_INTERNAL (possible bug. Recompile and run in debug mode for hints)
 */
int MB_Clear(MBt_Board mb) {
    
    int rc;
    MBIt_Board *board;
    
    if (mb == MB_NULL_MBOARD) return MB_ERR_INVALID;
    
    /* get ref to board object */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    
    /* if mb not valid */
    if (board == NULL) return MB_ERR_INVALID;
    
    /* simple sanity check */
    assert(board->data != NULL);
        
    /* if board locked */
    if (board->locked != MB_FALSE) return MB_ERR_LOCKED;
    
    /* clear pooled_list */
    
#ifdef MB_CONFIG_RECYCLE_MEMPOOL
    rc = pl_recycle(board->data);
#else
    rc = pl_reset(board->data);
#endif
    
    if (rc != PL_SUCCESS) return MB_ERR_INTERNAL;
   
    return MB_SUCCESS;
}
