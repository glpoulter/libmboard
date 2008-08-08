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

static int newBoardObj(MBt_handle *mb, size_t);

#ifdef _EXTRA_CHECKS
    static void check_all_mb_equal(MBt_Board mb, size_t msgsize);
    
    struct boardData_t {
        MBt_Board handle;
        size_t    msgsize;
    };
    
#endif /*_EXTRA_CHECKS*/

int MB_Create(MBt_Board *mb_ptr, size_t msgsize) {
    
    int rc;
    MBt_Board mb = MB_NULL_MBOARD;
    
    /* first, set board to NULL in case we quit on error */
    *mb_ptr = MB_NULL_MBOARD;
    
    /* check for invalid input size */
    if ((int)msgsize <= 0) return MB_ERR_INVALID;
    
    /* create board object */
    rc = newBoardObj(&mb, msgsize);
    assert(rc == MB_SUCCESS);
    *mb_ptr = mb;
    
    assert((int)mb <= MBI_MAX_BOARDS);
    if ((int)mb > MBI_MAX_BOARDS) return MB_ERR_OVERFLOW;
    
    /* debug: make sure same mb on all procs */
#ifdef _EXTRA_CHECKS
    check_all_mb_equal(mb, msgsize);
#endif /*_EXTRA_CHECKS*/
    
    return MB_SUCCESS;
}


static int newBoardObj(MBt_Board *mb_ptr, size_t msgsize) {
    
    int rc;
    OM_key_t rc_om;
    MBIt_Board *mb_obj;
    
    /* allocate message board object */
    mb_obj = (MBIt_Board *)malloc(sizeof(MBIt_Board));
    assert(mb_obj != NULL);
    if (mb_obj == NULL) /* on error */
    {
        return MB_ERR_MEMALLOC;
    }
    
    /* initialise control flags */
    mb_obj->locked        = MB_FALSE; 
    mb_obj->syncCompleted = MB_FALSE; 
    
    /* initialise message tagging function data */
    mb_obj->fh = MB_NULL_FUNCTION;
    mb_obj->tagging = MB_FALSE;
    mb_obj->fparams_size = 0;
    mb_obj->fparams = NULL;
    mb_obj->tt = NULL;

    
    /* allocate pthread mutex and conditional var */
    rc = pthread_mutex_init(&(mb_obj->syncLock), NULL);
    assert(0 == rc);
    if (rc != 0) 
    {
        free(mb_obj);
        return MB_ERR_INTERNAL;
    }
    rc = pthread_cond_init(&(mb_obj->syncCond), NULL);
    assert(0 == rc);
    if (rc != 0) 
    {
        pthread_mutex_destroy(&(mb_obj->syncLock));
        free(mb_obj);
        return MB_ERR_INTERNAL;
    }
    
    /* allocate pooled list */
    rc = pl_create(&(mb_obj->data), msgsize, MB_CONFIG_PARALLEL_POOLSIZE);
    if (rc != PL_SUCCESS)
    {
        free(mb_obj);
        if (rc == PL_ERR_MALLOC) return MB_ERR_MEMALLOC;
        else return MB_ERR_INTERNAL;
    }
    
    
    /* register new mboard object */
    assert(MBI_OM_mboard != NULL);
    assert(MBI_OM_mboard->type == OM_TYPE_MBOARD);
    rc_om = MBI_objmap_push(MBI_OM_mboard, (void*)mb_obj);
    if (rc_om > OM_MAX_INDEX)
    {
        if (rc_om == OM_ERR_MEMALLOC)
        {
            return MB_ERR_MEMALLOC;
        }
        else
        {
            return MB_ERR_INTERNAL;
        }
    }
    
    /* assign handle to new mboard */
    *mb_ptr  = (MBt_Board)rc_om;
    
    return MB_SUCCESS;
}


#ifdef _EXTRA_CHECKS


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

#endif /*_EXTRA_CHECKS*/
