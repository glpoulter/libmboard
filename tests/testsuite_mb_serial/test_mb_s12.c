/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : April 2008
 * 
 */
#include "header_mb_serial.h"

/* testing MB_Function_Register() */
void test_mb_s_function_register(void) {
    
    int rc;
    MBt_Function fh;
    
    /* this routine does nothing */
    rc = MB_Function_Register(&fh, NULL);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
}


/* testing MB_Function_Assign() */
void test_mb_s_function_assign(void) {
    
    int rc;
    int dummy_param;
    MBt_Board mb;
    MBt_Function fh;
    MBIt_Board *board;
    
    /* does not matter what fh we use */
    fh = MB_NULL_FUNCTION;
    dummy_param = 1;
    
    /* try assigning fh to null board */
    mb = MB_NULL_MBOARD;
    rc = MB_Function_Assign(mb, fh, NULL, 0);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* try assigning fh to non-existant board */
    mb = 9999999;
    rc = MB_Function_Assign(mb, fh, NULL, 0);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* create a board */
    rc = MB_Create(&mb, sizeof(int));
    if (rc != MB_SUCCESS) CU_FAIL("Unable to create test board");
    
    /* try assigning fh with invalid param size */
    rc = MB_Function_Assign(mb, fh, &dummy_param, 0);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* a proper call */
    rc = MB_Function_Assign(mb, fh, &dummy_param, sizeof(int));
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* if param = NULL, param_size can be 0 */
    rc = MB_Function_Assign(mb, fh, NULL, 0);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* get reference to board object */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    
    /* try assigning to locked board */
    board->locked = MB_TRUE;
    rc = MB_Function_Assign(mb, fh, NULL, 0);
    CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
    
    /* delete board */
    board->locked = MB_FALSE; 
    rc = MB_Delete(&mb);
    if (rc != MB_SUCCESS) CU_FAIL("Unable to delete test board");
    
}


/* testing MB_Function_Free() */
void test_mb_s_function_free(void) {
    
    int rc;
    MBt_Function fh;
    
    /* this routine does not do much */
    rc = MB_Function_Free(&fh);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(fh, MB_NULL_FUNCTION);
}
