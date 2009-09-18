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
    
    /* initialising MB environment */
    {"MB Environment initialisation                 ", test_mb_p_init                 },
    /* basic board operations */
    {"Creating and deleting message board           ", test_mb_p_create               },
    {"Setting board access modes                    ", test_mb_p_setaccessmode        },
    {"Setting board sync pattern                    ", test_mb_p_setsyncpattern       },
    {"Adding message                                ", test_mb_p_addmessage           },
    {"Adding messages beyond single block           ", test_mb_p_addmessage_many      },
    {"Clearing message board                        ", test_mb_p_clear                },
    /* Iterators */
    {"Creating Iterator                             ", test_mb_p_iter_create          },
    {"Reading messages from Iterator                ", test_mb_p_iter_getmsg          },
    {"Deleting Iterator                             ", test_mb_p_iter_delete          },
    {"Creating Sorted Iterator                      ", test_mb_p_iter_create_sorted   },
    {"Reading messages from Sorted Iterator         ", test_mb_p_iter_sorted_getmsg   },
    {"Creating Filtered Iterator                    ", test_mb_p_iter_create_filtered },
    {"Reading messages from Filtered Iterator       ", test_mb_p_iter_filtered_getmsg },
    {"Creating Filtered+Sorted Iterator             ", test_mb_p_iter_create_filteredsorted },
    {"Reading messages from Filtered+Sorted Iterator", test_mb_p_iter_filteredsorted_getmsg },
    {"Rewinding Iterator                            ", test_mb_p_iter_rewind          },
    {"Randomising Iterator                          ", test_mb_p_iter_randomise       },
    /* testing indexmap */
    {"Creating Index Maps                           ", test_mb_p_indexmap_create   },
    {"Deleting Index Maps                           ", test_mb_p_indexmap_delete   },
    {"Adding entries to Index Map                   ", test_mb_p_indexmap_addentry },
    {"Adding entries to Index Map with duplicates   ", test_mb_p_indexmap_addentry_withdups },
    {"Adding random entries to Index Map (with dups)", test_mb_p_indexmap_addentry_randomvals },
    {"Querying an Index Map                         ", test_mb_p_indexmap_memberof },
    {"Querying an Index Map with duplicates         ", test_mb_p_indexmap_memberof_withdups },
    {"Querying an Index Map with large spread + dups", test_mb_p_indexmap_memberof_randomvals },
    {"Querying an Index Map with no par' overlaps   ", test_mb_p_indexmap_memberof_nooverlaps },
    {"Querying an Index Map with some par' overlaps ", test_mb_p_indexmap_memberof_someoverlaps },
    {"Synchronising Index Maps                      ", test_mb_p_indexmap_sync     },
    /* filters */
    {"Creating filter objects                       ", test_mb_p_filter_create     },
    {"Assigning filters to boards                   ", test_mb_p_filter_assign     },
    {"Deleting filter objects                       ", test_mb_p_filter_delete     },
    /* synchronising distributed boards */
    {"Sync routines (basic tests)                   ", test_mb_p_sync_basic        },
    {"Checking board content after sync             ", test_mb_p_sync_checkcontent },
    {"Checking board content after sync (filter)    ", test_mb_p_sync_withfilter   },
    {"Checking fallback to full data replication    ", test_mb_p_sync_withfilter_fdr  },
    {"Checking with filters that use Index Map      ", test_mb_p_sync_indexmap  },
    {"Checking syncs of board with access modes     ", test_mb_p_sync_accessmode },
    {"Checking syncs of board with sync patterns    ", test_mb_p_sync_pattern },
    {"Board resyncs                                 ", test_mb_p_sync_resync  },
    {"Board resyncs (with filters)                  ", test_mb_p_sync_resync_filtered },
    /* finalising MB environment */
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
    
    /* message to users */
    if (testsuite_mpi_size < 4 && testsuite_mpi_rank == 0)
    {
        printf("\n\n");
        printf("=====================================================================\n");
        printf("\n");
        printf("  NOTICE: You should repeat this test with at least 4 MPI tasks\n");
        printf("          for a more thorough test of the communication routines\n");
        printf("\n");
        printf("=====================================================================\n");
        printf("\n");
    }
    
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
