/* $Id: function_assign.c 2113 2009-09-15 14:42:01Z lsc $ */
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
#include <stdio.h>

/*!
 * \brief Assigns function handle to a message board
 * \ingroup MB_API
 * \param[in] mb Message Board handle
 * \param[in] fh Function handle
 * \param[in] params Pointer to input data that will be passed into filter function 
 * \param[in] param_size Size of input data (in bytes)
 * 
 * This function is now DEPRECATED.
 * 
 * It now does nothing apart from printing a deprecation notice
 * 
 * This function will return with ::MB_SUCCESS.
 */
int MB_Function_Assign(MBt_Board mb, MBt_Function fh, 
        void *params, size_t param_size) {
    
    /* acknowledge+hide "unused parameter" compiler warnings */
    ACKNOWLEDGED_UNUSED(mb);
    ACKNOWLEDGED_UNUSED(fh);
    ACKNOWLEDGED_UNUSED(params);
    ACKNOWLEDGED_UNUSED(param_size);
    
    fprintf(stderr, "[libmboard] MB_Function_Assign() deprecated. Use MB_Filter_Assign() instead\n");
    return MB_SUCCESS;
}

