/* $Id$ */
/*!
 * \file parallel/delete.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Delete()
 * 
 */
#include "mb_parallel.h"

/* should this be a collective operation? */
int MB_Delete(MBt_Board *mb_ptr) {
    
    int rc;
    void *obj;
    MBIt_Board *board;
    
    /* Check for NULL message board */
    if (*mb_ptr == MB_NULL_MBOARD) return MB_SUCCESS;
    
    /* get reference to mboard object */
    board = (MBIt_Board *)MBI_getMBoardRef(*mb_ptr);
    if (board == NULL) return MB_ERR_INVALID;
    assert(board->data != NULL);
    
    /* If message board locked by other process */
    if (board->locked == MB_TRUE) return MB_ERR_LOCKED;
    
    /* Delete object from map*/
    obj = MBI_objmap_pop(MBI_OM_mboard, (OM_key_t)*mb_ptr);
    assert(obj == (void*)board);
    
    /* free object memory */
    rc = pl_delete(&(board->data));
    assert(rc == PL_SUCCESS);
    
    /* check that tagtable has been deallocated */
    assert(board->tt == NULL);
    
    /* destroy lock and cond var */
    rc = pthread_mutex_destroy(&(board->syncLock));
    assert(rc == 0);
    rc = pthread_cond_destroy(&(board->syncCond));
    assert(rc == 0);
    
    /* deallocate board object */
    free(obj);
    
    /* set mb to NULL */
    *mb_ptr = MB_NULL_MBOARD;
    
    /* indicate internal error if any */
    if (rc != PL_SUCCESS) return MB_ERR_INTERNAL;
    
    return MB_SUCCESS;
}
