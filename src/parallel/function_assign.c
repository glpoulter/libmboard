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
    static void check_all_fh_equal(MBt_Function fh, void *params);
    struct fh_data {
        MBt_Function fh;
        void *params;
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
    check_all_fh_equal(fh, params);
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
        board->fparams_size = (params == NULL) ? 0 : param_size;
        board->tagging = MB_TRUE;
    }
    
    return MB_SUCCESS;
}


#ifdef _EXTRA_CHECKS
    static void check_all_fh_equal(MBt_Function fh, void *params) {
        
        int rc;
        struct fh_data fh_ext;
        
        if (MASTERNODE) 
        {
            fh_ext.fh = fh;
            fh_ext.params = params;
        }
        
        rc = MPI_Bcast(&fh_ext, (int)sizeof(struct fh_data), MPI_BYTE, 0, MBI_CommWorld);
        assert(rc == MPI_SUCCESS);
        assert(fh_ext.fh == fh);
        if (params == NULL)
        {
            assert(fh_ext.params == NULL);
        }
    }
#endif /*_EXTRA_CHECKS*/
