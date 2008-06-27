/* $Id$ */
/*!
 * \file parallel/util_commqueue.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : April 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief management of Pending Communication Queue
 * 
 */

/* splint directive needed due to uthash implementation */
/*@+ignoresigns@*/

#include "mb_parallel.h"
#include "commqueue.h"

int *MBI_comm_indices = NULL;

/*! \brief pointer to CommQueue hashtable */
static struct MBIt_commqueue *CommQ = NULL;

/* internal routine to free commqueue node memory */
/*
static void free_commqueue_node(struct MBIt_commqueue *q);
*/
/*!
 * \brief Checks if CommQueue is empty 
 * \return Logical true/false 
 */
int MBI_CommQueue_isEmpty(void) {
    return (CommQ == NULL);
}

/*! 
 * \brief Initialises CommQueue hashtable
 * \return Return Code
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 */
int MBI_CommQueue_Init(void) {
    assert(CommQ == NULL);
    
    /* allocate memory for temp int array */
    MBI_comm_indices = (int*)malloc(sizeof(int) * MBI_CommSize);
    assert(MBI_comm_indices != NULL);
    if (MBI_comm_indices == NULL) return MB_ERR_MEMALLOC;
    
    return MB_SUCCESS;
}

/*! 
 * \brief Deletes an entry from the CommQueue
 * \param[in] mb Message Board handle
 * \return Return Code
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb is not in CommQueue)
 * 
 */
int MBI_CommQueue_Pop(MBt_Board mb) {
    
    struct MBIt_commqueue *node;
    
    /* retrieve obj from hashtable */
    HASH_FIND(hh, CommQ, &mb, sizeof(MBt_Board), node);
    
    if (node == NULL) return MB_ERR_INVALID;
    
    HASH_DEL(CommQ, node);
    /*
    free_commqueue_node(node);
    */
    free(node);
    return MB_SUCCESS;
}

/*! 
 * \brief Deletes the CommQueue
 * \return Return Code
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 * 
 */
int MBI_CommQueue_Delete(void) {
    
    struct MBIt_commqueue *node;
    
    while(CommQ)
    {
        node = CommQ;
        CommQ = CommQ->hh.next;
        /*
        free_commqueue_node(node);
        */
        free(node);
    }
    
    free(MBI_comm_indices);
    
    return MB_SUCCESS;
}

/*! 
 * \brief Adds a new CommQueue node and returns a reference to the node
 * \param[in] mb Message Board handle
 * \param[in] startstage Initial stage to begin communication process
 * \return Pointer to CommQueue node 
 */
int MBI_CommQueue_Push(MBt_Board mb, enum MBIt_CommStage startstage) {
    
    struct MBIt_commqueue *node;
    
    /* allocate memory for commQ node */
    node = (struct MBIt_commqueue*)malloc(sizeof(struct MBIt_commqueue));
    assert(node != NULL);
    if (node == NULL) return MB_ERR_MEMALLOC;
    
    /* initialise values */
    node->mb = mb;
    node->inbuf   = NULL;
    node->outbuf  = NULL;
    node->sendreq = NULL;
    node->recvreq = NULL;
    node->pending_in  = 0;
    node->pending_out = 0;
    node->stage   = startstage;
    
    
    /* add to hashtable */
    HASH_ADD(hh, CommQ, mb, sizeof(MBt_Board), node);
    
    return MB_SUCCESS;
}

/*! 
 * \brief Returns first node in CommQueue (NULL if empty)
 * \return Pointer to CommQueue node 
 */
struct MBIt_commqueue* MBI_CommQueue_GetFirstNode(void) {
    
    return CommQ;
}


/*!
 * \brief Deallocates memory used by CommQueue node
 * \param[in] q Pointer to CommQueue node
 */
/*
static void free_commqueue_node(struct MBIt_commqueue *q)
{
    if (q == NULL) return;
    
    assert(q->inbuf == NULL);
    assert(q->outbuf == NULL);
    assert(q->incount == NULL);
    assert(q->sendreq == NULL);
    assert(q->recvreq == NULL);

    
    free(q);
}*/
