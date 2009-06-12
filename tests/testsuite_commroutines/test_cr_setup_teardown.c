/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2008
 * 
 */
#include "header_commroutines.h"

/* instantiate global vars */
MBt_Board mb_nofilter, mb_filter, mb_filter_fdr;
MBt_Board mb_empty, mb_empty_filter;
MBt_Filter filter_map, filter_fdr;
MBt_IndexMap indexmap;

struct MBIt_commqueue *node_nofilter;
struct MBIt_commqueue *node_filter;
struct MBIt_commqueue *node_filter_fdr;
struct MBIt_commqueue *node_empty;
struct MBIt_commqueue *node_empty_filter;


/* initialise test environment */
int init_cr(void) {
    
    int rc, flag, i, v;
    struct MBIt_commqueue *node;
    MBIt_Board *board;
    
    /* check that MPI environment is initialised */
    rc = MPI_Initialized(&flag);
    if (rc != MPI_SUCCESS) return rc;
    if (!flag) 
    {
        rc = MPI_Init(NULL, NULL);
        if (rc != MPI_SUCCESS) return rc;
    }
    
    /* We don't want to call MB_Env_Init as we do not want to start
     * the communication thread.
     */
    /* --- Set up MB environment manually! */
    MBI_STATUS_initialised = MB_TRUE;
    
    /* create our own communicator */
    MPI_Comm_dup(MPI_COMM_WORLD, &MBI_CommWorld);
    MPI_Comm_rank(MBI_CommWorld, &MBI_CommRank);
    MPI_Comm_size(MBI_CommWorld, &MBI_CommSize);
    
    /* Allocate object map */
    MBI_OM_mboard   = (MBIt_objmap*)MBI_objmap_new();
    MBI_OM_iterator = (MBIt_objmap*)MBI_objmap_new();
    MBI_OM_filter   = (MBIt_objmap*)MBI_objmap_new();
    MBI_OM_indexmap = (MBIt_objmap*)MBI_objmap_new();
    MBI_OM_mboard   ->type = OM_TYPE_MBOARD;
    MBI_OM_iterator ->type = OM_TYPE_ITERATOR;
    MBI_OM_filter   ->type = OM_TYPE_FILTER;
    MBI_OM_indexmap ->type = OM_TYPE_INDEXMAP;

    
    /* Allocate string map */
    MBI_indexmap_nametable = MBI_stringmap_Create();
    
    /* Initialise commqueue */
    rc = MBI_CommQueue_Init();
    if (rc != MB_SUCCESS) return rc;
    
    /* --- Create and initialise index map */
    rc = MB_IndexMap_Create(&indexmap, "My Map");
    if (rc != MB_SUCCESS) return rc;
    rc = _initialise_map_values(indexmap);
    if (rc != MB_SUCCESS) return rc;
    
    /* --- Create boards */
    
    /* board with no filter */
    rc = MB_Create(&mb_nofilter, sizeof(int));
    if (rc != MB_SUCCESS) return rc;
    
    /* board with filter that uses indexmap */
    rc = MB_Create(&mb_filter, sizeof(int));
    if (rc != MB_SUCCESS) return rc;
    rc = MB_Filter_Create(&filter_map, &filter_func_map);
    if (rc != MB_SUCCESS) return rc;
    rc = MB_Filter_Assign(mb_filter, filter_map);
    if (rc != MB_SUCCESS) return rc;
    
    /* board with filter that leads to fallback to full data replication */
    rc = MB_Create(&mb_filter_fdr, sizeof(int));
    if (rc != MB_SUCCESS) return rc;
    rc = MB_Filter_Create(&filter_fdr, &filter_func_fdr);
    if (rc != MB_SUCCESS) return rc;
    rc = MB_Filter_Assign(mb_filter_fdr, filter_fdr);
    if (rc != MB_SUCCESS) return rc;
    
    /* empty board with no filter */
    rc = MB_Create(&mb_empty, sizeof(int));
    if (rc != MB_SUCCESS) return rc;
    
    /* empty board with filter */
    rc = MB_Create(&mb_empty_filter, sizeof(int));
    if (rc != MB_SUCCESS) return rc;
    rc = MB_Filter_Assign(mb_empty_filter, filter_map);
    if (rc != MB_SUCCESS) return rc;
    
    /* --- add messages to boards */
    for (i = 0; i < TEST_MESSAGE_COUNT; i++)
    {
        v = (MBI_CommRank * TEST_MESSAGE_COUNT) + i;
        rc = MB_AddMessage(mb_filter, (void *)&v);
        if (rc != MB_SUCCESS) return rc;
        rc = MB_AddMessage(mb_nofilter, (void *)&v);
        if (rc != MB_SUCCESS) return rc;
        rc = MB_AddMessage(mb_filter_fdr, (void *)&v);
        if (rc != MB_SUCCESS) return rc;
    }
    
    /* --- add boards to comm queue */
    rc = MBI_CommQueue_Push(mb_nofilter, PRE_TAGGING);
    if (rc != MB_SUCCESS) return rc;
    rc = MBI_CommQueue_Push(mb_filter, PRE_TAGGING);
    if (rc != MB_SUCCESS) return rc;
    rc = MBI_CommQueue_Push(mb_filter_fdr, PRE_TAGGING);
    if (rc != MB_SUCCESS) return rc;
    rc = MBI_CommQueue_Push(mb_empty, PRE_TAGGING);
    if (rc != MB_SUCCESS) return rc;
    rc = MBI_CommQueue_Push(mb_empty_filter, PRE_TAGGING);
    if (rc != MB_SUCCESS) return rc;
    
    /* lock boards */
    board = (MBIt_Board*)MBI_getMBoardRef(mb_nofilter);
    if (board == NULL) return 999;
    board->locked = MB_TRUE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_filter);
    if (board == NULL) return 999;
    board->locked = MB_TRUE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_filter_fdr);
    if (board == NULL) return 999;
    board->locked = MB_TRUE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_empty);
    if (board == NULL) return 999;
    board->locked = MB_TRUE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_empty_filter);
    if (board == NULL) return 999;
    board->locked = MB_TRUE;
    
    /* --- get reference to internal node structure */
    node_nofilter = node_filter = node_filter_fdr = NULL;
    node_empty = node_empty_filter = NULL;
    
    node = MBI_CommQueue_GetFirstNode();
    if (node == NULL) return 1000;
    else if (node->mb == mb_nofilter)   node_nofilter   = node;
    else if (node->mb == mb_filter)     node_filter     = node;
    else if (node->mb == mb_filter_fdr) node_filter_fdr = node;
    else if (node->mb == mb_empty)      node_empty      = node;
    else if (node->mb == mb_empty_filter) node_empty_filter = node;
    
    node = node->next;
    if (node == NULL) return 1000;
    else if (node->mb == mb_nofilter)   node_nofilter   = node;
    else if (node->mb == mb_filter)     node_filter     = node;
    else if (node->mb == mb_filter_fdr) node_filter_fdr = node;
    else if (node->mb == mb_empty)      node_empty      = node;
    else if (node->mb == mb_empty_filter) node_empty_filter = node;
    
    node = node->next;
    if (node == NULL) return 1000;
    else if (node->mb == mb_nofilter)   node_nofilter   = node;
    else if (node->mb == mb_filter)     node_filter     = node;
    else if (node->mb == mb_filter_fdr) node_filter_fdr = node;
    else if (node->mb == mb_empty)      node_empty      = node;
    else if (node->mb == mb_empty_filter) node_empty_filter = node;
    
    node = node->next;
    if (node == NULL) return 1000;
    else if (node->mb == mb_nofilter)   node_nofilter   = node;
    else if (node->mb == mb_filter)     node_filter     = node;
    else if (node->mb == mb_filter_fdr) node_filter_fdr = node;
    else if (node->mb == mb_empty)      node_empty      = node;
    else if (node->mb == mb_empty_filter) node_empty_filter = node;
    
    node = node->next;
    if (node == NULL) return 1000;
    else if (node->mb == mb_nofilter)   node_nofilter   = node;
    else if (node->mb == mb_filter)     node_filter     = node;
    else if (node->mb == mb_filter_fdr) node_filter_fdr = node;
    else if (node->mb == mb_empty)      node_empty      = node;
    else if (node->mb == mb_empty_filter) node_empty_filter = node;
    
    /* sanity checks */
    if (node_nofilter == NULL          || 
        node_filter == NULL            || 
        node_filter_fdr == NULL        ||
        node_empty == NULL        ||
        node_empty_filter == NULL       
        ) return 1001;

    
    return 0;
}

