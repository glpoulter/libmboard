/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_string_map.h"

void test_sm_contains(void) {
    int i, rc;
    MBIt_stringmap *sm;
    const char *str[10] = { "value 1", 
                            "value 2", 
                            "value 3", 
                            "value 4", 
                            "value 5", 
                            "The quick brown fox jumps over the lazy dog!", 
                            "The quick brown fox jumps over the lazy dog!!", 
                            " ",          /* a single space */
                            "   ",        /* three spaces */
                            "(*^$£$&" 
                          };
    
    /* query from a null map */
    sm = NULL;
    rc = MBI_stringmap_Contains(sm, "qwerty");
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* create a map */
    sm = MBI_stringmap_Create();
    CU_ASSERT_PTR_NOT_NULL(sm);
    if (sm != NULL) CU_ASSERT_PTR_NOT_NULL(sm->map);
    
    /* Add a strings */
    for (i = 0; i < 10; i++)
    {
        rc = MBI_stringmap_AddString(sm, str[i]);
        if (rc != MB_SUCCESS) 
        {
            CU_FAIL("Failed to add string");
            break;
        }
    }
    
    /* query null string */
    rc = MBI_stringmap_Contains(sm, NULL);
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* query unknown strings */
    rc = MBI_stringmap_Contains(sm, "value 8");
    CU_ASSERT_EQUAL(rc, MB_FALSE);
    rc = MBI_stringmap_Contains(sm, "    "); /* four spaces */
    CU_ASSERT_EQUAL(rc, MB_FALSE);
    rc = MBI_stringmap_Contains(sm, "The quick brown fox jumps over the lazy dog!!!");
    CU_ASSERT_EQUAL(rc, MB_FALSE);
    rc = MBI_stringmap_Contains(sm, "The quick brown fox jumps over the lazy dog");
    CU_ASSERT_EQUAL(rc, MB_FALSE);
    
    /* query known strings */
    for (i = 0; i < 10; i++)
    {
        rc = MBI_stringmap_Contains(sm, str[i]);
        if (rc != MB_TRUE)
        {
            CU_FAIL("Failed to retrieve valid string");
            break;
        }
    }
    
    /* delete the map */
    rc = MBI_stringmap_Delete(&sm);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(sm);
}
