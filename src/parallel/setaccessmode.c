/* $Id$ */
/*!
 * \file parallel/setaccessmode.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Aug 2009
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_SetAccessMode()
 * 
 */
#include "mb_parallel.h"

#ifdef _EXTRA_CHECKS
#define IDENT_CHANGEACCESSMODE 1972356
    static void check_all_mb_equal(OM_key_t key);
    
    struct amdata_t {
        OM_key_t handle;
        int ident; 
    };
#endif /*_EXTRA_CHECKS*/
    
/*!
 * \brief Sets access mode of the Message Board
 * \ingroup MB_API
 * \param[in] mb Message Board handle
 * \param[in] MODE Access mode identifier
 * 
 * Sets the \c board->is_reader and \c board->is_writer flags.
 * 
 * Collectively builds \c board->reader_list and \c board->writer_list.
 * \c board->reader_count and \c board->writer_count are set accordingly.
 *   
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c is null or invalid, or \c MODE is invalid)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_LOCKED (\c mb is locked by another process)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 *  - ::MB_ERR_MPI (error when calling MPI routines)
 *  - ::MB_ERR_NOTREADY (board is not empty)
 */
int MB_SetAccessMode(MBt_Board mb, int MODE) {

    int i, rc, mode;
    int *mode_array;
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
    
    /* board integrity check check */
    assert(board->reader_list != NULL);
    assert(board->writer_list != NULL);
    
    /* sync access modes across procs */
    /* TODO: check that all procs running for the same board */
    mode_array = (int*)malloc(sizeof(int) * MBI_CommSize);
    assert(mode_array != NULL);
    if (mode_array == NULL) 
    {
        P_FUNCFAIL("Could not allocate required memory");
        return MB_ERR_MEMALLOC;
    }
    
    /* debug: make sure same mb on all procs */
#ifdef _EXTRA_CHECKS
    check_all_mb_equal((OM_key_t)mb);
#endif /*_EXTRA_CHECKS*/
    
    /* gather mode of all remote procs */
    mode = MODE;
    rc = MPI_Allgather(&mode, 1, MPI_INT, 
                       mode_array, 1, MPI_INT, MBI_CommWorld);
    if (rc != MPI_SUCCESS)
    {
        P_FUNCFAIL("MPI_Allgather return with err code %d", rc);
        return MB_ERR_MPI;
    }
    
    /* set flags */
    switch (MODE) 
    {
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
    
    /* start populating internal counters and lists */
    board->reader_count = 0;
    board->writer_count = 0;
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank) continue;

        if (board->is_reader == MB_TRUE && (mode_array[i] == MB_MODE_READWRITE ||
                                            mode_array[i] == MB_MODE_WRITEONLY ))
            board->writer_list[board->writer_count++] = i;

        if (board->is_writer == MB_TRUE && (mode_array[i] == MB_MODE_READWRITE ||
                                            mode_array[i] == MB_MODE_READONLY ))
            board->reader_list[board->reader_count++] = i;
    }
    
    free(mode_array);
    
    return MB_SUCCESS;
}

#ifdef _EXTRA_CHECKS
static void check_all_mb_equal(OM_key_t key) {
    
    int rc;
    struct amdata_t data; 
    
    if (MASTERNODE)
    {
        data.handle = key;
        data.ident  = IDENT_CHANGEACCESSMODE;
    }
    
    rc = MPI_Bcast(&data, (int)sizeof(struct amdata_t), MPI_BYTE, 0, MBI_CommWorld);
    assert(rc == MPI_SUCCESS);
    assert(data.handle == key);
    assert(data.ident  == IDENT_CHANGEACCESSMODE);
}
#endif /*_EXTRA_CHECKS*/
