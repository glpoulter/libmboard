/* $Id$ */
/*!
 * \file serial/synctest.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_SyncTest()
 * 
 */

#include "mb_serial.h"

/*!
 * \brief Inspects the completion status of a board synchronisation
 * \ingroup MB_API
 * \param[in] mb MessageBoard Handle
 * \param[out] flag address where return value can be written to
 * 
 * In this serial version, we always write ::MB_TRUE to \c flag for valid boards 
 * as synchronisation is assumed to be completed immediately after it 
 * started.
 * 
 * Testing a null board (::MB_NULL_MBOARD)
 * will always return with the ::MB_TRUE flag and ::MB_SUCCESS 
 * return code.
 * 
 * If the board is invalid of not yet synched, \c flag is set to ::MB_FALSE 
 * and the routine returns with ::MB_ERR_INVALID.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID  (Invalid board)
 * 
 */
int MB_SyncTest(MBt_Board mb, int *flag) {
    
    MBIt_Board *board;
    
    /* Check for NULL message board */
    if (mb == MB_NULL_MBOARD) 
    {
        *flag = MB_TRUE;
        return MB_SUCCESS;
    }
      
    /* get object mapped to mb handle */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    if (board == NULL || board->locked != MB_TRUE) 
    {
        *flag = MB_FALSE;
        return MB_ERR_INVALID;
    }
    
    /* nothing much to do for serial implementation :) */
    
    /* unlock board */
    board->locked = MB_FALSE;
    
    /* return success */
    *flag = MB_TRUE;
    return MB_SUCCESS;
}
