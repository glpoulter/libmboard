/* $Id$ */
/*!
 * \file memlog.c
 * 
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Aug 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Routines for instrumenting memory usage
 * 
 */

#include "mb_memlog.h"
#define MAX_FILENAME_LENGTH 128

FILE *memlog_file;
char memlog_filename[MAX_FILENAME_LENGTH];

void memlog_init(void) {
        
    /* determine output filename */
    snprintf(memlog_filename, MAX_FILENAME_LENGTH, "memlog-%d_of_%d.txt", 
             MBI_CommRank, MBI_CommSize - 1);
    
    /* open file */
    memlog_file = fopen(memlog_filename, "w");
    if (memlog_file == NULL)
    {
        fprintf(stderr, "ERROR: Could not open %s for writing.\n", memlog_filename);
        exit(1);
    }
    assert(memlog_file != NULL);
    
    if (MBI_CommRank == 0)
    {
        printf("====================================================\n");
        printf("           libmboard memory instrumentation         \n");
        printf("----------------------------------------------------\n");
        printf(" The message board library (libmboard) used by this \n");
        printf(" program was compiled with memory instrumentation   \n");
        printf(" enabled.\n\n");
        
        printf(" This instrumentation is useful for exploring the   \n");
        printf(" memory requirements and usage of your program, but \n");
        printf(" do be aware that it may have a signifcation impact \n");
        printf(" on performance.\n\n");
        
        printf(" Memory usage logs will be written to memlog-*.txt  \n");
        printf("----------------------------------------------------\n");
    }

}

void memlog_finalise(void) {
    assert(memlog_file != NULL);
    fclose(memlog_file);
    
    printf("--> [%d] MEMLOG output written to %s\n", MBI_CommRank, memlog_filename);
}
