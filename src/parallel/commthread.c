/* $Id$ */
/*!
 * \file parallel/commthread.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Main routine run by communication thread
 * 
 * (more ...)
 * 
 * 
 */
#include "mb_parallel.h"
#include "commqueue.h"
#include "syncqueue.h"

/* variables global to this file only */
static pthread_t ct;
static int TERMINATE = 0;

/* routines available to this file only */
static void *commthread_main(void *params);
inline static void processSyncRequests(void);
inline static void processPendingComms(void);
inline static void initiate_board_sync(MBt_Board mb);
inline static void commthread_sendTerminationSignal(void);
#define TermFlagSet() (0 != TERMINATE)

/* initialise and start communication thread */
int MBI_CommThread_Init(void) {
    
    int rc; 
    
    /* init sync queue */
    rc = MBI_SyncQueue_Init();
    assert(0 == rc);
    if (0 != rc) return rc;
    
    /* init comm queue */
    rc = MBI_CommQueue_Init();
    assert(0 == rc);
    if (0 != rc) return rc;
    
    /* start communication thread */
    rc = pthread_create(&ct, NULL, &commthread_main, NULL);
    assert(0 == rc);
    if (0 != rc) return MB_ERR_INTERNAL;
    
    return MB_SUCCESS;
}

/* join comm thread and clean up */
int MBI_CommThread_Finalise(void) {
    
    int rc;

    /* set termination flag and send wake-up signal */
    commthread_sendTerminationSignal();
    
    /* wait for communication thread to end */
    rc = pthread_join(ct, NULL);
    assert(rc == 0);
    if (rc != 0) return MB_ERR_INTERNAL;

    /* sync queue should be empty by now */
    assert(MBI_SyncQueue_isEmpty());
    rc = MBI_SyncQueue_Delete();
    assert(0 == rc);
    if (0 != rc) return rc;
  
    /* comm queue should be empty by now */
    assert(MBI_CommQueue_isEmpty());
    rc = MBI_CommQueue_Delete();
    assert(0 == rc);
    if (0 != rc) return rc;
    
    return MB_SUCCESS;
}

/* main function run by communication thread */
static void *commthread_main(void *params) {
    
    int rc;
    /* continue looping until termination flag is read */
    while (1) {
        
        /* ----- check for condition where we either sleep, or quit ----- */
        
        /* lock Sync Request Queue */
        rc = pthread_mutex_lock(&MBI_SRQLock); 
        assert(0 == rc);
        /* Comm Queue need not be locked since only used by this thread */
        
        if (MBI_SyncQueue_isEmpty() && MBI_CommQueue_isEmpty())
        {
            if (TermFlagSet())
            {
                /* unlock Sync Request Queue */
                rc = pthread_mutex_unlock(&MBI_SRQLock); 
                assert(0 == rc);
                
                break; /* breaking out of loop ends thread */
            }
            else
            {
                /* no work to do, sleep for a bit */
                rc = pthread_cond_wait(&MBI_SRQCond, &MBI_SRQLock);
                assert(0 == rc);
                
                /* unlock Sync Request Queue */
                rc = pthread_mutex_unlock(&MBI_SRQLock); 
                assert(0 == rc);
                
                continue; /* restart main loop */
            }
        }
        
        /* unlock Sync Request Queue */
        rc = pthread_mutex_unlock(&MBI_SRQLock); 
        assert(0 == rc);
        
        /* ----- if queue not empty, start processing ----- */
        while(!MBI_SyncQueue_isEmpty() || !MBI_CommQueue_isEmpty())
        {
            if (!MBI_SyncQueue_isEmpty()) processSyncRequests();
            else processPendingComms();
        }
    }
    
    pthread_exit(NULL); /* thread ends */
}

/* sets termination flag and wakes comm thread up if sleeping */
inline static void commthread_sendTerminationSignal(void) {
    
    int rc;
    
    /* set termination flag */
    TERMINATE = 1;
    
    /* wake comm thread if sleeping */
    rc = pthread_cond_signal(&MBI_SRQCond); 
    assert(0 == rc);
}

/* process pending sync requests */
inline static void processSyncRequests(void) {
    
    int rc;
    MBt_Board mb = MB_NULL_MBOARD;
    
    /* get first request */
    rc = MBI_SyncQueue_Pop(&mb);
    assert(rc == MB_SUCCESS);
    
    while(mb != MB_NULL_MBOARD) /* loop till queue empty */
    {
        initiate_board_sync(mb);
        
        /* get next request */
        rc = MBI_SyncQueue_Pop(&mb);
        assert(rc == MB_SUCCESS);
    }
}

/* process pending communications */
inline static void processPendingComms(void) {
    int rc;
    struct MBIt_commqueue *node, *next;
    
    /* loop thru comm queue */
    next = MBI_CommQueue_GetFirstNode();
    while (next)
    {
        node = next;
        next = node->hh.next;
        
        switch(node->stage)
        {
            case PRE_TAGGING:
                rc = MBIt_Comm_InitTagging(node);
                assert(rc == MB_SUCCESS);
                break;
                
            case TAGINFO_SENT:
                rc = MBIt_Comm_WaitTagInfo(node);
                assert(rc == MB_SUCCESS);
                break;
                
            case TAGGING:
                rc = MBIt_Comm_TagMessages(node);
                assert(rc == MB_SUCCESS);
                break;
                
            case PRE_PROPAGATION:
                rc = MBIt_Comm_InitPropagation(node);
                assert(rc == MB_SUCCESS);
                break;
            
            case PROPAGATION:
                rc = MBIt_Comm_CompletePropagation(node);
                assert(rc == MB_SUCCESS);
                break;
            
            default: assert("Invalid state found!!" == "");
        }
    }

}

/* initiate synchronisation of a board */
inline static void initiate_board_sync(MBt_Board mb) { 
    
    int rc;
    enum MBIt_CommStage startstage;
    MBIt_Board *board;
    
    board = (MBIt_Board*)MBI_getMBoardRef(mb);
    
    /* check if info for tagging messages is available */
    if (board->fh == MB_NULL_FUNCTION)
    {
        startstage = PRE_PROPAGATION;
    }
    else
    {
        startstage = PRE_TAGGING;
    }
    
    /* push board into communication queue */
    rc = MBI_CommQueue_Push(mb, startstage);
    assert(rc == MB_SUCCESS);

}
