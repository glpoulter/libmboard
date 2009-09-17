/* $Id$ */
/*!
 * \file serial/setsyncpattern.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Sept 2009
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_SetSyncPattern()
 * 
 */
#include "mb_serial.h"

/*!
 * \brief Sets sync pattern of the Message Board
 * \ingroup MB_API
 * \param[in] mb Message Board handle
 * \param[in] sync_matrix Integer array representing board communication matrix
 * 
 * This routine does nothing much apart from making sure the input parameters 
 * are valid and not locked (access patterns not applicable in serial library).
 *   
 * \c sync_matrix should be an array of one element, where a zero value would
 * have the same effect as setting the board to ::MB_MODE_IDLE hence disable
 * MB_AddMessage() and all MB_Iterator_*() routines.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb is null or invalid, or \c sync_matrix is null)
 *  - ::MB_ERR_LOCKED (\c mb is locked by another process)
 */
int MB_SetSyncPattern(MBt_Board mb, unsigned int *sync_matrix) {
    
    MBIt_Board *board;
    
    if (mb == MB_NULL_MBOARD)
    {
        P_FUNCFAIL("Cannot set access mode null board (MB_NULL_MBOARD)");
        return MB_ERR_INVALID;
    }
    
    if (sync_matrix == NULL)
    {
        P_FUNCFAIL("sync_matrix pointer is NULL");
        return MB_ERR_INVALID;
    }
    
    /* map handle to object */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    if (board == NULL) 
    {
        P_FUNCFAIL("Invalid board handle (%d)", (int)mb);
        return MB_ERR_INVALID;
    }
    
    /* if board locked */
    if (board->locked != MB_FALSE)
    {
        P_FUNCFAIL("Board is locked");
        return MB_ERR_LOCKED;
    }
    
    /* if board not empty */
    if (board->data->count_current != 0)
    {
        P_FUNCFAIL("Board not empty. Access mode can only be changed for empty boards.");
        return MB_ERR_NOTREADY;
    }
    
    if (sync_matrix[0] == 0)
    {
        board->is_reader = MB_FALSE;
        board->is_writer = MB_FALSE;
    }
    else
    {
        board->is_reader = MB_TRUE;
        board->is_writer = MB_TRUE;
    } 
    
    return MB_SUCCESS;
}
