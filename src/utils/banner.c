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
    
    printf("==============================================================\n\n");
#ifdef _PARALLEL
    printf("             Message Board Library (PARALLEL)                 \n\n");
#else
    printf("              Message Board Library (SERIAL)                  \n\n");
#endif
#ifdef _EXTRA_CHECKS
    printf("            +++++ This is a DEBUG version +++++               \n\n");
#endif
#ifdef _EXTRA_INFO
    printf("            +++++ VERBOSE  output enabled +++++               \n\n");
#endif
#ifdef _LOG_MEMORY_USAGE
    printf("     >>>>> Instrumentation of MEMORY USAGE enabled <<<<<      \n");
    printf("       [ DO NOT USE THIS VERSION FOR PRODUCTION RUNS ]        \n\n");
#endif
#ifdef _COVERAGE_ANALYSIS
    printf("          >>>>> Compiled for COVERAGE ANALYSIS <<<<<          \n");
    printf("       [ DO NOT USE THIS VERSION FOR PRODUCTION RUNS ]        \n\n");
#endif

    printf("--------------------------------------------------------------\n\n");
    printf(" Version        : %s\n", PACKAGE_VERSION                         );
    printf(" Build date     : %s\n", INFO_CONFIG_DATE                        );
    printf(" Config options : %s\n", INFO_CONFIG_ARGS                        );
    printf("                                                              \n");
    printf(" Copyright (c) 2007-2009 STFC Rutherford Appleton Laboratory  \n");
    printf(" If you have any problems or enquiries, you can contact the   \n");
    printf(" library maintainer at <%s>\n\n", PACKAGE_BUGREPORT                );
    printf("==============================================================\n");
    
}
