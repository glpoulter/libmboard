/* $Id: test_cr_handshake_utils.c 2113 2009-09-15 14:42:01Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Sept 2009
 * 
 */
#include "header_commroutines_handshake.h"

#define void2int(m) (*((int*)m))

int fl_func_map_hs(const void *m, int pid) {    
    if (MB_IndexMap_MemberOf(imap_hs, pid, void2int(m))) return 1;
    else return 0;
}

int fl_func_fdr_hs(const void *m, int pid) {
    
    /* acknowledge+hide "unused parameter" compiler warnings */
    ACKNOWLEDGED_UNUSED(pid);
    
    if (void2int(m) % 2 == 0) return 1; /* return every other message */
    else return 0;
}

/* add values to map where values = num range +- {0-SMALLNUM} (cyclic) */
int _initialise_map_values_hs(MBt_IndexMap map) {
    
    int i, rc;
    int lbound, ubound;
    
    if (MBI_CommRank == 0) lbound = MBI_CommSize * TEST_MESSAGE_COUNT - 1;
    else lbound = MBI_CommRank * TEST_MESSAGE_COUNT - 1;
    
    if (MBI_CommRank == (MBI_CommSize - 1)) ubound = 0;
    else ubound = (MBI_CommRank + 1) * TEST_MESSAGE_COUNT;
    
    for (i = 0; i < SMALLNUM; i++)
    {
        rc = MB_IndexMap_AddEntry(map, ubound + i);
        if (rc != MB_SUCCESS) return rc;
        rc = MB_IndexMap_AddEntry(map, lbound - i);
        if (rc != MB_SUCCESS) return rc;
    }
    
    /* sync map and return */
    return MB_IndexMap_Sync(map);
}
