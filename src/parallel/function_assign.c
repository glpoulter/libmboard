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

/* if compiling debug version, we need additional functions and datastructs */
#ifdef _EXTRA_CHECKS
    static void check_all_fh_equal(MBt_Function fh, void *params);
    struct fh_data {
        MBt_Function fh;
        void *params;
    };
#endif /*_EXTRA_CHECKS*/
    
/*!
 * \brief Assigns function handle to a message board
 * \ingroup MB_API
 * \param[in] mb Message Board handle
 * \param[in] fh Function handle
 * \param[in] params Pointer to input data that will be passed into filter function 
 * \param[in] param_size Size of input data (in bytes)
 * 
 * Function handles, param pointer and param sizes are stored within the
 * board object. 
 * 
 * If \c params is \c NULL, the \c param_size value is ignored.
 * 
 * This routine also updates the <tt>board->tagging</tt> flag -- ::MB_TRUE if \c fh
 * is not null (::MB_NULL_FUNCTION) or ::MB_FALSE otherwise.
 * 
 * Users can also remove a function assigned to a board by passing ::MB_NULL_FUNCTION
 * into \c fh.
 * 
 * On error, \c mb will remain unchanged and an appropriate error code is returned.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (invalid or null board/fh given, or, invalid param size) 
 *  - ::MB_ERR_LOCKED (\c mb is locked)
 */
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
