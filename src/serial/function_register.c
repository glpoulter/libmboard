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

/*!
 * \brief Registers a function
 * \ingroup MB_API
 * \param[out] fh_ptr Address of Function handle
 * \param[in] filterFunc Pointer to user-defined filter function
 * 
 * This routine does nothing (message tagging no needed in serial library).
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 */

int MB_Function_Register (
        MBt_Function *fh_ptr, 
        int (*filterFunc)(const void *msg, const void *params) 
        ) {
    
    return MB_SUCCESS;
}
