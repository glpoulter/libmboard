/* $Id: stresstest.c 732 2008-03-10 15:55:53Z lsc $ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : stresstest.c
 *  
 */

#include "stresstest.h"

/* define and initialise global vars */
int env_rank = 0;
int env_size = 1;

int main(int argc, char ** argv) {
    
    int rc;
    
    /* Set up the message board environment */
    if (setup_env(&argc, &argv) != OK)
    {
        complain_then_quit("Could not set up libmoard environment");
    }
    
    /* ============================================================ */
    
    rc = t_manyboards();
    if (rc != OK) 
    {
        PRINT(">> TEST FAILED\n");
        PRINT(">> We might have run out of memory. Try setting a lower\n");
        PRINT("   value for TP_MANYBOARD_COUNT\n");
        shutdown_env();
        exit(1);
    }
    
    /* ------------------------------------------------------------ */
    
    rc = t_manymsgs();
    if (rc != OK) 
    {
        PRINT(">> TEST FAILED\n");
        PRINT(">> We might have run out of memory. Try setting a lower\n");
        PRINT("   value for TP_MANYMSG_COUNT\n");
        shutdown_env();
        exit(1);
    }
    
    /* ============================================================ */
    
    /* clean up and quit */
    if (shutdown_env() != OK) exit(1);
    exit(0);
}
