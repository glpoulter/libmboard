/* $Id$ */
/*!
 * \file parallel/util_syncqueue.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief management of Sync Request Queue
 * 
 */
#include "mb_parallel.h"
#include "mb_syncqueue.h"

struct MBIt_SyncRequest_queuenode {
    MBt_Board mb;
    struct MBIt_SyncRequest_queuenode *next;
    struct MBIt_SyncRequest_queuenode *prev;
};

/* circular queue */
struct MBIt_SyncRequest_queue {
    unsigned int count;
    struct MBIt_SyncRequest_queuenode *head;
    struct MBIt_SyncRequest_queuenode *tail;
};

/* file local vars */ 
static struct MBIt_SyncRequest_queue SRQ; /* Sync Request Queue */

/* file local routine */
static void resetSRQ(struct MBIt_SyncRequest_queue *srq);

/* global vars */
pthread_mutex_t MBI_SRQLock;
pthread_cond_t  MBI_SRQCond;

/* initialise sync request queue */
int MBI_SyncQueue_Init(void) {
    
    int rc;
    int errfound = 0;
    
    /* initialise mutex lock */
    rc = pthread_mutex_init(&MBI_SRQLock, NULL);
    if (rc != 0) errfound++;
    assert(rc == 0);

    /* initialise pthread condition */
    rc = pthread_cond_init(&MBI_SRQCond, NULL);
    if (rc != 0) errfound++;
    assert(rc == 0);
    
    /* initialise values in queue */
    SRQ.count = 0;
    SRQ.head  = NULL;
    SRQ.tail  = NULL;
    
    if (errfound != 0) return MB_ERR_INTERNAL;
    else return MB_SUCCESS;
    
}

/* Delete sync request queue */
int MBI_SyncQueue_Delete(void) {
    
    int rc;
    int errfound = 0;
    
    /* destroy mutex lock */
    rc = pthread_mutex_destroy(&MBI_SRQLock);
    if (rc != 0) errfound++;
    assert(rc == 0);
    
    /* destroy pthread condition var */
    rc = pthread_cond_destroy(&MBI_SRQCond);
    if (rc != 0) errfound++;
    assert(rc == 0);
    
    /* initialise values in queue */
    resetSRQ(&SRQ);
    
    if (errfound != 0) return MB_ERR_INTERNAL;
    else return MB_SUCCESS;
}   

/* clear linked list */
static void resetSRQ(struct MBIt_SyncRequest_queue *srq) {
    
    struct MBIt_SyncRequest_queuenode *node;
    
    assert(srq != NULL);
    if (srq == NULL) return;
    
    srq->count = 0;
    srq->tail = NULL;
    
    while(srq->head)
    {
        node = srq->head;
        srq->head = node->next;
        free(node);
    }
}

/* quick check if queue empty */
int MBI_SyncQueue_isEmpty(void) {
    /* this might be called when lock already held so do not 
     * capture lock here.
     */
    return (SRQ.count == 0);
}

/* add item to SRQ */
int MBI_SyncQueue_Push(MBt_Board mb) {
    
    int rc;
    
    struct MBIt_SyncRequest_queuenode *node;
    node = (struct MBIt_SyncRequest_queuenode *)
            malloc(sizeof(struct MBIt_SyncRequest_queuenode));
    assert(node != NULL);
    if (node == NULL) return MB_ERR_MEMALLOC;
    
    /* assign give mb handle to sync request */
    node->mb = mb;
    
    rc = pthread_mutex_lock(&MBI_SRQLock); /* capture lock */
    assert(0 == rc);
    
    /* add node to head of queue */
    if (SRQ.head != NULL) /* if NOT first node in list */
    {
        SRQ.head->prev = node;
        node->next = SRQ.head;
        node->prev = NULL;
        SRQ.head   = node;
    }
    else
    {
        assert(SRQ.tail == NULL);
        node->next = NULL;
        node->prev = NULL;
        SRQ.head = node;
        SRQ.tail = node;
    }

    SRQ.count++;
    
    rc = pthread_mutex_unlock(&MBI_SRQLock); /* release lock */
    assert(0 == rc);
    
    rc = pthread_cond_signal(&MBI_SRQCond); /* wake comm thread if sleeping */
    assert(0 == rc);

    return MB_SUCCESS;
}

/* pop an item from queue, MB_NULL_BOARD if queue empty */
int MBI_SyncQueue_Pop(MBt_Board *mb) {
    
    int rc;
    struct MBIt_SyncRequest_queuenode *node;
    
    rc = pthread_mutex_lock(&MBI_SRQLock); /* capture lock */
    assert(0 == rc);
    
    if (SRQ.count == 0)
    {
        assert(SRQ.head == NULL);
        assert(SRQ.tail == NULL);
        
        rc = pthread_mutex_unlock(&MBI_SRQLock); /* release lock */
        assert(0 == rc);
        
        *mb = MB_NULL_MBOARD;
        return MB_SUCCESS;
    }
    else if (SRQ.count > 1) /* more than one item in queue */
    {
        assert(SRQ.head != SRQ.tail);
        assert(SRQ.head != NULL);
        assert(SRQ.tail != NULL);
        
        node = SRQ.tail;
        SRQ.tail = node->prev;
        SRQ.tail->next = NULL;
        
    }
    else /* one item left in queue */
    {
        assert(SRQ.count == 1); 
        assert(SRQ.head == SRQ.tail);
        
        node = SRQ.tail;
        SRQ.head = NULL;
        SRQ.tail = NULL;
    }
    
    SRQ.count--;
    *mb = node->mb;
    free(node);
    
    rc = pthread_mutex_unlock(&MBI_SRQLock); /* release lock */
    assert(0 == rc);
    
    return MB_SUCCESS;
}
