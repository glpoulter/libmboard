/* $Id: agent_functions.c 879 2008-05-21 16:23:37Z lsc $ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : agent_functions.c
 *  
 */

#include "circles.h"
#include <math.h>

#define CONST_KR 0.1 /* Stiffness variable for repulsion */
#define CONST_KA 0.0 /* Stiffness variable for attraction */

void af_postLocation(void) {
    int id;
    double x, y;
    
    /* load agent data */
    id = agent_get_id();
    x = agent_get_x();
    y = agent_get_y();
    
    addMessage_Location(id, x, y);
    /* printf("%d @ (%.2f, %.2f)\n", id, x, y); */
}

void af_calculateForces(void) {
    
    int id;
    struct m_Location *msg;
    double core_distance, edge_distance;
    double x, y, fx, fy, radius, x2, y2, k;
    
    
    /* load agent data */
    x = agent_get_x();
    y = agent_get_y();
    id = agent_get_id();
    radius = agent_get_radius();
    
    /* initialise forces */
    fx = 0.0;
    fy = 0.0;
    
    /* loop thru messages */
    /* randomiseMessages_Location(); *//* randomisation not necessary */
    for (msg = getMessage_Location(); msg; msg = getMessage_Location())
    {
        if (id == msg->id) continue; /* ignore own message */
        
        x2 = msg->x;
        y2 = msg->y;
        
        core_distance = sqrt(((x - x2)*(x - x2)) + ((y - y2)*(y - y2)));
        edge_distance = core_distance - (radius * 2);
        
        /* if edges overlap, repulsion occurs */
        if (edge_distance < 0.0) k = CONST_KR; 
        /* if edges close enough, attraction occurs */ 
        else if (edge_distance < radius) k = CONST_KA;
        else continue;
        
        /* update forces */
        fx += k * edge_distance * ((x2 - x)/core_distance);
        fy += k * edge_distance * ((y2 - y)/core_distance);

    }
    
    /* update agent data */
    agent_set_fx(fx);
    agent_set_fy(fy);
    
    /* post force data */
    addMessage_Force(id, fx, fy);
    
}

void af_updatePosition(void) {
    
    int id;
    double x, y, fx, fy;
    
    /* load agent data */
    x  = agent_get_x();
    y  = agent_get_y();
    fx = agent_get_fx();
    fy = agent_get_fy();
    id = agent_get_id();
    
    /* update position */
    x += fx;
    y += fy;
    
    /* update agent data */
    agent_set_x(x);
    agent_set_y(y);
    
}
