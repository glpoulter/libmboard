/* $Id$ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : test_manyboards.c
 *  
 * Desc : Adding many boards
 */

#include "stresstest.h"

int t_manyboards(void) {
    
    int i, rc, e, e2;
    MBt_Board *mb;
    
    PRINT("--- test: manyboards ---\n");
    
    /* create board array */
    mb = (MBt_Board *)malloc(sizeof(MBt_Board) * TP_MANYBOARD_COUNT);
    
    /* add boards */
    PRINT("* Adding %d boards ... ", TP_MANYBOARD_COUNT);
    e = 0;
    for (i = 0; i < TP_MANYBOARD_COUNT; i++)
    {
        rc = MB_Create(&mb[i], (size_t)(i + 1));
        
        if (rc != MB_SUCCESS) e++;
    }
    if (0 == e) PRINT("PASS\n");
    else PRINT("FAILED %d of %d\n", e, TP_MANYBOARD_COUNT);
    
    
    /* delete boards */
    PRINT("* Deleting boards ... ");
    e2 = 0;
    for (i = 0; i < TP_MANYBOARD_COUNT; i++)
    {
        rc = MB_Delete(&mb[i]);
        
        if (rc != MB_SUCCESS) e2++;
    }
    if (0 == e2) PRINT("PASS\n");
    else PRINT("FAILED %d of %d\n", e2, TP_MANYBOARD_COUNT);
    
    /* clean up and quit */
    free(mb);
    
    if (0 == (e + e2)) return OK;
    else return FAIL;
}
