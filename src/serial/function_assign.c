/* $Id$ */
/*!
 * \file serial/function_assign.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_Function_Assign()
 * 
 */

#include "mb_serial.h"

/*!
 * \brief Assigns function handle to a message board
 * \ingroup MB_API
 * \param[in] mb Message Board handle
 * \param[in] fh Function handle
 * \param[in] params Pointer to input data that will be passed into filter function 
 * \param[in] param_size Size of input data (in bytes)
 * 
 * This routine does nothing apart from making sure the input \c mb us valid and not 
 * locked (message tagging no needed in serial library).
 * 
 * While a valid board is not needed, checking its validity will help users identify 
 * problems that might exist when their code is linked to the parallel library.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (invalid or null board given, or, invalid param size) 
 *  - ::MB_ERR_LOCKED (\c mb is locked)
 */

int MB_Function_Assign ( 
        MBt_Board mb, 
        MBt_Function fh, 
        void *params, 
        size_t param_size
        ) {
    
    MBIt_Board *board;
    
    /* check input param size. Should be >1 if params not NULL */
    if (params != NULL && param_size < 1) return MB_ERR_INVALID;
    
    /* Check for NULL message board */
    if (mb == MB_NULL_MBOARD) return MB_ERR_INVALID;
    
    /* get ptr to board */
    board = (MBIt_Board*)MBI_getMBoardRef(mb);
    if (board == NULL) return MB_ERR_INVALID;
    
    /* check if board is locked */
    if (board->locked != MB_FALSE) return MB_ERR_LOCKED;

    return MB_SUCCESS;
}
