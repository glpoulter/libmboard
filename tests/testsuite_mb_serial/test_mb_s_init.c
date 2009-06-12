/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_mb_serial.h"

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
