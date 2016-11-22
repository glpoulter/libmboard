/* $Id: testsuite_om_utils.c 2927 2012-07-20 15:33:14Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_objmap.h"

/* create and text OM object */
MBIt_objmap* create_om_obj(void) {
    MBIt_objmap *mymap = NULL;
    
    mymap = MBI_objmap_new();
    CU_ASSERT_PTR_NOT_NULL_FATAL(mymap);
    return mymap;
}

/* delete OM object */
void delete_om_obj(MBIt_objmap **mymap) {
    MBI_objmap_destroy(mymap);
    CU_ASSERT_PTR_NULL(*mymap);
}
