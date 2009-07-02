/* $Id$ */
/*!
 * \file serial/env_finalised.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_Env_Finalised()
 * 
 */

/*!
 * \brief Indicates whether MB_Env_Finalise() has been called
 * \ingroup MB_API
 * 
 * 
 * This routine will return ::MB_SUCCESS if the environment has been finalised 
 * (::MBI_STATUS_finalised set to ::MB_TRUE), or ::MB_ERR_ENV otherwise.
 * 
 *  Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_ENV
 */
#include "mb_serial.h"

int MB_Env_Finalised(void) {
    
    if (MBI_STATUS_finalised == MB_TRUE) return MB_SUCCESS;
    else return MB_ERR_ENV;
}
