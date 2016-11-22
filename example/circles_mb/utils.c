/* $Id: utils.c 880 2008-05-22 10:53:05Z lsc $ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : utils.c
 *  
 */

#include "circles.h"

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
    #endif /* _PARALLEL */
    
    /* Set up mboard enviromnent */
    if (MB_Env_Init() != MB_SUCCESS) return FAIL;


    /* allocate memory for partition data */
    partData = malloc(sizeof(double[4]) * env_size);
    
    if (createBoards() != OK) return FAIL;
    if (registerFunctions() != OK) return FAIL;
    if (assignFunctionsToBoards() != OK) return FAIL;
    
    return OK;
}

int shutdown_env(void) {
        
    free(partData);
    
    /* Delete boards */
    deleteBoards();
    
    /* clean up */
    if (MB_Env_Finalise() != MB_SUCCESS) return FAIL;
    
#ifdef _PARALLEL
    MPI_Finalize();
#endif /* _PARALLEL */
    
    return OK;
    
}

void print_env_data(void) {
    
    /* partition data */
    printf("[%d] partition: (%g, %g) -> (%g, %g)\n", env_rank, 
            partData[env_rank][XMIN],
            partData[env_rank][YMIN],
            partData[env_rank][XMAX],
            partData[env_rank][YMAX]
            );
    
    /* agent count */
    printf("[%d] agent count: %d\n", env_rank, env_agent_count);
}
