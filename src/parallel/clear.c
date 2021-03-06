/* $Id$ */
/*!
 * \file parallel/clear.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Clear()
 * 
 */

#include "mb_parallel.h"
#include "mb_objmap.h"

/*!
 * \brief Clears the content of a MessageBoard
 * \ingroup MB_API
 * \param[in] mb MessageBoard handle
 * 
 * If \c MBI_CONFIG.recycle_mempool==0, we
 * use pl_recycle() instead of pl_reset() . This essentially 
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
    
    if (mb == MB_NULL_MBOARD) 
    {
        P_FUNCFAIL("Cannot clear a null board (MB_NULL_MBOARD)");
        return MB_ERR_INVALID;
    }
    /* get ref to board object */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    
    /* if mb not valid */
    if (board == NULL)
    {
        P_FUNCFAIL("Unknown board handle");
        return MB_ERR_INVALID;
    }
    
    /* simple sanity check */
    assert(board->data != NULL);
        
    /* if board locked */
    if (board->locked != MB_FALSE) 
    {
        P_FUNCFAIL("Board is locked and cannot be cleared");
        return MB_ERR_LOCKED;
    }
    
    P_INFO("Clearing board (%d, count: %d, msgsize: %d)", (int)mb, 
            (int)board->data->count_current, (int)board->data->elem_size);
    
    if (MBI_CONFIG.mempool_recycle == 0) /* reset, not recycle */
    {
        rc = pl_reset(board->data);
        if (rc != PL_SUCCESS) 
        {
            P_FUNCFAIL("pl_reset(board->data) returned err code %d", rc);
            return MB_ERR_INTERNAL;
        }
    }
    else
    {
        rc = pl_recycle(board->data);
        if (rc != PL_SUCCESS) 
        {
            P_FUNCFAIL("pl_recycle(board->data) returned err code %d", rc);
            return MB_ERR_INTERNAL;
        }
    }

    /* reset sync cursor */
    board->synced_cursor = 0;
    
    return MB_SUCCESS;
}
