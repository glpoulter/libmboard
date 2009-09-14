/* $Id$ */
/*!
 * \file serial/addmessage.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_AddMessage()
 * 
 */
#include "mb_serial.h"
#include <string.h>

/*!
 * \brief Adds a message to the MessageBoard
 * \ingroup MB_API
 * \param[in] mb MessageBoard handle
 * \param[in] msg Address of the message to be added
 *  
 * The message data is copied into a data node in the message board. Upon 
 * completion, users are free to modify, reuse, or deallocate their copy of 
 * the message. Not only will this be less susceptible to bugs, it also 
 * gives us more control over memory management.
 * 
 * \note We use \c memcpy to copy message data which makes endianness an
 * issue if run on heterogenous systems.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 *  - ::MB_ERR_INVALID (invalid or null board given) 
 *  - ::MB_ERR_MEMALLOC (error allocating memory for expanding memory pool)
 *  - ::MB_ERR_LOCKED (message board is locked for synchronisation)
 *  - ::MB_ERR_INTERNAL (possible bug. Recompile and run in debug mode for hints)
 */
int MB_AddMessage(MBt_Board mb, void *msg) {
    
    int rc;
    void *ptr_new;
    pooled_list *pl;
    MBIt_Board  *board;
    
    /* if message board is null */
    if (mb == MB_NULL_MBOARD)
    {
        P_FUNCFAIL("Cannot add message to null board (MB_NULL_MBOARD)");
        return MB_ERR_INVALID;
    }
    
    /* if message pointer is null */
    if (msg == NULL)
    {
        P_FUNCFAIL("NULL pointer given as message");
        return MB_ERR_INVALID;
    }
    
    /* map handle to object */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    if (board == NULL) 
    {
        P_FUNCFAIL("Invalid board handle (%d)", (int)mb);
        return MB_ERR_INVALID;
    }
    
    /* do not add message if board is locked */
    if (board->locked == MB_TRUE)
    {
        P_FUNCFAIL("Board (%d) is locked", (int)mb);
        return MB_ERR_LOCKED;
    }
    
    /* make sure board is meant to be writeable */
    if (board->is_writer == MB_FALSE)
    {
        P_FUNCFAIL("Board access mode was set to non-writeable");
        return MB_ERR_DISABLED;
    }
    
    /* get access to internal data */
    pl = board->data;
    assert(pl != NULL);
    
    /* create new node in pooled list */
    rc = pl_newnode(pl, &ptr_new);
    if (rc == PL_ERR_MALLOC) 
    {
        P_FUNCFAIL("Could not allocate required memory");
        return MB_ERR_MEMALLOC;
    }
    else if (rc != PL_SUCCESS) 
    {
        P_FUNCFAIL("pl_newnode() returned with err code %d", rc);
        return MB_ERR_INTERNAL;
    }
    /* copy data into mboard node */
    memcpy(ptr_new, msg, (size_t)pl->elem_size);
    
    return MB_SUCCESS;
}
