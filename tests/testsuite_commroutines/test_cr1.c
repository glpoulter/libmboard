/* $Id$ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2008
 * 
 */
#include "header_commroutines.h"

MBt_Board mb_e, mb_m, mb_x;
MBt_Function fh_even, fh_min;
struct MBIt_commqueue *node_e = NULL, *node_m = NULL, *node_x = NULL;
static int function_param = 999;
static int addMessagesToBoard(MBt_Board mb);

/* initialise test environment */
int init_cr(void) {
    
    int rc, flag;
    MBt_Board mb;
    struct MBIt_commqueue *node;
    
    /* initialise MPI environment */
    rc = MPI_Initialized(&flag);
    if (rc != MPI_SUCCESS) return rc;
    if (!flag) 
    {
        rc = MPI_Init(NULL, NULL);
        if (rc != MPI_SUCCESS) return rc;
    }
    
    /* create our own communicator */
    MPI_Comm_dup(MPI_COMM_WORLD, &MBI_CommWorld);
    MPI_Comm_rank(MBI_CommWorld, &MBI_CommRank);
    MPI_Comm_size(MBI_CommWorld, &MBI_CommSize);
    
    /* Allocate object map */
    MBI_OM_mboard   = (MBIt_objmap*)MBI_objmap_new();
    MBI_OM_iterator = (MBIt_objmap*)MBI_objmap_new();
    MBI_OM_function = (MBIt_objmap*)MBI_objmap_new();
    MBI_OM_mboard->type = OM_TYPE_MBOARD;
    MBI_OM_iterator ->type = OM_TYPE_ITERATOR;
    MBI_OM_function ->type = OM_TYPE_FUNCTION;

    /* Initialise commqueue */
    rc = MBI_CommQueue_Init();
    if (rc != MB_SUCCESS) return rc;
    
    /* register functions */
    rc = MB_Function_Register(&fh_even, &tcr_filter_even);
    if (rc != MB_SUCCESS) return rc;
    rc = MB_Function_Register(&fh_min, &tcr_filter_minimum);
    if (rc != MB_SUCCESS) return rc;
    
    /* create new board */
    rc = MB_Create(&mb, sizeof(tcr_msg_t));
    if (rc != MB_SUCCESS) return rc;
    rc = addMessagesToBoard(mb);
    if (rc != MB_SUCCESS) return rc;
    mb_x = mb;
    
    /* add board to queue */
    rc = MBI_CommQueue_Push(mb, PRE_PROPAGATION);
    if (rc != MB_SUCCESS) return rc;
    
    
    /* create another board */
    rc = MB_Create(&mb, sizeof(tcr_msg_t));
    if (rc != MB_SUCCESS) return rc;
    rc = addMessagesToBoard(mb);
    if (rc != MB_SUCCESS) return rc;
    
    /* assign func to filter only even values */
    rc = MB_Function_Assign(mb, fh_even, NULL, 0);
    if (rc != MB_SUCCESS) return rc;
    mb_e = mb;
    
    /* add board to queue */
    rc = MBI_CommQueue_Push(mb, PRE_TAGGING);
    if (rc != MB_SUCCESS) return rc;
    
    /* create another board */
    rc = MB_Create(&mb, sizeof(tcr_msg_t));
    if (rc != MB_SUCCESS) return rc;
    rc = addMessagesToBoard(mb);
    if (rc != MB_SUCCESS) return rc;
    
    /* assign func to filter only minumum values */
    rc = MB_Function_Assign(mb, fh_min, &function_param, sizeof(int));

    if (rc != MB_SUCCESS) return rc;
    mb_m = mb;
    
    /* add board to queue */
    rc = MBI_CommQueue_Push(mb, PRE_TAGGING);
    if (rc != MB_SUCCESS) return rc;
    
    /* get ptr to nodes in commqueue */
    node = MBI_CommQueue_GetFirstNode();
    if (node == NULL) return 1;
    else if (node->mb == mb_e) node_e = node;
    else if (node->mb == mb_m) node_m = node;
    else if (node->mb == mb_x) node_x = node;
    
    node = node->hh.next;
    if (node == NULL) return 1;
    else if (node->mb == mb_e) node_e = node;
    else if (node->mb == mb_m) node_m = node;
    else if (node->mb == mb_x) node_x = node;
    
    node = node->hh.next;
    if (node == NULL) return 1;
    else if (node->mb == mb_e) node_e = node;
    else if (node->mb == mb_m) node_m = node;
    else if (node->mb == mb_x) node_x = node;

    if (node_e == NULL || node_m == NULL || node_x == NULL) return 1;
    if (mb_e == mb_m || mb_e == mb_x || mb_m == mb_x) return 1;
    
    return 0;
}

