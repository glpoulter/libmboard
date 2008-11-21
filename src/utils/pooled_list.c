/* $Id$ */
/*!
 * \file pooled_list.c
 * 
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief ADT for linked-list that uses memory pooling
 * 
 * \note To use pooled_list, ::MB_CONFIG_SERIAL_POOLSIZE must be defined.
 * 
 * \todo Allow MB_CONFIG_*_POOLSIZE to be read from environment variable
 */
/* 
 * 
 * Notes : (move this to pl_newnode() doc)
 *         
 */

#include "mb_pooled_list.h"

/* function prototypes for routines local to this file */
static void free_pl_object(pooled_list **pl_ptr_loc);

/*!
 * \brief Creates a new Pooled List object 
 * \ingroup PLIST
 * \param[out] pl_ptr_loc Address to write reference to newly created object
 * \param[in] elem_size Size of nodes that will be store in list
 * \param[in] pool_size Number of nodes per memory block
 * 
 * Memory for pooled list object and first memory block is allocated. Internal 
 * counters and positional pointers are initialised.
 * 
 * If an error occurs, \c pl_ptr_loc will be assigned \c NULL and the routine 
 * returns with the appropriate error code.
 * 
 * Possible return codes:
 *  - ::PL_SUCCESS
 *  - ::PL_ERR_INVALID (Invalid value for \c elem_size or \c pool_size)
 *  - ::PL_ERR_MALLOC (Unable to allocate required memory)
 */
int pl_create(pooled_list **pl_ptr_loc, size_t elem_size, int pool_size) {
    
    void *mem_block;
    pooled_list *new;
    
    /* set return ptr to NULL before we begin, in case of errors */
    *pl_ptr_loc = NULL;
   
    if ((int)elem_size <= 0 || (int)pool_size <= 0)
    {
        return PL_ERR_INVALID;
    }
    
    /* allocate required memory for PL object */
    new = (pooled_list *)malloc(sizeof(pooled_list));
    if (new == NULL) /* on error */
    {
        return PL_ERR_MALLOC;
    }
    assert(new != NULL);
    
    /* allocate first memory block */
    mem_block = (void *)malloc((elem_size + PL_ADDRNODE_SIZE) * pool_size); 
    assert(mem_block != NULL);
    if (mem_block == NULL) /* on malloc error */
    {
        free_pl_object(&new);
        return PL_ERR_MALLOC;
    }
    
    /* allocate address list node */
    new->addr_list = (pl_address_node *)malloc(sizeof(pl_address_node));
    assert(new->addr_list != NULL);
    if (new->addr_list == NULL) /* on malloc error */
    {
        free(mem_block);
        free_pl_object(&new);
        return PL_ERR_MALLOC;
    }  
    
    
    /* assign first memory block to out address list */
    new->addr_list->addr = mem_block;
    new->addr_list->next = NULL;
    new->active_memblock = new->addr_list;
    new->next_free = mem_block;
    new->head      = NULL;
    new->tail      = NULL;
    
    /* populate default values to new PL object */
    new->elem_size     = (unsigned int)elem_size;
    new->elem_count    = (unsigned int)pool_size;
    new->count_total   = new->elem_count;
    new->count_free    = new->count_total;
    new->count_current = 0;
    
    /* assign allocated object to input argument */
    *pl_ptr_loc = new;
    
    return PL_SUCCESS;
}
                          

/* create a new linked-list tail node within pool. Return reference via *ptr */ 
/*!
 * \brief Allocates a new node within a pooled list
 * \ingroup PLIST
 * \param[in] pl Reference to pooled list object
 * \param[out] ptr_new Address to write reference to newly allocated node
 * 
 * pl_newnode() returns a pointer to the newly assigned node. Users can populate 
 * the node with data throught the returned pointer. This pointer should not be
 * freed by users.
 * 
 * Additional memory blocks will automatically be allocated as more nodes are 
 * added and the current memory pool fills up.
 * 
 * If an error occurs, \c ptr_new will be assigned \c NULL and the routine 
 * returns with the appropriate error code.
 * 
 * Possible return codes:
 *  - ::PL_SUCCESS
 *  - ::PL_ERR_INVALID (Invalid value for \c pl)
 *  - ::PL_ERR_MALLOC (Unable to allocate required memory)
 */
