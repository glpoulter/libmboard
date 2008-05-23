/* $Id$ */
/*!
 * \file parallel/env_initialised.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_Env_Initialised()
 * 
 */

#include "mb_parallel.h"

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
