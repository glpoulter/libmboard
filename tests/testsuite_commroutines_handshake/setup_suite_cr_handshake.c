/* $Id: setup_suite_cr_handshake.c 2929 2012-07-20 16:37:09Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Sept 2009
 * 
 */
#include "header_commroutines_handshake.h"

/* Define tests within this suite */
CU_TestInfo cr_handshake_test_array[] = { 

    {"AgreeBufSizes (RW)                    ", test_cr_handshake_agreebufsizes_rw },
    {"AgreeBufSizes (RW. Empty)             ", test_cr_handshake_agreebufsizes_rw_empty },
    {"AgreeBufSizes (RW. F+map)             ", test_cr_handshake_agreebufsizes_rw_fmap },
    {"AgreeBufSizes (RW. F+FDR)             ", test_cr_handshake_agreebufsizes_rw_fdr },
    {"AgreeBufSizes (RW+IDLE)               ", test_cr_handshake_agreebufsizes_rw_id },
    {"AgreeBufSizes (RO+WO)                 ", test_cr_handshake_agreebufsizes_ro_wo },
    {"PropagateMessages (RW)                ", test_cr_handshake_propagatemessages_rw },
    {"PropagateMessages (RW. Empty)         ", test_cr_handshake_propagatemessages_rw_empty },
    {"PropagateMessages (RW. F+map)         ", test_cr_handshake_propagatemessages_rw_fmap },
    {"PropagateMessages (RW. F+FDR)         ", test_cr_handshake_propagatemessages_rw_fdr },
    {"PropagateMessages (RW+IDLE)           ", test_cr_handshake_propagatemessages_rw_id },
    {"PropagateMessages (RO+WO)             ", test_cr_handshake_propagatemessages_ro_wo },
    {"LoadAndFreeBuffers (RW)               ", test_cr_handshake_loadandfreebuffers_rw },
    {"LoadAndFreeBuffers (RW. Empty)        ", test_cr_handshake_loadandfreebuffers_rw_empty },
    {"LoadAndFreeBuffers (RW. F+map)        ", test_cr_handshake_loadandfreebuffers_rw_fmap },
    {"LoadAndFreeBuffers (RW. F+FDR)        ", test_cr_handshake_loadandfreebuffers_rw_fdr },
    {"LoadAndFreeBuffers (RW+IDLE)          ", test_cr_handshake_loadandfreebuffers_rw_id },
    {"LoadAndFreeBuffers (RO+WO)            ", test_cr_handshake_loadandfreebuffers_ro_wo },
    
    CU_TEST_INFO_NULL,
};
    
CU_ErrorCode testsuite_commroutines_handshake(void) {
    
    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"Communication Routines (HANDSHAKE)", 
                init_cr_handshake, clean_cr_handshake, cr_handshake_test_array},
        CU_SUITE_INFO_NULL,
    };
    
    return CU_register_suites(suites);
    
}