int pl_newnode(pooled_list *pl, void **ptr_new) {
    
    char *addr;
    void *prev_tail;
    pl_address_node *addr_node;
    pl_address_node *dh; /* datablock header */
    
    /* reset pointers */
    *ptr_new = NULL;
    
    /* check if given pooled_list is available */
    if (!pl) return PL_ERR_INVALID;
    assert(pl != NULL);
    
    /* since we always allocate a new block whenever there is no
     * free space left, p1->count_free should never be less than 1
     */
    assert((int)pl->count_free > 0);
    
    /* modify the appropriate counters */
    pl->count_free--;
    pl->count_current++;
    
    /* move "tail" pointer */
    prev_tail = pl->tail; /* store prev tail value */
    pl->tail  = pl->next_free;

    /* populate data header for new node */
    dh = (pl_address_node *)pl->tail;
    dh->next = NULL;
    dh->addr = (void*)((char*)dh + PL_ADDRNODE_SIZE);
    
    /*  assign return pointer */
    *ptr_new = dh->addr;
    
    /* if this is the first elem in list, assign head pointer */
    if (pl->count_current == 1) { pl->head = pl->next_free; }
    
    /* connect up linked list */
    if (prev_tail == NULL) 
    { 
        dh = (pl_address_node*)pl->head;
        dh->next = NULL;
    }
    else  
    { 
        dh = (pl_address_node*)prev_tail;
        dh->next = pl->tail;
    }
    
    if (pl->count_free > 0) /* still more space in current block */
    {
        /* time for some nasty address arithmetic */
        
        /* assign next_free pointer */
        addr = (char *)pl->next_free;
        addr += (pl->elem_size + PL_ADDRNODE_SIZE);
        pl->next_free = (void *)addr;
     
        return PL_SUCCESS;
    }
    else /* need to move on to next block */
    {     
        /* if next memory block not yet available, allocate some */
        if (!pl->active_memblock->next)
        {
            
            /* allocate new address list node */
            addr_node = (pl_address_node *)malloc(sizeof(pl_address_node));
            assert(addr_node != NULL);
            if (addr_node == NULL)
            {
                pl->next_free = NULL;/*trip assert if return code not handled*/
                return PL_ERR_MALLOC;
            }
            addr_node->next = NULL;
            pl->active_memblock->next = addr_node;
            
            /* allocate memory block */
            addr_node->addr = malloc((size_t)((pl->elem_size + PL_ADDRNODE_SIZE) 
                                              * pl->elem_count));

            assert(addr_node->addr != NULL);
            if (addr_node->addr == NULL)
            {
                return PL_ERR_MALLOC;
            }
            pl->active_memblock->next = addr_node;
            pl->active_memblock = addr_node;
        }
        else /* recycle memblock */
        {
            pl->active_memblock = pl->active_memblock->next;
        }
        
        /* next node will go into new memblock */
        pl->next_free = pl->active_memblock->addr;
        
        /* we now have pl->elem_count more spaces in */
        pl->count_free  += pl->elem_count;
        pl->count_total += pl->elem_count;
       
        return PL_SUCCESS;
    }
}

/*!
 * \brief Deletes pooled list object and deallocates all memory
 * \ingroup PLIST
 * \param[in,out] pl_ptr_loc Pointer to pooled list reference
 * 
 * Once the pooled list is deleted, all pointers to nodes (returned 
 * by pl_getnode() or obtained indirectly via list traversal) will
 * no longer be valid. It is the users' responsibility to ensure 
 * that pointes to nodes are not dereferenced after the pooled list
 * is deleted or cleared.
 * 
 * Possible return codes:
 *  - ::PL_SUCCESS
 *  - ::PL_ERR_INVALID (\c pl_ptr_loc is \c NULL)
 */
int pl_delete(pooled_list **pl_ptr_loc) {
    
    if (*pl_ptr_loc == NULL) return PL_ERR_INVALID;
    
    free_pl_object(pl_ptr_loc);
    
    return PL_SUCCESS;
}