/* clean up test environment */
int clean_cr(void) {
    
    /* delete boards */
    MB_Delete(&mb_e);
    MB_Delete(&mb_m);
    MB_Delete(&mb_x);
    
    /* deallocate Object Maps */
    MBI_objmap_destroy(&MBI_OM_mboard);
    MBI_objmap_destroy(&MBI_OM_iterator);
    MBI_objmap_destroy(&MBI_OM_function);
    
    /* free our MPI_Communicator */
    MPI_Comm_free(&MBI_CommWorld);
    
    /* finalise MPI */
    MPI_Finalize();
    
    /* delete commqueue */
    MBI_CommQueue_Delete();
    
    /* free registered functions */
    MB_Function_Free(&fh_min);
    MB_Function_Free(&fh_even);
    
    
    return 0;
}

/* testing MBIt_Comm_InitTagging() */
void test_cr_inittagging(void) {
    
    int i, rc;

    if (MBI_CommSize == 1)
    {
        CU_PASS("This routine will never be called when NP=1")
        return;
    }
    
    /* first, test for board with fparams == NULL */
    CU_ASSERT_EQUAL_FATAL(node_e->stage, PRE_TAGGING);
    

    CU_ASSERT_EQUAL_FATAL(node_e->mb, mb_e);
    rc = MBIt_Comm_InitTagging(node_e);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(node_e->sendreq);
    CU_ASSERT_PTR_NULL(node_e->recvreq);
    CU_ASSERT_PTR_NULL(node_e->outbuf);
    CU_ASSERT_PTR_NULL(node_e->inbuf);
    CU_ASSERT_EQUAL(node_e->stage, TAGGING);
    
    /* next, test for board with assigned fparams */
    function_param = (MBI_CommRank * TCR_MCOUNT);
    CU_ASSERT_EQUAL_FATAL(node_m->stage, PRE_TAGGING);
    CU_ASSERT_EQUAL_FATAL(node_m->mb, mb_m);
    rc = MBIt_Comm_InitTagging(node_m);
    
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_m->sendreq);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_m->recvreq);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_m->inbuf);
    CU_ASSERT_EQUAL(node_m->stage, TAGINFO_SENT);
    
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank)
        {
            CU_ASSERT_PTR_NULL(node_m->inbuf[i]);
            CU_ASSERT_EQUAL(node_m->sendreq[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node_m->recvreq[i], MPI_REQUEST_NULL);
        }
        else
        {
            CU_ASSERT_PTR_NOT_NULL(node_m->inbuf[i]);
            CU_ASSERT_NOT_EQUAL(node_m->sendreq[i], MPI_REQUEST_NULL);
            CU_ASSERT_NOT_EQUAL(node_m->recvreq[i], MPI_REQUEST_NULL);
        }
    }
    
    MPI_Barrier(MBI_CommWorld);
}

