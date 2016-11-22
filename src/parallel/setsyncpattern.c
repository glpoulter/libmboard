/* $Id: setsyncpattern.c 2128 2009-09-17 14:17:21Z lsc $ */
/*!
 * \file parallel/setsyncpattern.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Sept 2009
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_SetSyncPattern()
 * 
 */
#include "mb_parallel.h"

#ifdef _EXTRA_CHECKS
#include "mb_commqueue.h"
#include "mb_syncqueue.h"
#define IDENT_CHANGESYNCPATTERN 187512
    static void check_all_mb_equal(OM_key_t key);
    
    struct spdata_t {
        OM_key_t handle;
        int ident; 
    };
#endif /*_EXTRA_CHECKS*/
    
#define READ_FROM(matrix, pid) matrix[(MBI_CommSize * MBI_CommRank) + pid]
#define SEND_TO(matrix, pid) matrix[(MBI_CommSize * pid) + MBI_CommRank]


/*!
 * \brief Sets sync pattern of the Message Board
 * \ingroup MB_API
 * \param[in] mb Message Board handle
 * \param[in] sync_matrix Integer array representing board communication matrix
 * 
 * The master node (node 0) should supply a \c sync_matrix of size 
 * MBI_CommSize^2 (the square of the number of MPI nodes). The first row 
 * will represent the amount of communication from each node to node 0, the 
 * second row for node 1, and so on. 
 * 
 * \c sync_matrix will be ignored on other nodes.
 * 
 * The master node will distribute the necessary information to all other nodes
 * such that every node can populate the following information:
 *  - reader_list
 *  - writer_list
 *  - reader_count
 *  - writer_count
 *  - is_reader
 *  - is_writer
 *   
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb is null or invalid, or \c sync_matrix is null)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_LOCKED (\c mb is locked by another process)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 *  - ::MB_ERR_MPI (error when calling MPI routines)
 *  - ::MB_ERR_NOTREADY (board is not empty)
 */
int MB_SetSyncPattern(MBt_Board mb, unsigned int *sync_matrix) {
    
    int i, rc;
    int count;
    MBIt_Board *board;
    unsigned int *matrix;
    
    assert(MBI_CommQueue_isEmpty() && MBI_SyncQueue_isEmpty());
    
    if (mb == MB_NULL_MBOARD)
    {
        P_FUNCFAIL("Cannot set access mode null board (MB_NULL_MBOARD)");
        return MB_ERR_INVALID;
    }
    
    if (MASTERNODE && sync_matrix == NULL)
    {
        P_FUNCFAIL("sync_matrix pointer is NULL on master node. Deadlock may occur (see docs)");
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
    
    /* debug: make sure same mb on all procs */
#ifdef _EXTRA_CHECKS
    check_all_mb_equal((OM_key_t)mb);
#endif /*_EXTRA_CHECKS*/
    
    count = MBI_CommSize * MBI_CommSize;
    if (MASTERNODE)
    {
        matrix = sync_matrix;
    }
    else
    {
        matrix = (unsigned int*)malloc(sizeof(unsigned int) * count);
    }
    
    /* broadcast sync_matrix to all procs */
    rc = MPI_Bcast(matrix, count, MPI_UNSIGNED, 0, MBI_CommWorld);
    assert(rc == MPI_SUCCESS);
    if (rc != MPI_SUCCESS)
    {
        P_FUNCFAIL("MPI_Bcast() return error code %d", rc);
        return MB_ERR_MPI;
    }
    
    /* build reader/writer list */
    assert(board->writer_list != NULL);
    assert(board->reader_list != NULL);
    board->writer_count = 0;
    board->reader_count = 0;
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank) continue;
        
        if (READ_FROM(matrix, i) != 0) 
            board->writer_list[board->writer_count++] = i;
        if (SEND_TO(matrix, i) != 0) 
            board->reader_list[board->reader_count++] = i;
    }
    
    /* is reader/writer ? */
    if (board->writer_count == 0 && SEND_TO(matrix, MBI_CommRank) == 0)
        board->is_reader = MB_FALSE;
    else
        board->is_reader = MB_TRUE;
    
    if (board->reader_count == 0 && SEND_TO(matrix, MBI_CommRank) == 0)
        board->is_writer = MB_FALSE;
    else
        board->is_writer = MB_TRUE;
    
    /* clean up */
    if (!MASTERNODE) free(matrix);
    
    return MB_SUCCESS;
}

#ifdef _EXTRA_CHECKS
static void check_all_mb_equal(OM_key_t key) {
    
    int rc;
    struct spdata_t data; 
    
    if (MASTERNODE)
    {
        data.handle = key;
        data.ident  = IDENT_CHANGESYNCPATTERN;
    }
    
    rc = MPI_Bcast(&data, (int)sizeof(struct spdata_t), MPI_BYTE, 0, MBI_CommWorld);
    assert(rc == MPI_SUCCESS);
    assert(data.handle == key);
    assert(data.ident  == IDENT_CHANGESYNCPATTERN);
}
#endif /*_EXTRA_CHECKS*/
