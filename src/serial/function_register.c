/* $Id$ */
/*!
 * \file serial/function_register.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_Function_Register()
 * 
 */
#include "mb_serial.h"
#include <stdio.h>

/*!
 * \brief Registers a function
 * \ingroup MB_API
 * \param[out] fh_ptr Address of Function handle
 * \param[in] filterFunc Pointer to user-defined filter function
 * 
 * This function is now DEPRECATED.
 * 
 * It now does nothing apart from printing a deprecation notice
 * 
 * This function will return with ::MB_SUCCESS.
 */

int MB_Function_Register (
        MBt_Function *fh_ptr, 
        int (*filterFunc)(const void *msg, const void *params) 
        ) {
    
    /* acknowledge+hide "unused parameter" compiler warnings */
    ACKNOWLEDGED_UNUSED(fh_ptr);
    ACKNOWLEDGED_UNUSED(filterFunc);
    
    printf("[libmboard] MB_Function_Register() deprecated. Use MB_Filter_Create() instead.\n");  
    return MB_SUCCESS;
}
