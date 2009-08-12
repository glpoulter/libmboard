/* $Id$ */
/*!
 * \file banner.c
 * 
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : May 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Functions to print information about this package
 * 
 */
#include "mb_banner.h"

void MBI_print_banner(void) {

    /* only master proc should print the banner */
    if (MBI_CommRank != 0) return;
    
#ifdef _PARALLEL
    printf("[libmboard] Version        : %s (PARALLEL)\n", PACKAGE_VERSION   );
#else
    printf("[libmboard] Version        : %s (SERIAL)\n", PACKAGE_VERSION     );
#endif
    printf("[libmboard] Build date     : %s\n", INFO_CONFIG_DATE             );
    printf("[libmboard] Config options : %s\n", INFO_CONFIG_ARGS             );
    printf("\n");

#ifdef _UNSAFE_CHECKS
    printf("[libmboard] +++ Unsafe checks enabled +++\n");
#endif
#ifdef _EXTRA_CHECKS
    printf("[libmboard] +++ This is a DEBUG version +++\n");
#endif
#ifdef _PRINT_WARNINGS
    printf("[libmboard] +++ Warning messages enabled +++\n");
#endif
#ifdef _EXTRA_INFO
    printf("[libmboard] +++ VERBOSE output enabled +++\n");
#endif
#ifdef _LOG_MEMORY_USAGE
    printf("[libmboard] +++ Instrumentation of MEMORY USAGE enabled +++\n");
    printf("[libmboard] !!! DO NOT USE THIS VERSION FOR PRODUCTION RUNS !!!\n");
#endif
#ifdef _COVERAGE_ANALYSIS
    printf("[libmboard] +++ Compiled for COVERAGE ANALYSIS  +++\n");
    printf("[libmboard] !!! DO NOT USE THIS VERSION FOR PRODUCTION RUNS !!!\n");
#endif
    
}
