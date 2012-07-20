/* $Id$ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Sept 2009
 * 
 */
#include "header_commroutines_handshake.h"

#define BOARD_COUNT 6

/* RW on all procs */
MBt_Board mb_rw_hs;
struct MBIt_commqueue *node_rw_hs;
/* RW on all procs. No messages */
MBt_Board mb_rw_empty_hs;
struct MBIt_commqueue *node_rw_empty_hs;
/* RW on all procs. Has filter that uses indexmap */
MBt_Board mb_rw_fmap_hs;
struct MBIt_commqueue *node_rw_fmap_hs;
/* RW on all procs. Has filter that will cause fallback to full data
 * replication when procs > 3 */
MBt_Board mb_rw_fdr_hs;
struct MBIt_commqueue *node_rw_fdr_hs;
/* RW on even procs. IDLE on odd */
MBt_Board mb_rw_id_hs;
struct MBIt_commqueue *node_rw_id_hs;
/* RO on even procs. WO on odd */
MBt_Board mb_ro_wo_hs;
struct MBIt_commqueue *node_ro_wo_hs;

/* filters */
MBt_Filter fl_map_hs, fl_fdr_hs;
/* indexmap for use with filter_map */
MBt_IndexMap imap_hs;


/* initialise test environment */
int init_cr_handshake(void) {
    
    int rc, i, v;
    MBIt_Board *board;
    struct MBIt_commqueue *node;
    
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
    rc = MB_IndexMap_Create(&imap_hs, "My Map");
    if (rc != MB_SUCCESS) return rc;
    rc = _initialise_map_values_hs(imap_hs);
    if (rc != MB_SUCCESS) return rc;
   
    /* Create filters */
    rc = MB_Filter_Create(&fl_map_hs, &fl_func_map_hs);
    if (rc != MB_SUCCESS) return rc;
    rc = MB_Filter_Create(&fl_fdr_hs, &fl_func_fdr_hs);
    if (rc != MB_SUCCESS) return rc;
    
    /* --- Create boards */

    /* RW on all procs */
    rc = MB_Create(&mb_rw_hs, sizeof(int));
    if (rc != MB_SUCCESS) return rc;
    
    /* RW on all procs. No messages */
    rc = MB_Create(&mb_rw_empty_hs, sizeof(int));
    if (rc != MB_SUCCESS) return rc;
    
    /* RW on all procs. Has filter that uses indexmap */
    rc = MB_Create(&mb_rw_fmap_hs, sizeof(int));
    if (rc != MB_SUCCESS) return rc;
    rc = MB_Filter_Assign(mb_rw_fmap_hs, fl_map_hs);
    if (rc != MB_SUCCESS) return rc;
    
    /* RW on all procs. Has filter that will cause fallback to full data
     * replication when procs > 3 */
    rc = MB_Create(&mb_rw_fdr_hs, sizeof(int));
    if (rc != MB_SUCCESS) return rc;
    rc = MB_Filter_Assign(mb_rw_fdr_hs, fl_fdr_hs);
    if (rc != MB_SUCCESS) return rc;
    
    /* RW on even procs. IDLE on odd */
    rc = MB_Create(&mb_rw_id_hs, sizeof(int));
    if (rc != MB_SUCCESS) return rc;
    if (MBI_CommRank % 2 == 0) 
        rc = MB_SetAccessMode(mb_rw_id_hs, MB_MODE_READWRITE);
    else
        rc = MB_SetAccessMode(mb_rw_id_hs, MB_MODE_IDLE);
    if (rc != MB_SUCCESS) return rc;
            
    /* RO on even procs. WO on odd */
    rc = MB_Create(&mb_ro_wo_hs, sizeof(int));
    if (rc != MB_SUCCESS) return rc;
    if (MBI_CommRank % 2 == 0) 
        rc = MB_SetAccessMode(mb_ro_wo_hs, MB_MODE_READONLY);
    else
        rc = MB_SetAccessMode(mb_ro_wo_hs, MB_MODE_WRITEONLY);
    if (rc != MB_SUCCESS) return rc;
    
    /* --- add messages to boards */
    for (i = 0; i < TEST_MESSAGE_COUNT; i++)
    {
        v = (MBI_CommRank * TEST_MESSAGE_COUNT) + i;
        rc = MB_AddMessage(mb_rw_hs, (void *)&v);
        if (rc != MB_SUCCESS) return rc;
        rc = MB_AddMessage(mb_rw_fmap_hs, (void *)&v);
        if (rc != MB_SUCCESS) return rc;
        rc = MB_AddMessage(mb_rw_fdr_hs, (void *)&v);
        if (rc != MB_SUCCESS) return rc;
        if ((MBI_CommRank % 2) == 0)
        { /* write only on even procs (odd == IDLE)*/
            rc = MB_AddMessage(mb_rw_id_hs, (void *)&v);
            if (rc != MB_SUCCESS) return rc;
        }
        else
        { /* write only on odd procs (even == RO)*/
            rc = MB_AddMessage(mb_ro_wo_hs, (void *)&v);
            if (rc != MB_SUCCESS) return rc;
        }
    }

    /* --- add boards to comm queue */
    rc = MBI_CommQueue_Push(mb_rw_hs, MB_COMM_HANDSHAKE_PRE_PROP);
    if (rc != MB_SUCCESS) return rc;
    rc = MBI_CommQueue_Push(mb_rw_empty_hs, MB_COMM_HANDSHAKE_PRE_PROP);
    if (rc != MB_SUCCESS) return rc;
    rc = MBI_CommQueue_Push(mb_rw_fmap_hs, MB_COMM_HANDSHAKE_PRE_PROP);
    if (rc != MB_SUCCESS) return rc;
    rc = MBI_CommQueue_Push(mb_rw_fdr_hs, MB_COMM_HANDSHAKE_PRE_PROP);
    if (rc != MB_SUCCESS) return rc;
    rc = MBI_CommQueue_Push(mb_rw_id_hs, MB_COMM_HANDSHAKE_PRE_PROP);
    if (rc != MB_SUCCESS) return rc;
    rc = MBI_CommQueue_Push(mb_ro_wo_hs, MB_COMM_HANDSHAKE_PRE_PROP);
    if (rc != MB_SUCCESS) return rc;
    
    /* lock boards */
    board = (MBIt_Board*)MBI_getMBoardRef(mb_rw_hs);
    if (board == NULL) return 999;
    board->locked = MB_TRUE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_rw_empty_hs);
    if (board == NULL) return 999;
    board->locked = MB_TRUE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_rw_fmap_hs);
    if (board == NULL) return 999;
    board->locked = MB_TRUE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_rw_fdr_hs);
    if (board == NULL) return 999;
    board->locked = MB_TRUE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_rw_id_hs);
    if (board == NULL) return 999;
    board->locked = MB_TRUE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_ro_wo_hs);
    if (board == NULL) return 999;
    board->locked = MB_TRUE;
    
    /* --- get reference to internal node structure */
    node_rw_hs       = NULL;
    node_rw_empty_hs = NULL;
    node_rw_fmap_hs  = NULL;
    node_rw_fdr_hs   = NULL;
    node_rw_id_hs    = NULL;
    node_ro_wo_hs    = NULL;
    
    node = MBI_CommQueue_GetFirstNode();
    for (node = MBI_CommQueue_GetFirstNode(); node != NULL; node = node->next)
    {
        if      (node->mb == mb_rw_hs)       node_rw_hs       = node;
        else if (node->mb == mb_rw_empty_hs) node_rw_empty_hs = node;
        else if (node->mb == mb_rw_fmap_hs)  node_rw_fmap_hs  = node;
        else if (node->mb == mb_rw_fdr_hs)   node_rw_fdr_hs   = node;
        else if (node->mb == mb_rw_id_hs)    node_rw_id_hs    = node;
        else if (node->mb == mb_ro_wo_hs)    node_ro_wo_hs    = node;
        else return 1002;
    }
    
    if ( /* make sure we assigned all nodes */
            node_rw_hs       == NULL ||
            node_rw_empty_hs == NULL ||
            node_rw_fmap_hs  == NULL ||
            node_rw_fdr_hs   == NULL ||
            node_rw_id_hs    == NULL ||
            node_ro_wo_hs    == NULL
       ) return 1001;
    
    return 0;
}

