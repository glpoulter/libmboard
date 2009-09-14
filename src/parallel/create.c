/* $Id$ */
/*!
 * \file parallel/create.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Mar 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Create()
 * 
 */
#include "mb_parallel.h"

/* function prototype for internal routine used within this file only */
inline static int newBoardObj(MBt_handle *mb, size_t);

/* if Extra Checks are required, we need these datastructures as well */
#ifdef _EXTRA_CHECKS
#ifdef _UNSAFE_CHECKS
    static void check_all_mb_equal(MBt_Board mb, size_t msgsize);
    
    struct boardData_t {
        MBt_Board handle;
        size_t    msgsize;
    };
#endif /*_UNSAFE_CHECKS */
#endif /*_EXTRA_CHECKS*/

/*!
 * \brief Instantiates a new MessageBoard
 * \ingroup MB_API
 * \param[out] mb_ptr Address of MessageBoard handle
 * \param[in] msgsize Size of message that this MessageBoard will be used for
 * 
 * The MessageBoard object is allocated and registered  with the ::MBI_OM_mboard
 * ObjectMap. The reference ID returned by ObjectMap is then written to 
 * \c mb_ptr as the handle.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 *  - ::MB_ERR_INVALID (invalid or null board given) 
 *  - ::MB_ERR_MEMALLOC (error allocating memory for memory pool or ObjectMap entry)
 *  - ::MB_ERR_INTERNAL (possible bug. Recompile and run in debug mode for hints)
 *  - ::MB_ERR_ENV (MessageBoard environment not yet initialised)
 */
int MB_Create(MBt_Board *mb_ptr, size_t msgsize) {
    
    int rc;
    MBt_Board mb = MB_NULL_MBOARD;
    
    /* first, set board to NULL in case we quit on error */
    *mb_ptr = MB_NULL_MBOARD;
    
    /* check for invalid input size */
    if ((int)msgsize <= 0) 
    {
        P_FUNCFAIL("Invalid message size (%d)", (int)msgsize);
        return MB_ERR_INVALID;
    }
    
    /* create board object */
    rc = newBoardObj(&mb, msgsize);
    assert(rc == MB_SUCCESS);
    *mb_ptr = mb;
    
    if ((int)mb > MBI_MAX_BOARDS) 
    {
        P_FUNCFAIL("Too many boards created. Try recompiling libmboard "
                   "with -DOBJMAP_CYCLE_KEY");
        return MB_ERR_OVERFLOW;
    }
    assert((int)mb <= MBI_MAX_BOARDS);
    
    /* debug: make sure same mb on all procs */
#ifdef _EXTRA_CHECKS
#ifdef _UNSAFE_CHECKS
    check_all_mb_equal(mb, msgsize);
#endif /* _UNSAFE_CHECKS */
#endif /*_EXTRA_CHECKS*/
    
    P_INFO("Created Board (%d, msgsize: %d)", (int)mb, (int)msgsize);
    
    return MB_SUCCESS;
}

/*! \brief Creates board object
 * \param[in] msgsize Size of message that this MessageBoard will be used for
 * \param[out] mb_ptr Pointer to created board object
 * 
 * This routine is only used by MB_Create(). It creates a new board object
 * and returns a pointer to the object.
 * 
 */
