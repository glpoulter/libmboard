/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */
#include "header_mb_parallel.h"

/* declare global vars */
MPI_Comm testsuite_mpi_comm;
int testsuite_mpi_rank, testsuite_mpi_size;

/* Define tests within this suite */
CU_TestInfo mbp_test_array[] = {
    
    /* test_mb_p1.c */
    {"MB Environment initialisation                 ", test_mb_p_init                 },
    /* test_mb_p2.c */
    {"Creating and deleting message board           ", test_mb_p_create               },
    /* test_mb_p4.c */
    {"Adding message                                ", test_mb_p_addmessage           },
    {"Adding messages beyond single block           ", test_mb_p_addmessage_many      },
    /* test_mb_p3.c */
    {"Clearing message board                        ", test_mb_p_clear                },
    /* test_mb_p5.c */
    {"Creating Iterator                             ", test_mb_p_iter_create          },
    {"Reading messages from Iterator                ", test_mb_p_iter_getmsg          },
    {"Deleting Iterator                             ", test_mb_p_iter_delete          },
    /* test_mb_p6.c */
    {"Creating Sorted Iterator                      ", test_mb_p_iter_create_sorted   },
    {"Reading messages from Sorted Iterator         ", test_mb_p_iter_sorted_getmsg   },
    /* test_mb_p7.c */
    {"Creating Filtered Iterator                    ", test_mb_p_iter_create_filtered },
    {"Reading messages from Filtered Iterator       ", test_mb_p_iter_filtered_getmsg },
    /* test_mb_p10.c */
    {"Creating Filtered+Sorted Iterator             ", test_mb_p_iter_create_filteredsorted },
    {"Reading messages from Filtered+Sorted Iterator", test_mb_p_iter_filteredsorted_getmsg },
    /* test_mb_p8.c */
    {"Rewinding Iterator                            ", test_mb_p_iter_rewind          },
    /* test_mb_p9.c */
    {"Randomising Iterator                          ", test_mb_p_iter_randomise       },
    /* test_mb_p11.c */
    {"Registering and freeing Functions             ", test_mb_p_function_register    },
    {"Assigning Function to Board                   ", test_mb_p_function_assign      },
    /* test_mb_p1.c */
    {"MB Environment finalisation                   ", test_mb_p_finalise             },
    
    CU_TEST_INFO_NULL,
};
    
int init_mb_parallel(void) {
    int rc, flag;
    
    /* initialise MPI environment */
    rc = MPI_Initialized(&flag);
    if (rc != MPI_SUCCESS) return rc;
    if (!flag) 
    {
        rc = MPI_Init(NULL, NULL);
        if (rc != MPI_SUCCESS) return rc;
    }
    /* create our own communicator */
    MPI_Comm_dup(MPI_COMM_WORLD, &testsuite_mpi_comm);
    MPI_Comm_rank(testsuite_mpi_comm, &testsuite_mpi_rank);
    MPI_Comm_size(testsuite_mpi_comm, &testsuite_mpi_size);
    
    /* initialise MB environment */
    rc = MB_Env_Init();
    if (rc != MB_SUCCESS) return rc;
    
    return 0;
}

int clean_mb_parallel(void) {
    int rc;
    
    /* finalise MB environment */
    if (MB_Env_Finalised() != MB_SUCCESS)
    {
        rc = MB_Env_Finalise();
        if (rc != MB_SUCCESS) return rc;
    }
    
    /* finalise MPI environment */
    MPI_Comm_free(&testsuite_mpi_comm);
    MPI_Finalize();
    
    return 0;
}

CU_ErrorCode testsuite_mb_parallel(void) {
   
    /* Register test suite */
    CU_SuiteInfo suites[] = {
        {"MBoard Library (parallel)", init_mb_parallel, clean_mb_parallel, mbp_test_array},
        CU_SUITE_INFO_NULL,
    };
    
    return CU_register_suites(suites);
    
}
