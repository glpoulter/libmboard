/* $Id: stats.c 732 2008-03-10 15:55:53Z lsc $ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : stats.c
 *  
 */

#include "circles.h"
#include <math.h>
#define TOL 1e-7
void calculate_stats(void) {
    
    struct m_Force *msg = NULL;
    int id, id_mmove;
    double fx, fy;
    double total_move = 0.0;
    double movement, max_move = 0.0;
    
    /* loop thru messages */
    /* randomiseMessages_Force(); *//* randomisation not necessary */
    for (msg = getMessage_Force(); msg; msg = getMessage_Force())
    {
        /* load message data */
        id = msg->id; 
        fx = msg->fx;
        fy = msg->fy;
        
        movement = sqrt((fx * fx) + (fy * fy));
        total_move += movement;
        
        if (movement >= max_move) 
        {
            max_move = movement;
            id_mmove = id;
        }
    }
    
    PRINT("----------- ITER %d STATS -----------\n", iter);
    PRINT(" + Total movement: %f\n", total_move);
    PRINT(" + Max agent movement: %f\n", max_move);
    PRINT("-------------------------------------\n");
    PRINT("\n");
}
