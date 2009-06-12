/* $Id$ */
/*!
 * \file serial/filter_assign.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_Filter_Assign()
 * 
 */

#include "mb_serial.h"

/*!
 * \brief Assigns fitler handle to a message board
 * \ingroup MB_API
 * \param[in] mb Message Board handle
 * \param[in] ft Filter handle
 * 
 * This routine does nothing apart from making sure the input \c mb us valid and not 
 * locked (message filtering no needed in serial library).
 * 
 * While a valid board is not needed, checking its validity will help users identify 
 * problems that might exist when their code is linked to the parallel library.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (invalid or null board given, or, invalid param size) 
 *  - ::MB_ERR_LOCKED (\c mb is locked)
 */
int MB_Filter_Assign(MBt_Board mb, MBt_Filter ft) {
    
    MBIt_Board *board;
    
    /* Check for NULL message board */
    if (mb == MB_NULL_MBOARD) return MB_ERR_INVALID;
    
    /* get ptr to board */
    board = (MBIt_Board*)MBI_getMBoardRef(mb);
    if (board == NULL) return MB_ERR_INVALID;
    
    /* check if board is locked */
    if (board->locked != MB_FALSE) return MB_ERR_LOCKED;

    return MB_SUCCESS;
}
