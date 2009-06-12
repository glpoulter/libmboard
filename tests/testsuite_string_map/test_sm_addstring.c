/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : June 2009
 * 
 */

#include "header_string_map.h"

void test_sm_addstring(void) {
    
    int rc;
    MBIt_stringmap *sm;
    
    /* add to a null map */
    sm = NULL;
    rc = MBI_stringmap_AddString(sm, "qwerty");
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* create a map */
    sm = MBI_stringmap_Create();
    CU_ASSERT_PTR_NOT_NULL(sm);
    if (sm != NULL) CU_ASSERT_PTR_NOT_NULL(sm->map);
    
    /* Add a strings */
    rc = MBI_stringmap_AddString(sm, "My first string!!");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* Add duplicate string */
    rc = MBI_stringmap_AddString(sm, "My first string!!");
    CU_ASSERT_EQUAL(rc, MB_ERR_DUPLICATE);
    
    /* All these should be different */
    rc = MBI_stringmap_AddString(sm, "My first string!! ");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_stringmap_AddString(sm, "My First String!!");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_stringmap_AddString(sm, "My first st");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_stringmap_AddString(sm, "st string!!");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    rc = MBI_stringmap_AddString(sm, ")(*&%$£!\"$%+_?><~@:}{#';][/.,    ");
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* add a looooooong one */
    rc = MBI_stringmap_AddString(sm, \
            "Lorem ipsum dolor sit amet, "  \
            "consectetur adipiscing elit. " \
            "Maecenas turpis urna, tempor eu viverra ut, "\
            "consequat sagittis felis." \
            );
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    
    /* add empty string */
    rc = MBI_stringmap_AddString(sm, "");
    CU_ASSERT_EQUAL(rc, MB_ERR_INVALID);
    
    /* delete the map */
    rc = MBI_stringmap_Delete(&sm);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(sm);
}
