/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_mb_serial.h"
#include "objmap.h"

/* test init */
void test_mb_s_init(void) {
    
    int rc;
    
    /* Flags should be properly set */
    CU_ASSERT_EQUAL(MBI_STATUS_initialised, MB_TRUE);
    CU_ASSERT_EQUAL(MBI_STATUS_finalised,   MB_FALSE);
    
    /* object maps properly defined */
    CU_ASSERT_PTR_NOT_NULL_FATAL(MBI_OM_mboard);
    CU_ASSERT_EQUAL(MBI_OM_mboard->type, OM_TYPE_MBOARD);
    
    CU_ASSERT_PTR_NOT_NULL_FATAL(MBI_OM_iterator);
    CU_ASSERT_EQUAL(MBI_OM_iterator->type, OM_TYPE_ITERATOR);
    
    rc = MB_Env_Initialised();
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MB_Env_Finalised();
    CU_ASSERT_EQUAL(rc, MB_ERR_ENV);
}

/* test message board creation and deletion */
void test_mb_s_create_delete(void) {
    
   int rc;
   MBt_Board mb  = (MBt_Board)-9; /* set dummy value to appease valgrind */
   MBt_Board mb2 = (MBt_Board)-9; /* set dummy value to appease valgrind */
   MBIt_Board *board;
   void *obj;
   
   /* create message with wrong params */
   rc = MB_Create(&mb, 0);
   CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
   CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
   
   rc = MB_Create(&mb, -1234);
   CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
   CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
   
   /* Creating a message board */
   rc = MB_Create(&mb, sizeof(dummy_msg));
   CU_ASSERT_EQUAL(rc, MB_SUCCESS);
   CU_ASSERT_NOT_EQUAL(mb, MB_NULL_MBOARD);
   
   /* make sure returned mb is registered in object map */
   obj = MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb);
   CU_ASSERT_PTR_NOT_NULL(obj);
   if (obj)
   {
       board = (MBIt_Board *)obj;
       CU_ASSERT_EQUAL(board->locked, MB_FALSE);
       
       /* make sure pooled list created */
       CU_ASSERT_PTR_NOT_NULL(board->data);
       CU_ASSERT_EQUAL((int)board->data->elem_size, (int)sizeof(dummy_msg));
   }
   else
   {
       return; /* Do not proceed with this test */
   }
   
   /* Deleting an invalid board */
   rc = MB_Delete(&mb2);
   CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
   
   /* Deleting a null board */
   mb2 = MB_NULL_MBOARD;
   rc = MB_Delete(&mb2);
   CU_ASSERT_EQUAL(rc, MB_SUCCESS);
   
   /* Deleting a locked board */
   board = (MBIt_Board *)obj;
   board->locked = MB_TRUE;
   rc = MB_Delete(&mb);
   CU_ASSERT_EQUAL(rc, MB_ERR_LOCKED);
   /* make sure returned mb is registered in object map */
   obj = MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb);
   CU_ASSERT_PTR_NOT_NULL(obj);
   if (!obj) return; /* no not proceed */
   CU_ASSERT_PTR_NOT_NULL(board->data);
   
   /* Deleting a board */
   board = (MBIt_Board *)obj;
   board->locked = MB_FALSE; /* reset value */
   rc = MB_Delete(&mb);
   obj = MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mb);
   CU_ASSERT_EQUAL(rc, MB_SUCCESS);
   CU_ASSERT_PTR_NULL(obj);
   CU_ASSERT_EQUAL(mb, MB_NULL_MBOARD);
   
}

/* test creation of multiple boards */
void test_mb_s_create_multiple(void) {
    
    int rc, i;
    int errfound;
    int count = 100;
    MBt_Board *mbs;
    void *obj;
    
    /* allocate memory to store boards */
    mbs = (MBt_Board *)malloc(sizeof(MBt_Board) * count);
    
    /* Create a bunch of boards */
    errfound = 0;
    for (i = 0; i < count; i++)
    {
        mbs[i] = -1; /* set dummy value to appease valgrind */
        rc = MB_Create(&mbs[i], sizeof(dummy_msg));
        if (rc != MB_SUCCESS) errfound++;
    }
    CU_ASSERT_EQUAL(errfound, 0);
    
    /* Check them */
    errfound = 0;
    for (i = 0; i < count; i++)
    {
        obj = MBI_objmap_getobj(MBI_OM_mboard, (OM_key_t)mbs[i]);
        if (!obj) errfound++;
    }
    CU_ASSERT_EQUAL(errfound, 0);
    
    /* Delete them */
    errfound = 0;
    for (i = 0; i < count; i++)
    {
        rc = MB_Delete(&mbs[i]);
        if (rc != MB_SUCCESS) errfound++;
        if (mbs[i] != MB_NULL_MBOARD) errfound++;
    }
    CU_ASSERT_EQUAL(errfound, 0);
    
    free(mbs);
    
}