/* testing MBIt_Comm_WaitTagInfo() */
void test_cr_waittaginfo(void) {
    
    int rc;
    struct MBIt_commqueue *node;
    

    if (MBI_CommSize == 1)
    {
        CU_PASS("This routine will never be called when NP=1")
        return;
    }
    
    CU_ASSERT_EQUAL_FATAL(node_m->stage, TAGINFO_SENT);
    
    /* keep repeating call till comm is done */
    node = node_m;
    while(node->stage == TAGINFO_SENT)
    {
        rc = MBIt_Comm_WaitTagInfo(node);
        if (rc != MB_SUCCESS) CU_FAIL("MBIt_Comm_WaitTagInfo() call failed");
        
    }
    
    CU_ASSERT_PTR_NULL(node->sendreq);
    CU_ASSERT_PTR_NULL(node->recvreq);
    
    CU_ASSERT_EQUAL(node->stage, TAGGING);
    
    MPI_Barrier(MBI_CommWorld);
}

/* testing MBIt_Comm_TagMessages() */
void test_cr_tagmessages(void) {
    
    int rc;
    MBIt_Board *board;
    struct MBIt_commqueue *node;
    
    if (MBI_CommSize == 1)
    {
        CU_PASS("This routine will never be called when NP=1")
        return;
    }
    
    
    CU_ASSERT_EQUAL_FATAL(node_e->stage, TAGGING);
    CU_ASSERT_EQUAL_FATAL(node_m->stage, TAGGING);

    
    /* tag messages with even values */
    node = node_e;
    rc = MBIt_Comm_TagMessages(node);
    
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_EQUAL(node->stage, PRE_PROPAGATION);
    board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    CU_ASSERT_PTR_NOT_NULL(board->tt);
    CU_ASSERT_PTR_NOT_NULL(board->tt->table);
    CU_ASSERT_EQUAL(board->tt->cols, MBI_CommSize);
    
    /* tag messages with values > (MBI_CommRank * TCR_MCOUNT) */
    node = node_m;
    rc = MBIt_Comm_TagMessages(node);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_PTR_NULL(node->inbuf);
    CU_ASSERT_EQUAL(node->stage, PRE_PROPAGATION);
    board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    CU_ASSERT_PTR_NOT_NULL(board->tt);
    CU_ASSERT_PTR_NOT_NULL(board->tt->table);
    CU_ASSERT_EQUAL(board->tt->cols, MBI_CommSize);
    
    MPI_Barrier(MBI_CommWorld);
}