/*!
 * \brief Resets all internal counters without deallocating memory.
 * \ingroup PLIST
 * \param[in] pl Reference to pooled list object
 * 
 * After this routine is called, the pooled list can be used as thought
 * it was a new or reset list. Node memory is not deallocated, and will be
 * reused as nodes are added to the list.
 * 
 * Once the pooled list is recycled, all pointers to nodes (returned 
 * previously by pl_getnode() or obtained indirectly via list traversal)
 * should no longer be used. They will still be dereferenceable, but the
 * data it points to will eventually be replaced as new nodes are added 
 * to the list. Users are advised not to rely on this behaviour as it 
 * may change in the future.
 * 
 * Possible return codes:
 *  - ::PL_SUCCESS
 *  - ::PL_ERR_INVALID (\c pl is \c NULL)
 */
int pl_recycle(pooled_list *pl) {

    if (!pl) return PL_ERR_INVALID;
    assert(pl != NULL);
    
    /* if pl is not empty, addr_list should have at least one mem block */
    assert(pl->addr_list != NULL);
    assert(pl->addr_list->addr != NULL);
    
    /* reset values */
    pl->count_total = pl->elem_count;
    pl->count_free  = pl->count_total;
    pl->count_current = 0;
    pl->head = NULL;
    pl->tail = NULL;
    pl->next_free = pl->addr_list->addr;
    pl->active_memblock = pl->addr_list;
    
    return PL_SUCCESS;
    
}

/*!
 * \brief Empties the list
 * \ingroup PLIST
 * \param[in] pl Reference to pooled list object
 * 
 * After this routine is called, the pooled list can be used as thought
 * it was a new list. 
 * 
 * Once the pooled list is reset, all pointers to nodes (returned 
 * by pl_getnode() or obtained indirectly via list traversal) will
 * no longer be valid. It is the users' responsibility to ensure 
 * that pointes to nodes are not dereferenced after the pooled list
 * is deleted or cleared.
 * 
 * Possible return codes:
 *  - ::PL_SUCCESS
 *  - ::PL_ERR_INVALID (\c pl is \c NULL)
 */
int pl_reset(pooled_list *pl) {
    
    int rc;
    pl_address_node *addr_node, *willy;
    
    /* First reset all values within pl object */
    rc = pl_recycle(pl);
    
    /* if reseting failed, pass on error code */
    if (rc != PL_SUCCESS)
    {
        return rc;
    }
    
    /* now free up extra memory */
    addr_node = pl->addr_list->next; /* get second address node */
    pl->addr_list->next = NULL;      /* lob the other nodes off the list */
    while(addr_node) /* free all allocated mem */
    {
        willy = addr_node;
        addr_node = addr_node->next;
        
        assert(willy->addr != NULL); 
        free(willy->addr);
        free(willy);
    }
    
    return PL_SUCCESS;
}

/* get reference to a particular node by index */
/* ptr set to NULL for invalid index or pl */
/*!
 * \brief Gets reference to node by index
 * \ingroup PLIST
 * \param[in] pl Reference to pooled list object
 * \param[in] index node index (starting from 0)
 * \param[out] ptr Address to write node reference
 * 
 * The associated node pointer is retrived from the list and
 * returned to the user. 
 * 
 * The user must NOT deallocate memory associated 
 * with the returned pointer as the same object will still be 
 * referenced by the list. 
 * 
 * If routine is unsuccessful, \c ptr will be assigned \c NULL and an
 * appropriate error code will be returned.
 * 
 * \note Please note that if the list has been randomise using pl_randomise(),
 * this routine will still return nodes indexed in the order that they were 
 * added.
 * 
 * Possible return codes:
 *  - ::PL_SUCCESS
 *  - ::PL_ERR_INVALID (\c pl is \c NULL or \c index is invalid)
 * 
 */
int pl_getnode(pooled_list *pl, int index, void **ptr) {
    
    int i;
    int block_id;
    int block_offset;
    void *data_block;
    void *node;
    pl_address_node *addr_list;
    
    if (pl == NULL || index < 0 || index >= (int)pl->count_current) 
    {
        *ptr = NULL;
        return PL_ERR_INVALID;
    }
    
    /* quick sanity check */
    assert(pl->addr_list != NULL);
    assert(pl->active_memblock != NULL);
    assert(pl->next_free != NULL);
    assert((int)pl->elem_count > 0);
    
    /* calculate location of node */
    block_id = index / pl->elem_count;
    block_offset = index - (block_id * pl->elem_count);
    
    /* get reference to proper memory block */
    addr_list = pl->addr_list;
    for (i = 0; i < block_id; i++)
    {
        addr_list = addr_list->next;
        assert(addr_list != NULL);
    }
    data_block = addr_list->addr;
    
    /* Get reference to actual node */
    node = (void *)((char *)data_block + ((pl->elem_size + PL_ADDRNODE_SIZE) * block_offset));
    *ptr = PL_NODEDATA(node);
    
    return PL_SUCCESS;
    
}

