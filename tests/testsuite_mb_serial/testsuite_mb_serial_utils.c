/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_mb_serial.h"


/* map message sequence ID to value */
int get_message_value_from_id(int id) {
    /* return a value for message such that it is predictable,
     * yet capable for exercising the sorting/filtering
     * functionalities of Iterators
     */
    
    int index, ub;
    
    if ((id % 2) == 0) /* even nums */
    {
        index = (int)(id / 2);
        ub = SERIAL_TEST_MSG_COUNT;
    }
    else /* odd nums */
    {
        index = (int)((id - 1) / 2);
        ub = (int)(SERIAL_TEST_MSG_COUNT / 2);
    }
    
    return ub - index;
    
    
}
/* initialise message board with content */
int init_mb_with_content(MBt_Board *mb_ptr) {
    
    int i, rc;
    dummy_msg msg;
    
    /* Create Message Board */
    rc = MB_Create(mb_ptr, sizeof(dummy_msg));
    if (rc != MB_SUCCESS) return rc; /* abort on failure */
    
    /* populate board with messages */
    for (i = 0; i < SERIAL_TEST_MSG_COUNT; i++)
    {
        msg.ernet = get_message_value_from_id(i);
        msg.jeopardy = (double)get_message_value_from_id(i);
        rc = MB_AddMessage(*mb_ptr, (void *)&msg);
        
        /* stop on failure */
        if (rc != MB_SUCCESS) break;
    }
    
    return rc;
    
}