/* testing MBIt_Comm_InitPropagation() */
void test_cr_initpropagation(void) {

    int i, rc;
    MBIt_Board *board;
    struct MBIt_commqueue *node;

    if (MBI_CommSize == 1)
    {
        CU_PASS("This routine will never be called when NP=1")
        return;
    }
    
    CU_ASSERT_EQUAL_FATAL(node_e->stage, PRE_PROPAGATION);
    CU_ASSERT_EQUAL_FATAL(node_m->stage, PRE_PROPAGATION);
    CU_ASSERT_EQUAL_FATAL(node_x->stage, PRE_PROPAGATION);

    
    /* for board with no tagging */
    node = node_x;
    board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    CU_ASSERT_EQUAL(board->tagging, MB_FALSE);
    
    MPI_Barrier(MPI_COMM_WORLD);
    rc = MBIt_Comm_InitPropagation(node);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(node->stage, PROPAGATION);
    
    CU_ASSERT_PTR_NULL(board->tt);
    

    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq);
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (MBI_CommSize != 1)
        {
            if (i == 0)
            {
                CU_ASSERT_PTR_NOT_NULL(node->outbuf[i]);
            }
            else
            {
                CU_ASSERT_PTR_NULL(node->outbuf[i]);
            }
        }
        
        if (i == MBI_CommRank)
        {
            CU_ASSERT_PTR_NULL(node->inbuf[i]);
            CU_ASSERT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
            CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
        }
        else
        {
            CU_ASSERT_PTR_NOT_NULL(node->inbuf[i]);
            CU_ASSERT_NOT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
            CU_ASSERT_NOT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
        }
    }


    /* for board with even messages tagged */
    node = node_e;
    board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    
    CU_ASSERT_PTR_NOT_NULL(board->tt);
    rc = MBIt_Comm_InitPropagation(node);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(node->stage, PROPAGATION);
    

    CU_ASSERT_PTR_NULL(board->tt);
    CU_ASSERT_PTR_NOT_NULL(node->incount);
    CU_ASSERT_PTR_NOT_NULL(node->inbuf);
    CU_ASSERT_PTR_NOT_NULL(node->outbuf);
    CU_ASSERT_PTR_NOT_NULL(node->sendreq);
    CU_ASSERT_PTR_NOT_NULL(node->recvreq);
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (node->outbuf[i] == NULL) 
        {
            CU_ASSERT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
        }
        else 
        {
            CU_ASSERT_NOT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
        }
        
        if (i == MBI_CommRank)
        {
            CU_ASSERT_PTR_NULL(node->inbuf[i]);
            CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
        }
        else
        {
            if (node->incount[i] > 0)
            {
                CU_ASSERT_PTR_NOT_NULL(node->inbuf[i]);
                CU_ASSERT_NOT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
            }
            else
            {
                CU_ASSERT_PTR_NULL(node->inbuf[i]);
                CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
            }
            
        }
    }

    
    /* for board with messages of value > (MBI_CommRank * TCR_MCOUNT) tagged */
    node = node_m;
    board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    CU_ASSERT_EQUAL(board->tagging, MB_TRUE);
    
    rc = MBIt_Comm_InitPropagation(node);
    CU_ASSERT_EQUAL(rc, MB_SUCCESS);
    CU_ASSERT_EQUAL(node->stage, PROPAGATION);
    

    CU_ASSERT_PTR_NULL(board->tt);
    
    if (node->pending_in > 0)
    {
        CU_ASSERT_PTR_NOT_NULL(node->incount);
        CU_ASSERT_PTR_NOT_NULL(node->inbuf);
        CU_ASSERT_PTR_NOT_NULL(node->recvreq);
    }
    else
    {
        CU_ASSERT_PTR_NULL(node->incount);
        CU_ASSERT_PTR_NULL(node->inbuf);
        CU_ASSERT_PTR_NULL(node->recvreq);
    }
    
    if (node->pending_out > 0)
    {
        CU_ASSERT_PTR_NOT_NULL(node->outbuf);
        CU_ASSERT_PTR_NOT_NULL(node->sendreq);
    }
    else
    {
        CU_ASSERT_PTR_NULL(node->outbuf);
        CU_ASSERT_PTR_NULL(node->sendreq);
    }
    
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (node->pending_out > 0)
        {
            if (node->outbuf[i] == NULL) 
            {
                CU_ASSERT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
            }
            else 
            {
                CU_ASSERT_NOT_EQUAL(node->sendreq[i], MPI_REQUEST_NULL);
            }
        }
        
        if (node->pending_in > 0)
        {
            if (i == MBI_CommRank)
            {
                CU_ASSERT_PTR_NULL(node->inbuf[i]);
                CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
            }
            else
            {
                if (node->incount[i] > 0)
                {
                    CU_ASSERT_PTR_NOT_NULL(node->inbuf[i]);
                    CU_ASSERT_NOT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
                }
                else
                {
                    CU_ASSERT_PTR_NULL(node->inbuf[i]);
                    CU_ASSERT_EQUAL(node->recvreq[i], MPI_REQUEST_NULL);
                }
                
            }
        }
    }

    
    MPI_Barrier(MBI_CommWorld);
}

