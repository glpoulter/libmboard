/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_objmap.h"

/* create and text OM object */
MBIt_objmap* create_om_obj() {
    MBIt_objmap *mymap = NULL;
    
    mymap = MBI_objmap_new();
    
    CU_ASSERT_PTR_NOT_NULL_FATAL(mymap);
    /* CU_ASSERT_PTR_NULL_FATAL(mymap->map); */
    CU_ASSERT_EQUAL(mymap->top, 0);
    
    return mymap;
}

/* delete OM object */
void delete_om_obj(MBIt_objmap **mymap) {
    MBI_objmap_destroy(mymap);
    CU_ASSERT_PTR_NULL(*mymap);
}
