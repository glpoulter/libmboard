/* $Id$ */
/*!
 * \file parallel/synctest.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_SyncTest()
 * 
 */
#include "mb_parallel.h"

/*!
 * \brief Inspects the completion status of a board synchronisation.
 * \ingroup MB_API
 * \param[in] mb MessageBoard Handle
 * \param[out] flag address where return value can be written to
 * 
 * (more ...)
 * 
 * Testing a null board (::MB_NULL_MBOARD) or one that is not being 
 * synchronised will always return with the ::MB_TRUE flag and ::MB_SUCCESS 
 * return code.
 * 
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID  (Invalid board)
 * 
 */
int MB_SyncTest(MBt_Board mb, int *flag) {
    
    MBIt_Board *board;
    int rc, completion_status;
    
    /* Check for NULL message board */
    if (mb == MB_NULL_MBOARD) 
    {
        P_WARNING("Deletion of null board (MB_NULL_MBOARD)");
        *flag = MB_TRUE;
        return MB_SUCCESS;
    }
    
    /* get object mapped to mb handle */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    if (board == NULL) return MB_ERR_INVALID;

    /* check if board is locked */
    if (board->locked == MB_FALSE)
    {
        P_FUNCFAIL("Board (%d) is not locked", (int)mb);
        *flag = MB_FALSE;
        return MB_ERR_INVALID;
    }
    
    /* capture lock */
    rc = pthread_mutex_lock(&(board->syncLock));
    assert(0 == rc);
    if (rc != 0) return MB_ERR_INTERNAL;
    
    /* retrieve completion status */
    completion_status = (int)board->syncCompleted;
    
    /* if sync completed, unlock board */
    if (completion_status == MB_TRUE) 
    {
        P_INFO("Board (%d) sync completed. Unlocking board", (int)mb);
        board->locked = MB_FALSE;
    }
    
    /* release lock */
    rc = pthread_mutex_unlock(&(board->syncLock));
    assert(0 == rc);
    if (rc != 0) return MB_ERR_INTERNAL;
    
    /* return results */
    *flag = completion_status;
    return MB_SUCCESS;
}
