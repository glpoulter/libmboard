/* $Id: env_initialised.c 687 2008-02-27 16:57:25Z lsc $ */
/*!
 * \file serial/env_initialised.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_Env_Initialised()
 * 
 */

#include "mb_serial.h"

/*!
 * \brief Indicates whether MB_Env_Init() has been called
 * \ingroup MB_API
 * 
 * 
 * This routine will return ::MB_SUCCESS if the environment has been initialised 
 * (::MBI_STATUS_initialised set to ::MB_TRUE), or ::MB_ERR_ENV otherwise.
 * 
 *  Possible return codes:
 *  - ::MB_SUCCESS 
 *  - ::MB_ERR_ENV 
 */
int MB_Env_Initialised(void) {
    if (MBI_STATUS_initialised == MB_TRUE) return MB_SUCCESS;
    else return MB_ERR_ENV;
}