/* clean up test environment */
int clean_cr(void) {

    int rc;
    MBIt_Board *board;
    
    /* Delete filters */
    MB_Filter_Delete(&filter_map);
    MB_Filter_Delete(&filter_fdr);
    
    /* Delete index map */
    MB_IndexMap_Delete(&indexmap);
    
    /* unlock boards */
    board = (MBIt_Board*)MBI_getMBoardRef(mb_nofilter);
    assert(board != NULL);
    board->locked = MB_FALSE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_filter);
    assert(board != NULL);
    board->locked = MB_FALSE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_filter_fdr);
    assert(board != NULL);
    board->locked = MB_FALSE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_empty);
    assert(board != NULL);
    board->locked = MB_FALSE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_empty_filter);
    assert(board != NULL);
    board->locked = MB_FALSE;
    
    /* Delete boards */
    rc = MB_Delete(&mb_nofilter);
    assert(rc == MB_SUCCESS);
    rc = MB_Delete(&mb_filter);
    assert(rc == MB_SUCCESS);
    rc = MB_Delete(&mb_filter_fdr);
    assert(rc == MB_SUCCESS);
    rc = MB_Delete(&mb_empty_filter);
    assert(rc == MB_SUCCESS);
    rc = MB_Delete(&mb_empty);
    assert(rc == MB_SUCCESS);
    
    /* deallocate Object Maps */
    MBI_objmap_destroy(&MBI_OM_mboard);
    MBI_objmap_destroy(&MBI_OM_iterator);
    MBI_objmap_destroy(&MBI_OM_filter);
    MBI_objmap_destroy(&MBI_OM_indexmap);
    
    /* deallocate string map */
    rc = MBI_stringmap_Delete(&MBI_indexmap_nametable);
    assert(rc == MB_SUCCESS);
    
    /* free our MPI_Communicator */
    MPI_Comm_free(&MBI_CommWorld);
    
    /* finalise MPI */
    MPI_Finalize();
    
    /* message to users */
    if (MBI_CommSize < 4 && MBI_CommRank == 0)
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
    
    /* delete commqueue */
    MBI_CommQueue_Delete();
    
    return 0;
}

