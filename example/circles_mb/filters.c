/* $Id$ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : filters.c
 *  
 */

#include "circles.h"
#include <math.h>
#include <float.h>

int filterWithinRange(const void *msg, const void *params) {
    
    struct m_Location *m;
    struct filterWithinRangeParams *p;
    double distance_squared;
    
    /* cast input pointers to proper type */
    m = (struct m_Location *)msg;
    p = (struct filterWithinRangeParams *)params;
    
    /* calculate distance */
    distance_squared = (m->x - p->x)*(m->x - p->x) + (m->y - p->y)*(m->y - p->y);
    
    /* if within range, accept msg */
    if (distance_squared <= (p->range * p->range)) return 1;
    
    return 0;
}

void generate_filterfunc_params(void) {
    
    double xmin, xmax, ymin, ymax;
    double x, y, radius;
    
    /* init values */
    xmin = DBL_MAX;
    ymin = DBL_MAX;
    xmax = -DBL_MAX;
    ymax = -DBL_MAX;
    
    LOOP_THRU_AGENTS {
        x = agent_get_x();
        y = agent_get_y();
        radius = 2 * agent_get_radius();
        
        if (xmin > x - radius) xmin = x - radius;
        if (ymin > y - radius) ymin = y - radius;
        if (xmax < x + radius) xmax = x + radius;
        if (ymax < y + radius) ymax = y + radius;
    }
    
    /* set fparam values */
    fp_range.xmin = xmin;
    fp_range.ymin = ymin;
    fp_range.xmax = xmax;
    fp_range.ymax = ymax;

}
