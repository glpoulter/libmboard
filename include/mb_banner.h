/* $Id$ */
/*!
 * \file mb_banner.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Header file for banner routine
 * 
 */
#ifndef MB_BANNER_H_
#define MB_BANNER_H_

#include <stdio.h>
#include "mb_config.h"
#ifdef _PARALLEL
#include "mb_parallel.h"
#else
#include "mb_serial.h"
#endif
#include "mb_common.h"

/* .... see src/utils/banner.c .... */
void MBI_print_banner(void);

#endif /* MB_BANNER_H_ */
