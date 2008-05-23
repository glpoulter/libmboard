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

/*!
 * \brief Deallocates a registered function
 * \ingroup MB_API
 * \param[in,out] fh_ptr Address of Function handle
 * 
 * This routine does nothing apart from setting fh to ::MB_NULL_FUNCTION 
 * (message tagging no needed in serial library).
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 */

int MB_Function_Free ( MBt_Function *fh_ptr ) {
    *fh_ptr = MB_NULL_FUNCTION;
    return MB_SUCCESS;
}