/* clean up test environment */
int clean_cr_handshake(void) {
    
    int rc;
    MBIt_Board *board;
    
    /* Delete index map */
    MB_IndexMap_Delete(&imap_hs);
    
    /* Delete filters */
    MB_Filter_Delete(&fl_map_hs);
    MB_Filter_Delete(&fl_fdr_hs);
    
    /* unlock boards */
    board = (MBIt_Board*)MBI_getMBoardRef(mb_rw_hs);
    assert(board != NULL);
    board->locked = MB_FALSE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_rw_empty_hs);
    assert(board != NULL);
    board->locked = MB_FALSE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_rw_fmap_hs);
    assert(board != NULL);
    board->locked = MB_FALSE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_rw_fdr_hs);
    assert(board != NULL);
    board->locked = MB_FALSE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_rw_id_hs);
    assert(board != NULL);
    board->locked = MB_FALSE;
    board = (MBIt_Board*)MBI_getMBoardRef(mb_ro_wo_hs);
    assert(board != NULL);
    board->locked = MB_FALSE;
    
    /* delete boards */
    rc = MB_Delete(&mb_rw_hs);
    assert(rc == MB_SUCCESS);
    rc = MB_Delete(&mb_rw_empty_hs);
    assert(rc == MB_SUCCESS);
    rc = MB_Delete(&mb_rw_fmap_hs);
    assert(rc == MB_SUCCESS);
    rc = MB_Delete(&mb_rw_fdr_hs);
    assert(rc == MB_SUCCESS);
    rc = MB_Delete(&mb_rw_id_hs);
    assert(rc == MB_SUCCESS);
    rc = MB_Delete(&mb_ro_wo_hs);
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

