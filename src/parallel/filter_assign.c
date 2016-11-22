/* $Id: filter_assign.c 2934 2012-07-27 14:08:11Z lsc $ */
/*!
 * \file parallel/filter_assign.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Filter_Assign()
 * 
 */

#include "mb_parallel.h"

/*!
 * \brief Assigns fitler handle to a message board
 * \ingroup MB_API
 * \param[in] mb Message Board handle
 * \param[in] fh Filter handle
 * 
 * Users can also remove a filters assigned to a board by passing ::MB_NULL_FILTER
 * into \c fh.
 * 
 * On error, \c mb will remain unchanged and an appropriate error code is returned.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb or \c fh is either NULL or invalid) 
 *  - ::MB_ERR_LOCKED (\c mb is locked)
 */
int MB_Filter_Assign(MBt_Board mb, MBt_Filter fh) {
    
    MBIt_Board  *board;
    MBIt_filterfunc_wrapper *fwrap;

    if (mb == MB_NULL_MBOARD)
    {
    	P_FUNCFAIL("Cannot assign filter to null board");
    	return MB_ERR_INVALID;
    }
    
    /* get reference to board object */
    assert(MBI_OM_mboard != NULL);
    assert(MBI_OM_mboard->type == OM_TYPE_MBOARD);
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    if (board == NULL)
    {
    	P_FUNCFAIL("Unknown board handle (%d)", (int)mb);
    	return MB_ERR_INVALID;
    }
    if (board->locked == MB_TRUE)
    {
    	P_FUNCFAIL("Board (%d) is locked", (int)mb);
    	return MB_ERR_LOCKED;
    }
    if (fh == MB_NULL_FILTER)
    {
    	P_INFO("Removing filters from board %d", (int)mb);
        board->filter  = (MBIt_filterfunc)NULL;
    }
    else
    {
    	P_INFO("Filter %d assigned to board %d", (int)fh, (int)mb);
        /* get reference to filter object */
        assert(MBI_OM_filter != NULL);
        assert(MBI_OM_filter->type == OM_TYPE_FILTER);
        fwrap = (MBIt_filterfunc_wrapper *)MBI_getFilterRef(fh);
        if (fwrap == NULL) return MB_ERR_INVALID;
        /* assign function pointer to board */
        board->filter  = fwrap->func;
    }

    return MB_SUCCESS;
}