/*!
 * \brief Randomise the linked list 
 * \ingroup PLIST
 * \param[in] pl Reference to pooled list object
 * 
 * This randomises the 'next' pointers within the data headers without
 * moving the actual data. Therefore, it is important to note that
 * only users who traverses the data as a linked list will see the randomised
 * order. Access using pl_getnode() will still see data in the order
 * that they were entered. 
 * 
 * If routine is unsuccessful, appropriate error code will be returned.
 * 
 * Possible return codes:
 *  - ::PL_SUCCESS
 *  - ::PL_ERR_MALLOC (Unable to allocate required memory)
 *  - ::PL_ERR_INVALID (\c pl is \c NULL or invalid)
 * 
 */
int pl_randomise(pooled_list *pl) {
    
    pl_address_node **node_headers;
    pl_address_node *node;
    int *index_array;
    int i, count, temp;
    double rnd_ratio;
    int rnd;
    
    if (pl == NULL) return PL_ERR_INVALID;
    if ((int)pl->count_current < 2) return PL_SUCCESS;
    
    count = (int)pl->count_current; /* number of nodes */
    
    /* allocate node header array (plus one space for NULL pointer) */
    node_headers = (pl_address_node **)malloc(sizeof(pl_address_node*)*count);
    assert(node_headers != NULL);
    if (node_headers == NULL) return PL_ERR_MALLOC;
    
    /* allocate index array */
    index_array = (int *)malloc(sizeof(int) * count);
    assert(index_array != NULL);
    if (index_array == NULL)
    {
    	free(node_headers);
    	return PL_ERR_MALLOC;
    }
    
    /* populate node header array and index array */
    node = (pl_address_node*)pl->head;
    for (i = 0; i < count; i++)
    {
        assert(node != NULL);
        node_headers[i] = node; /* store address of node header */
        node = node->next;
        
        index_array[i] = i;
    }
    assert(pl->head == node_headers[0]);
    assert(pl->tail == node_headers[count - 1]);
    assert(node == NULL);

    /* randomise the index array */
    rnd_ratio = 1.0 / (RAND_MAX + 1.0); /* ratio to scale random numbers */
    for (i = count - 1; i > 0; i--)
    {
        /* get a random number from 0 to i */
        rnd = (int)(rnd_ratio * (i) * rand());
        
        if (rnd == i) continue; /* this value stays in place */
        
        /* perform swap */
        temp = index_array[i];
        index_array[i] = index_array[rnd];
        index_array[rnd] = temp;
    }
    
    /* -------- Augment linked list to form new list ------------ */
    
    /* set head pointer */
    pl->head = node_headers[index_array[0]];
    
    /* link up nodes based on randomised index_array */
    for (i = 0; i < count - 1; i++)
    {
    	node_headers[index_array[i]]->next = node_headers[index_array[i + 1]];
    }
    
    /* identify tail node. Have it point to NULL */
    node_headers[index_array[count - 1]]->next = NULL;
    pl->tail = node_headers[index_array[count - 1]];

    /* free arrays */
    free(node_headers);
    free(index_array);
    
    return PL_SUCCESS;
}

/* ============ internal routines ======================= */

static void free_pl_object(pooled_list **pl_ptr_loc) {
    
    pl_address_node *willy;
    pooled_list *obj;
    
    if (*pl_ptr_loc == NULL) return;
    
    obj = *pl_ptr_loc; /* Get refernce to PL data */
    *pl_ptr_loc = NULL;
    
    /* iterate thru address list and free all assigned memory */
    while (obj->addr_list) 
    {
        willy = obj->addr_list;
        obj->addr_list = obj->addr_list->next;
        
        /* free referenced memory block */
        if (willy->addr) free(willy->addr);
        
        /* free address node */
        if (willy) free(willy);
    }
    
    /* free memory associated with pl object */
    free(obj);
    
}
