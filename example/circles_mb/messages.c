/* $Id: messages.c 1871 2009-06-12 13:58:18Z lsc $ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : messages.c
 *  
 */

#include "circles.h"

int createBoards(void) {
    
    if (MB_Create(&b_Location, sizeof(struct m_Location)) != MB_SUCCESS) 
        return FAIL;
    if (MB_Create(&b_Force, sizeof(struct m_Force)) != MB_SUCCESS) 
        return FAIL;
    
    return OK;
}

int registerFunctions(void) {
    /* DEPRECATED
    if (MB_Function_Register(&f_range, &filterWithinRange) != MB_SUCCESS)
        return FAIL;
    */
    return OK;
}

int assignFunctionsToBoards(void) {
    /* DEPRECATED
    if (MB_Function_Assign(b_Location, f_range, &fp_range, sizeof(struct rangeFuncParams)) 
            != MB_SUCCESS)
        return FAIL;
    */
    return OK;
}

int deleteBoards(void) {
    
    MB_Delete(&b_Location);
    MB_Delete(&b_Force);
    
    return OK;
}

int addMessage_Location(int id, double x, double y) {
    
    struct m_Location msg;
    
    msg.id = id;
    msg.x  = x;
    msg.y  = y;
    
    return MB_AddMessage(b_Location, &msg);
}

int addMessage_Force(int id, double fx, double fy) {
    
    struct m_Force msg;
    
    msg.id = id;
    msg.fx = fx;
    msg.fy = fy;
    
    return MB_AddMessage(b_Force, &msg);
}

/* memory of previous message is deallocated when routine is called
 * users WILL NOT BE ABLE TO access two messages at once. If required,
 * first store the content of message before getting next message
 */
struct m_Location* getMessage_Location(void) {
    
    static struct m_Location *msg_prev = NULL;
    void *obj;
    struct m_Location *msg;
    
    /* deallocate previously returned message */
    if (msg_prev != NULL) free(msg_prev);
    else MB_Iterator_Rewind(i_Location); 
    
    
    /* get next message from iterator */
    MB_Iterator_GetMessage(i_Location, (void **)&obj);
    msg = (struct m_Location*)obj;
    
    /* store pointer so memory can be deallocated later */
    msg_prev = msg;
    
    return msg;
}

/* memory of previous message is deallocated when routine is called
 * users WILL NOT BE ABLE TO access two messages at once. If required,
 * first store the content of message before getting next message
 */
struct m_Force* getMessage_Force(void) {
    
    static struct m_Force *msg_prev = NULL;
    struct m_Force *msg;
    void *obj;

    /* deallocate previously returned message */
    if (msg_prev != NULL) free(msg_prev);
    /* rewind iterator if starting from beginning */
    else MB_Iterator_Rewind(i_Force); 
    
    /* get next message from iterator */
    MB_Iterator_GetMessage(i_Force, (void **)&obj);
    msg = (struct m_Force*)obj;
    
    /* store pointer so memory can be deallocated later */
    msg_prev = msg;
    
    return msg;
}

void randomiseMessages_Location(void) {
    MB_Iterator_Randomise(i_Location);
}

void randomiseMessages_Force(void) {
    MB_Iterator_Randomise(i_Force);
}
