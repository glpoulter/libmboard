/* $Id$ */
/*!
 * \file serial/setaccessmode.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Aug 2009
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_SetAccessMode()
 * 
 */
#include "mb_serial.h"

/*!
 * \brief Sets access mode of the Message Board
 * \ingroup MB_API
 * \param[in] mb Message Board handle
 * \param[in] MODE Access mode identifier
 * 
 * Sets the \c board->is_reader and \c board->is_writer flags.
 * 
 * While access modes aren't really used in the serial implementation, 
 * we go ahead and set the modes as well as disable the relevant routines
 * when read/write access is disabled. This is to ensure that the
 * serial API has the same behaviour as the parallel to make it easier
 * for users to reuse their serial code when porting to parallel.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c is null or invalid, or \c MODE is invalid)
 *  - ::MB_ERR_LOCKED (\c mb is locked by another process)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 *  - ::MB_ERR_NOTREADY (board is not empty)
 */
int MB_SetAccessMode(MBt_Board mb, int MODE) {
    
    MBIt_Board  *board;
    
    /* if message board is null */
    if (mb == MB_NULL_MBOARD)
    {
        P_FUNCFAIL("Cannot set access mode null board (MB_NULL_MBOARD)");
        return MB_ERR_INVALID;
    }
    
    /* if invalid mode */
    if (MODE != MB_MODE_READONLY      &&
            MODE != MB_MODE_WRITEONLY &&
            MODE != MB_MODE_IDLE      &&
            MODE != MB_MODE_READWRITE  )
    {
        P_FUNCFAIL("Unknown MODE");
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
    
    /* set flags */
    switch (MODE) {
        case MB_MODE_READWRITE:
            board->is_reader = MB_TRUE;
            board->is_writer = MB_TRUE;
            break;
        case MB_MODE_READONLY:
            board->is_reader = MB_TRUE;
            board->is_writer = MB_FALSE;
            break;
        case MB_MODE_WRITEONLY:
            board->is_reader = MB_FALSE;
            board->is_writer = MB_TRUE;
            break;
        case MB_MODE_IDLE:
            board->is_reader = MB_FALSE;
            board->is_writer = MB_FALSE;
            break;
    }
    
    return MB_SUCCESS;
}
