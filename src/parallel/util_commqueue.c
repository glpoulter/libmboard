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
#include "mb_commqueue.h"


/*! \brief Shared array used to store temporary indices */
int *MBI_comm_indices = NULL;

/*! \brief pointer to CommQueue hashtable */
static struct MBIt_commqueue* CommQ = NULL;

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
 *  - ::MB_ERR_MEMALLOC
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
 * \param[in] node Pointer to CommQueue node
 * \return Return Code
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 * 
 */
int MBI_CommQueue_Pop(struct MBIt_commqueue *node) {
       
    if (CommQ == node) /* First node in list */
    {
        assert(node->prev == NULL);
        CommQ = node->next;
        if (node->next != NULL) node->next->prev = NULL;
    }
    else 
    {
        node->prev->next = node->next;
        if (node->next != NULL) node->next->prev = node->prev;
    }
    
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
    
    struct MBIt_commqueue *next;
    
    while(CommQ)
    {
        next = CommQ->next;
        free(CommQ);
        CommQ = next;
    }
    
    /* free temporary indices array */
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
    node->incount = NULL;
    node->inbuf   = NULL;
    node->outbuf  = NULL;
    node->sendreq = NULL;
    node->recvreq = NULL;
    node->pending_in  = 0;
    node->pending_out = 0;
    node->stage   = startstage;
    
    /* Add to Queue */
    if (CommQ != NULL) CommQ->prev = node;
    node->next = CommQ;
    node->prev = NULL;
    CommQ = node;
    
    return MB_SUCCESS;
}

/*! 
 * \brief Returns first node in CommQueue (NULL if empty)
 * \return Pointer to CommQueue node 
 */
struct MBIt_commqueue* MBI_CommQueue_GetFirstNode(void) {
    
    return CommQ;
}

