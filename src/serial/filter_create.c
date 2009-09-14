/* $Id$ */
/*!
 * \file serial/filter_create.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_Filter_Create()
 * 
 */

#include "mb_serial.h"

/*!
 * \brief Creates a Filter object based on a function
 * \ingroup MB_API
 * \param[out] ft_ptr Address of Filter handle
 * \param[in] filterFunc Pointer to user-defined filter function
 * 
 * This routine does nothing apart from setting the return 
 * filter handle to ::MB_NULL_FILTER (message tagging no needed 
 * in serial library).
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 */
int MB_Filter_Create(MBt_Filter *ft_ptr, 
        int (*filterFunc)(const void *msg, int pid) ) {
    
    *ft_ptr = MB_NULL_FILTER;
    
    return MB_SUCCESS;
}
