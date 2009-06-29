/* $Id$ */
/*!
 * \file serial/filter_delete.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_Filter_Delete()
 * 
 */

#include "mb_serial.h"

/*!
 * \brief Deletes a filter object
 * \ingroup MB_API
 * \param[in,out] ft_ptr Address of Filter handle
 * 
 * This routine does nothing apart from setting fh to ::MB_NULL_FUNCTION 
 * (message tagging no needed in serial library).
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 */
int MB_Filter_Delete(MBt_Filter *ft_ptr) {
    *ft_ptr = MB_NULL_FUNCTION;
    P_WARNING("Serial version of MB_Filter_Delete() does nothing interesing");
    return MB_SUCCESS;
}