/* testing MBIt_Comm_CompletePropagation() */
void test_cr_completepropagation(void) {

    int rc, count, err;
    int expected_count;
    MBIt_Board *board;
    MBt_Board mb;
    MBt_Iterator itr;
    tcr_msg_t *mptr;
    struct MBIt_commqueue *node;
    
    
    if (MBI_CommSize == 1)
    {
        CU_PASS("This routine will never be called when NP=1")
        return;
    }
    
    CU_ASSERT_EQUAL_FATAL(node_e->stage, PROPAGATION);
    CU_ASSERT_EQUAL_FATAL(node_m->stage, PROPAGATION);
    CU_ASSERT_EQUAL_FATAL(node_x->stage, PROPAGATION);
    
    
    /* start with board with no tagging (full replication) */
    node = node_x;
    mb   = node->mb;
    board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    
    /* manually lock boards since we did not go through sync* process */
    board->locked = MB_TRUE;
    board->syncCompleted = MB_FALSE;
    
    /* repeat till comm completed */
    while (board->syncCompleted == MB_FALSE)
    {
        rc = MBIt_Comm_CompletePropagation(node);
        
        /* if still not completed */
        if (board->syncCompleted == MB_FALSE)
        {
            if (rc != MB_SUCCESS) CU_FAIL("wrong return code");
        }
        else
        {
            if (rc != MB_SUCCESS_2) CU_FAIL("wrong return code");
        }
    }
    CU_ASSERT_EQUAL(board->syncCompleted, MB_TRUE);
    
    /* unlocking is meant to be done by SyncComplete */
    CU_ASSERT_EQUAL(board->locked, MB_TRUE);
    board->locked = MB_FALSE;
    
    /* check board content */

    rc = MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    rc = MB_Iterator_GetMessage(itr, (void *)&mptr);
    if (rc != MB_SUCCESS) CU_FAIL_FATAL("Could not get message from iterator");
    count = err = 0;
    expected_count = MBI_CommSize * TCR_MCOUNT;
    while(mptr)
    {
        count++;
        if (mptr->id * 10.0 != mptr->value) err++;
        if (mptr->id < 0 || mptr->id > expected_count - 1) err++;

        free(mptr);
        
        rc = MB_Iterator_GetMessage(itr, (void *)&mptr);
        if (rc != MB_SUCCESS) CU_FAIL_FATAL("Could not get message from iterator");
    }
    
    CU_ASSERT_EQUAL(err, 0);
    CU_ASSERT_EQUAL(count, expected_count);
    MB_Iterator_Delete(&itr);
    
    /* board with even messages tagged */
    node = node_e;
    mb   = node->mb;
    board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    
    /* manually lock boards since we did not go through sync* process */
    board->locked = MB_TRUE;
    board->syncCompleted = MB_FALSE;
    
    /* repeat till comm completed */
    while (board->syncCompleted == MB_FALSE)
    {
        rc = MBIt_Comm_CompletePropagation(node);
        
        /* if still not completed */
        if (board->syncCompleted == MB_FALSE)
        {
            if (rc != MB_SUCCESS) CU_FAIL("wrong return code");
        }
        else
        {
            if (rc != MB_SUCCESS_2) CU_FAIL("wrong return code");
        }
    }
    CU_ASSERT_EQUAL(board->syncCompleted, MB_TRUE);
    
    /* unlocking is meant to be done by SyncComplete */
    CU_ASSERT_EQUAL(board->locked, MB_TRUE);
    board->locked = MB_FALSE;
    
    /* check board content */

    rc = MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    rc = MB_Iterator_GetMessage(itr, (void *)&mptr);
    if (rc != MB_SUCCESS) CU_FAIL_FATAL("Could not get message from iterator");
    count = err = 0;
    expected_count = TCR_MCOUNT + ((MBI_CommSize - 1) * TCR_MCOUNT / 2) ;
    while(mptr)
    {
        count++ ;
        
        /* local messages */
        if (mptr->id >= MBI_CommRank * TCR_MCOUNT ||
            mptr->id < (MBI_CommRank + 1) * TCR_MCOUNT - 1  )
        {
            if (mptr->id * 10.0 != mptr->value) err++;
        }
        else
        {
            if ((mptr->id % 2) == 1) err++;
        }

        
        free(mptr);
        
        rc = MB_Iterator_GetMessage(itr, (void *)&mptr);
        if (rc != MB_SUCCESS) CU_FAIL_FATAL("Could not get message from iterator");
    }
    CU_ASSERT_EQUAL(err, 0);
    CU_ASSERT_EQUAL(count, expected_count);
    MB_Iterator_Delete(&itr);
    
    
    /* board with a 'minimum' value tagged */
    node = node_m;
    mb   = node->mb;
    board = (MBIt_Board *)MBI_getMBoardRef(node->mb);
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);
    
    /* manually lock boards since we did not go through sync* process */
    board->locked = MB_TRUE;
    board->syncCompleted = MB_FALSE;
    
    /* repeat till comm completed */
    while (board->syncCompleted == MB_FALSE)
    {
        rc = MBIt_Comm_CompletePropagation(node);
        
        /* if still not completed */
        if (board->syncCompleted == MB_FALSE)
        {
            if (rc != MB_SUCCESS) CU_FAIL("wrong return code");
        }
        else
        {
            if (rc != MB_SUCCESS_2) CU_FAIL("wrong return code");
        }
        
    };
    CU_ASSERT_EQUAL(board->syncCompleted, MB_TRUE);
    
    /* unlocking is meant to be done by SyncComplete */
    CU_ASSERT_EQUAL(board->locked, MB_TRUE);
    board->locked = MB_FALSE;
    
    /* check board content */

    rc = MB_Iterator_Create(mb, &itr);
    CU_ASSERT_EQUAL_FATAL(rc, MB_SUCCESS);
    
    rc = MB_Iterator_GetMessage(itr, (void *)&mptr);
    if (rc != MB_SUCCESS) CU_FAIL_FATAL("Could not get message from iterator");
    count = err = 0;
    expected_count = (MBI_CommSize - MBI_CommRank) * TCR_MCOUNT ;
    while(mptr)
    {
        count++ ;
        
        if (mptr->id * 10.0 != mptr->value) err++;
        
        if (mptr->id < MBI_CommRank * TCR_MCOUNT ||
                mptr->id > MBI_CommSize * TCR_MCOUNT - 1) err++;

        free(mptr);
        
        rc = MB_Iterator_GetMessage(itr, (void *)&mptr);
        if (rc != MB_SUCCESS) CU_FAIL_FATAL("Could not get message from iterator");
    }
    CU_ASSERT_EQUAL(err, 0);
    CU_ASSERT_EQUAL(count, expected_count);

    MB_Iterator_Delete(&itr);
    
    /* comm queue should now be empty */
    CU_ASSERT(MBI_CommQueue_isEmpty());
    
    MPI_Barrier(MBI_CommWorld);
    
}


int tcr_filter_even(const void *msg, const void *params) {
    tcr_msg_t *m;
    
    assert(msg != NULL);
    assert(params == NULL);
    
    m = (tcr_msg_t*)msg;
    
    if (m->id % 2 == 0) return 1;
    else return 0;
}

int tcr_filter_minimum(const void *msg, const void *params) {
    int *p;
    tcr_msg_t *m;
    
    assert(msg != NULL);
    assert(params != NULL);
    
    p = (int *)params;
    m = (tcr_msg_t*)msg;
    
    if ( m->id >= *p ) return 1;
    else return 0;
}

static int addMessagesToBoard(MBt_Board mb) {
    int i, rc;
    tcr_msg_t msg;
    
    for (i = 0; i < TCR_MCOUNT; i++)
    {
        msg.id = i + (TCR_MCOUNT * MBI_CommRank);
        msg.value = msg.id * 10.0;
        
        rc = MB_AddMessage(mb, &msg);
        if (rc != MB_SUCCESS) return rc;
    }
    
    return MB_SUCCESS;
}
