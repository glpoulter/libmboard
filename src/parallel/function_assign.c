/* $Id$ */
/*!
 * \file parallel/function_assign.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Function_Assign()
 * 
 */

#include "mb_parallel.h"

#ifdef _EXTRA_CHECKS
    static void check_all_fparamsize_equal(MBt_Function fh, size_t size);
    
    struct fpdata_t {
        MBt_Function fh;
        size_t size;
    };
    
#endif /*_EXTRA_CHECKS*/
    
/* Assign function handle to a message board */
int MB_Function_Assign(MBt_Board mb, MBt_Function fh, 
        void *params, size_t param_size) {
    
    MBIt_Board  *board;
    void *obj;
    
    /* if params not NULL, check param_size is valid */
    if (fh != MB_NULL_FUNCTION 
        && params != NULL 
        && (int)param_size < 1) return MB_ERR_INVALID;
    
    /* check that mb is valid */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    if (board == NULL) return MB_ERR_INVALID;
    
    /* check that fh is valid (NULL function is valid) */
    if (fh != MB_NULL_FUNCTION)
    {
        obj = MBI_getFunctionRef(fh);
        if (obj == NULL) return MB_ERR_INVALID;
        board->tagging = MB_TRUE;
    }
    
    /* make sure board is not locked */
    if (board->locked == MB_TRUE) return MB_ERR_LOCKED;
    
    /* debug: make sure same fparam size on all procs */
#ifdef _EXTRA_CHECKS
    check_all_fparamsize_equal(fh, param_size);
#endif /*_EXTRA_CHECKS*/
    
    /* assign fh and params to board */
    if (fh == MB_NULL_FUNCTION)
    {
        board->fh = MB_NULL_FUNCTION;
        board->fparams = NULL;
        board->fparams_size = 0;
        board->tagging = MB_FALSE;
    }
    else
    {
        board->fh = fh;
        board->fparams = params;
        board->fparams_size = param_size;
        board->tagging = MB_TRUE;
    }
    
    return MB_SUCCESS;
}


#ifdef _EXTRA_CHECKS
    static void check_all_fparamsize_equal(MBt_Function fh, size_t size) {
        
        int rc;
        struct fpdata_t fpdata; 
        
        if (MASTERNODE)
        {
            fpdata.fh   = fh;
            fpdata.size = size;
        }
        
        rc = MPI_Bcast(&fpdata, (int)sizeof(struct fpdata_t), MPI_BYTE, 0, MBI_CommWorld);
        assert(rc == MPI_SUCCESS);
        assert(fpdata.fh == fh);
        assert(fpdata.size == size);
    }
#endif /*_EXTRA_CHECKS*/
