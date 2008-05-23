/* $Id$ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : utils.c
 *  
 */

#include "stresstest.h"

void complain_then_quit(char * msg) {
    
    /* complain */
    fprintf(stderr, "\nERROR: ");
    fprintf(stderr, msg);
    fprintf(stderr, "\n -- Quitting -- \n");
    
    /* clean up */
    shutdown_env();
    
    /* quit */
    exit(1);
}

int setup_env(int * argc, char *** argv) {
    
#ifdef _PARALLEL
    /* Set up MPI environment */
    MPI_Init(argc, argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &env_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &env_size);
#endif
    
    /* Set up mboard enviromnent */
    if (MB_Env_Init() != MB_SUCCESS) return FAIL;
    
    return OK;
}

int shutdown_env(void) {
        
#ifdef _PARALLEL
    MPI_Finalize();
#endif
    
    /* clean up */
    if (MB_Env_Finalise() != MB_SUCCESS) return FAIL;
    
    return OK;
    
}
