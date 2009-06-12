/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_string_map.h"

void test_sm_removestring(void) {
    
    int rc;
    MBIt_stringmap *sm;
    
    /* remove from a null map */
    sm = NULL;
    rc = MBI_stringmap_RemoveString(sm, "qwerty");
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* create a map */
    sm = MBI_stringmap_Create();
    CU_ASSERT_PTR_NOT_NULL(sm);
    if (sm != NULL) CU_ASSERT_PTR_NOT_NULL(sm->map);
    
    /* Add some strings */
    rc = MBI_stringmap_AddString(sm, ">> String 1 <<");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_stringmap_AddString(sm, ">> String 2 <<");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_stringmap_AddString(sm, ">> String 3 <<");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_stringmap_AddString(sm, ">> String 4 <<");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_stringmap_AddString(sm, ">> String 5 <<");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Remove string */
    rc = MBI_stringmap_RemoveString(sm, ">> String 3 <<");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Remove unknown strings */
    rc = MBI_stringmap_RemoveString(sm, ">> String 7 <<");
    CU_ASSERT_EQUAL(rc, MB_ERR_NOT_FOUND);
    rc = MBI_stringmap_RemoveString(sm, "String 5 <<");
    CU_ASSERT_EQUAL(rc, MB_ERR_NOT_FOUND);
    rc = MBI_stringmap_RemoveString(sm, ">> String 5");
    CU_ASSERT_EQUAL(rc, MB_ERR_NOT_FOUND);
    rc = MBI_stringmap_RemoveString(sm, ">> String 5 << ");
    CU_ASSERT_EQUAL(rc, MB_ERR_NOT_FOUND);
    
    /* add another */
    rc = MBI_stringmap_AddString(sm, ">> String 6 <<");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* remove a few more */
    rc = MBI_stringmap_RemoveString(sm, ">> String 1 <<");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_stringmap_RemoveString(sm, ">> String 2 <<");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_stringmap_RemoveString(sm, ">> String 6 <<");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    
    /* delete the map */
    /* there should one more string left in the map, but Delete()
     * should still not leak memory. Run tests via valgrind to be sure
     */
    rc = MBI_stringmap_Delete(&sm);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(sm);
}
