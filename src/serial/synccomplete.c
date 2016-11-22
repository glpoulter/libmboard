/* $Id: synccomplete.c 2108 2009-09-14 14:14:31Z lsc $ */
/*!
 * \file serial/synccomplete.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_SyncComplete()
 * 
 */

#include "mb_serial.h"

/*!
 * \brief Completes the synchronisation of a board
 * \ingroup MB_API
 * \param[in] mb MessageBoard Handle
 * 
 * In this serial version, we do nothing anything apart from unlocking the 
 * message board as synchronisation is assumed to be completed immediately after it 
 * started.
 * 
 * Synchronisation of a null board (::MB_NULL_MBOARD) is valid, and will 
 * return immediately with ::MB_SUCCESS
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID  (Invalid board, or board is not being synced)
 * 
 */
int MB_SyncComplete(MBt_Board mb) {
    
    MBIt_Board *board;
    
    /* Check for NULL message board */
    if (mb == MB_NULL_MBOARD) 
    {
        P_WARNING("Completing sync on null board (MB_NULL_MBOARD)");
        return MB_SUCCESS;
    }
    
    /* make sure mboard object map valid */
    assert(MBI_OM_mboard != NULL);
    assert((int)MBI_OM_mboard->type == OM_TYPE_MBOARD);
    
    /* get object mapped to mb handle */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    if (board == NULL)
    {
        P_FUNCFAIL("Invalide board handle (%d)", (int)mb);
        return MB_ERR_INVALID;
    }
    /* Check if board is locked */
    if (board->locked != MB_TRUE)
    {
        P_FUNCFAIL("Board (%d) is not locked by sync process", (int)mb);
        return MB_ERR_INVALID;
    }
    /* unlock the board */
    board->locked = MB_FALSE;
    
    /* nothing much to do for serial implementation :) */

    /* return success */
    return MB_SUCCESS;
}
