/* $Id: test_manymsgs.c 732 2008-03-10 15:55:53Z lsc $ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : test_manyboards.c
 *  
 * Desc : Adding many messages
 */

#include "stresstest.h"

static int mycmp(const void *m1, const void* m2)
{
    int *a1, *a2;
    a1 = (int*)m1;
    a2 = (int*)m2;
    
    if (*a1 < *a2) return -1;
    else if (*a1 > *a2) return 1;
    else return 0;
    
}

int t_manymsgs(void) {
    
    int i, rc, e, e2, msg;
    int found, expected;
    int *mptr;
    unsigned int count;
    MBt_Board mb;
    MBt_Iterator itr;
    
    count = TP_MANYMSG_COUNT / env_size;
    PRINT("--- test: manymsgs ---\n");
    
    
    PRINT("* Creating board ... ");
    rc = MB_Create(&mb, sizeof(int));
    if (rc == MB_SUCCESS)
    {
        PRINT("PASS\n");
    }
    else
    {
        PRINT("FAILED to create board\n");
        return FAIL;
    }
    
    /* add messages */
    PRINT("* Adding %d messages ... ", count);
    e = 0;
    for (i = 0; i < count; i++)
    {
        msg = (count * env_rank) + i;
        rc = MB_AddMessage(mb, &msg);
        
        if (rc != MB_SUCCESS) e++;
    }
    if (0 == e) PRINT("PASS\n");
    else PRINT("FAILED %d of %d\n", e, count);
    
    /* sync */
    PRINT ("* Start synchronisation of board ... ");
    rc = MB_SyncStart(mb);
    if (rc == MB_SUCCESS) PRINT("PASS\n");
    else PRINT("FAIL\n");
    
    PRINT ("* Completing synchronisation of board ... ");
    rc = MB_SyncComplete(mb);
    if (rc == MB_SUCCESS) PRINT("PASS\n");
    else PRINT("FAIL\n");
    
    /* traverse */
    e2 = 0;
    found = 0;
    expected = count * env_size;
    PRINT("* Iterating through messages ... ");
    rc = MB_Iterator_CreateSorted(mb, &itr, &mycmp);
    if (rc != MB_SUCCESS) PRINT("FAIL to create iterator\n");
    else 
    {
        MB_Iterator_GetMessage(itr, (void *)&mptr);
        while (mptr != NULL)
        {
            if (*mptr != found) e2++;
            
            found ++;
            MB_Iterator_GetMessage(itr, (void *)&mptr);
        }
        
        if (found != expected)
        {
            PRINT("FAIL (did not get all expected messages)\n");
        }
        else if (0 != e2)
        {
            PRINT("FAIL (message data corrupted)\n");
        }
        else 
        {
            PRINT("PASS\n");
        }
    }
    
    /* delete boards */
    PRINT("* Deleting boards along with messages ... ");
    rc = MB_Delete(&mb);
    if (rc == MB_SUCCESS)
    {
        PRINT("PASS\n");
    }
    else
    {
        PRINT("FAILED to delete board");
        return FAIL;
    }
    
    if (0 == (e)) return OK;
    else return FAIL;
}
