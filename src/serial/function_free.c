/* $Id$ */
/*!
 * \file serial/function_free.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_Function_Free()
 * 
 */
#include "mb_serial.h"
#include <stdio.h>

/*!
 * \brief Deallocates a registered function
 * \ingroup MB_API
 * \param[in,out] fh_ptr Address of Function handle
 * 
 * This function is now DEPRECATED.
 * 
 * It now does nothing apart from printing a deprecation notice
 * 
 * This function will return with ::MB_SUCCESS.
 */

int MB_Function_Free ( MBt_Function *fh_ptr ) {
    
    printf("[libmboard] MB_Function_Free() deprecated. Use MB_Filter_Delete() instead.\n");
    return MB_SUCCESS;
}
