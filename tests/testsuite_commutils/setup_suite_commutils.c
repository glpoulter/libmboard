/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Aug 2009
 * 
 */
#include "header_commutils.h"

int TCU_CommRank, TCU_CommSize;

/* Define tests within this suite */
CU_TestInfo cu_test_array[] = {
   
    {"MBI_CommUtil_TagMessages              ", test_cu_tagmessages             },
    {"MBI_CommUtil_UpdateCursor             ", test_cu_updatecursor            },
    {"MBI_CommUtil_BuildBuffer_All          ", test_cu_buildbuffer_all         },
    {"MBI_CommUtil_BuildBuffers_Tagged      ", test_cu_buildbuffers_tagged     },
    {"MBI_CommUtil_LoadBuffer               ", test_cu_loadbuffer              },
    
    CU_TEST_INFO_NULL,
};
    
CU_ErrorCode testsuite_commutils(void) {
    
    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"Communication Utilities", init_cu, clean_cu, cu_test_array},
        CU_SUITE_INFO_NULL,
    };
    
    return CU_register_suites(suites);
    
}

int init_cu(void) {

    MPI_Comm_rank(MPI_COMM_WORLD, &TCU_CommRank);
    MPI_Comm_size(MPI_COMM_WORLD, &TCU_CommSize);
    return 0;
}
int clean_cu(void) {return 0;}