inline static int newBoardObj(MBt_Board *mb_ptr, size_t msgsize) {
    
    int rc, i, next;
    OM_key_t rc_om;
    MBIt_Board *mb_obj;
    
    /* allocate message board object */
    mb_obj = (MBIt_Board *)malloc(sizeof(MBIt_Board));
    assert(mb_obj != NULL);
    if (mb_obj == NULL) /* on error */
    {
        P_FUNCFAIL("Could not allocate required memory");
        return MB_ERR_MEMALLOC;
    }
    
    /* initialise control flags */
    mb_obj->locked        = MB_FALSE; 
    mb_obj->syncCompleted = MB_FALSE; 
    
    /* set cursor indicating board has not been synced */
    mb_obj->synced_cursor = 0;
    
    /* initialise message tagging function data */
    mb_obj->filter = NULL;
    mb_obj->tt = NULL;

    /* initialise flags and variables for reader/writer accounting */
    mb_obj->is_reader = MB_TRUE; 
    mb_obj->is_writer = MB_TRUE;
    mb_obj->reader_count = MBI_CommSize - 1; /* All remote nodes are readers */
    mb_obj->writer_count = MBI_CommSize - 1; /* All remote nodes are writers */
    
    mb_obj->reader_list = (int*)malloc(sizeof(int) * MBI_CommSize);
    assert(mb_obj->reader_list != NULL);
    if (mb_obj->reader_list == NULL)
    {
        P_FUNCFAIL("Could not allocate required memory");
        free(mb_obj);
        return MB_ERR_MEMALLOC;
    }
    
    mb_obj->writer_list = (int*)malloc(sizeof(int) * MBI_CommSize);
    assert(mb_obj->writer_list != NULL);
    if (mb_obj->writer_list == NULL)
    {
        P_FUNCFAIL("Could not allocate required memory");
        free(mb_obj->reader_list);
        free(mb_obj);
        return MB_ERR_MEMALLOC;
    }
    
    for (i = 0, next = 0; i < MBI_CommSize - 1; i++, next++)
    {
        if (next == MBI_CommRank) next++;
        mb_obj->reader_list[i] = mb_obj->writer_list[i] =next;
    }
    
    /* allocate pthread mutex and conditional var */
    rc = pthread_mutex_init(&(mb_obj->syncLock), NULL);
    assert(0 == rc);
    if (rc != 0) 
    {
        P_FUNCFAIL("pthread_mutex_init(board->syncLock) returned "
                   "err code %d", rc);
        free(mb_obj->reader_list);
        free(mb_obj->writer_list);
        free(mb_obj);
        return MB_ERR_INTERNAL;
    }
    rc = pthread_cond_init(&(mb_obj->syncCond), NULL);
    assert(0 == rc);
    if (rc != 0) 
    {
        P_FUNCFAIL("pthread_cond_init(board->syncCond) returned "
                   "err code %d", rc);
        pthread_mutex_destroy(&(mb_obj->syncLock));
        free(mb_obj);
        return MB_ERR_INTERNAL;
    }
    
    /* allocate pooled list */
    rc = pl_create(&(mb_obj->data), msgsize, (int)MBI_CONFIG.mempool_blocksize);
    if (rc != PL_SUCCESS)
    {
        free(mb_obj->reader_list);
        free(mb_obj->writer_list);
        pthread_cond_destroy(&(mb_obj->syncCond));
        pthread_mutex_destroy(&(mb_obj->syncLock));
        free(mb_obj);
        
        if (rc == PL_ERR_MALLOC) 
        {
            P_FUNCFAIL("Could not allocate required memory");
            return MB_ERR_MEMALLOC;
        }
        else 
        {
            P_FUNCFAIL("pl_create(board->data) returned err code %d", rc);
            return MB_ERR_INTERNAL;
        }
    }
    
    
    /* register new mboard object */
    assert(MBI_OM_mboard != NULL);
    assert(MBI_OM_mboard->type == OM_TYPE_MBOARD);
    rc_om = MBI_objmap_push(MBI_OM_mboard, (void*)mb_obj);
    if (rc_om > OM_MAX_INDEX)
    {
        if (rc_om == OM_ERR_MEMALLOC)
        {
            free(mb_obj->reader_list);
            free(mb_obj->writer_list);
            pthread_cond_destroy(&(mb_obj->syncCond));
            pthread_mutex_destroy(&(mb_obj->syncLock));
            free(mb_obj);
            P_FUNCFAIL("Could not allocate required memory");
            return MB_ERR_MEMALLOC;
        }
        else
        {
            P_FUNCFAIL("MBI_objmap_push() returned err code %d", rc_om);
            return MB_ERR_INTERNAL;
        }
    }
    
    /* assign handle to new mboard */
    *mb_ptr  = (MBt_Board)rc_om;
    
    return MB_SUCCESS;
}


#ifdef _EXTRA_CHECKS
#ifdef _UNSAFE_CHECKS
/*! \brief Checks that board is created with same msgsize across all procs 
 * \param[in] msgsize Message size
 * \param[in] mb Board Handle
 * 
 * Only used when _EXTRA_CHECKS is defined (used within the debug version of 
 * libmboard).
 * 
 */
static void check_all_mb_equal(MBt_Board mb, size_t msgsize) {
    
    int rc;
    struct boardData_t bdata; 
    
    if (MASTERNODE)
    {
        bdata.handle  = mb;
        bdata.msgsize = msgsize;
    }
    
    rc = MPI_Bcast(&bdata, (int)sizeof(struct boardData_t), MPI_BYTE, 0, MBI_CommWorld);
    
    assert(rc == MPI_SUCCESS);
    assert(bdata.handle  == mb);
    assert(bdata.msgsize == msgsize);
}
#endif /* _UNSAFE_CHECKS */
#endif /*_EXTRA_CHECKS*/
