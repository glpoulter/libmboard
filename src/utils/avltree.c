/* $Id$ */
/*!
 * \file avltree.c
 * 
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : May 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Implementation of the AVL binary search tree
 * 
 */

#include "mb_avltree.h"
#include <assert.h>
#include <stdlib.h>

#define GET_PARENT_TO_CHILD_DIR(p,c) (c->key > p->key) 
#define BALANCE_MODIFIER(newnode_dir) ((newnode_dir) ? 1:-1)

/*! \brief Internal data structure representing a queue element */
struct _fifo_elem_t {
    /*! \brief Pointer data stored in queue entry */
    void *ptr;
    /*! \brief Reference to next node in queue */
    struct _fifo_elem_t *next;
};

/*! \brief Internal data structure representing a FIFO queue */
struct _fifo_t {
    /*! \brief Reference to first node in queue */
    struct _fifo_elem_t *head;
    /*! \brief Reference to last node in queue */
    struct _fifo_elem_t *tail;
};

/* ------------------- internal routine ------------------------- */
static void _delete_node_data(MBIt_AVLnode *node);
static MBIt_AVLnode* _allocate_node(int key, void *data);
static int _locate_node(int key, MBIt_AVLnode* root, MBIt_AVLnode** nodeaddr);
static int _rebalance_tree(MBIt_AVLtree *tree, MBIt_AVLnode *node, int direction);
static int _rotate_single(MBIt_AVLtree *tree, MBIt_AVLnode *node, int direction);
static int _rotate_double(MBIt_AVLtree *tree, MBIt_AVLnode *node, int direction);
static struct _fifo_t* _fifo_new();
static int _fifo_push(struct _fifo_t *queue, void *ptr);
static void* _fifo_pop(struct _fifo_t *queue);
static void _fifo_delete(struct _fifo_t **queue_ptr);
static int _walk_tree(MBIt_AVLnode *node, int (*func)(MBIt_AVLnode *node));

/*!
 * \brief Creates a new AVLtree object
 * \return Pointer to newly created AVLtree (or \c NULL if unable to allocate 
 *         required memory)
 * 
 * The required memory is allocated and its initial values written in.
 * 
 */
MBIt_AVLtree* MBI_AVLtree_create(void) {
    
    MBIt_AVLtree *tree;
    
    /* Routines in this file relies on the fact that a condition would
     * return 0 or 1, while !0 == 1 and !1 == 0
     * Q: Is this defined in the C standards?
     * Q: Shouldn't we be checking this via configure.ac?
     */
    assert(AVL_RIGHT == (1 == 1));
    assert(AVL_LEFT  == (0 == 1));
    assert(AVL_LEFT  == !AVL_RIGHT);
    assert(AVL_RIGHT == !AVL_LEFT);
    
    /* allocate memory for object map */
    tree = (MBIt_AVLtree *)malloc(sizeof(MBIt_AVLtree));
    assert(tree != NULL);
    if (tree == NULL) /* on malloc error */
    {
        return NULL;
    }
    
    /* initialise values */
    tree->count = 0;
    tree->root  = NULL;
    
    /* return ptr to new obj */
    return tree;
}

/*!
 * \brief Insert a new node into tree
 * \param[in] tree Reference to tree object
 * \param[in] key Node value
 * \param[in] data data pointer to attach to node (can be \c NULL)
 * \return Error code, or ::AVL_SUCCESS;
 * 
 * If key already exist in the tree, the node will remain and the \c data 
 * pointer will be replaced with the new one. Replacing an existing pointer
 * with \c NULL will not free the object being pointed to; it is the user's
 * responsibility to ensure this does not lead to memory leaks.
 * 
 */
int MBI_AVLtree_insert(MBIt_AVLtree *tree, int key, void* data) {
    
    int rc, dir;
    MBIt_AVLnode *node, *parent;
    
    /* sanity check */
    if (tree == NULL) return AVL_ERR_INVALID;
    
    if (tree->count == 0) /* if starting from empty tree */
    {
        assert(tree->root == NULL);
        if (tree->root != NULL) return AVL_ERR_INVALID;
        
        node = _allocate_node(key, data);
        if (node == NULL) return AVL_ERR_MEMALLOC;
        
        tree->root = node;
        tree->count++;
        
        return AVL_SUCCESS;
    }
    assert(tree->root != NULL); /* if count != 0, there should be a root */
    
    /* travese the tree and locate the spot to insert new node */
    rc = _locate_node(key, tree->root, &node);
    if (rc == AVL_FOUND) /* value already exist */
    {
        assert(node->key == key);
        node->data = data; /* overwrite data value */
        return AVL_SUCCESS;
    }
    /* if not found, _locate_node returns parent node where new node should go */
    parent = node; 
    assert(parent != NULL);
    
    /* create new node */
    node = _allocate_node(key, data);
    assert(node != NULL);
    if (node == NULL) return AVL_ERR_MEMALLOC;
    
    /* determine direction */
    dir = GET_PARENT_TO_CHILD_DIR(parent, node);
    
    /* attach node to parent */
    assert(parent->child[dir] == NULL);
    parent->child[dir] = node;
    node->parent = parent;
    
    /* update count */ 
    tree->count++;
    
    /* rebalance tree and return */
    /* this involves updating balance factors and rotations if necessary */
    return _rebalance_tree(tree, parent, dir);
}


/*!
 * \brief Retrieve list of nodes as an array of keys
 * \param[in] tree Reference to tree object
 * \param[in,out] array_ptr Reference to int* to write array address to
 * \param[in,out] size_ptr Reference to int to write array size to
 * \return ::AVL_SUCCESS or error code
 * 
 * This routine will allocate and return an int array via \c array_ptr of 
 * which size is written to \c size_ptr. It is the user's responsibility to 
 * free the array when done.
 * 
 * Contents of the array will be ordered such that adding them sequentially 
 * to a new tree will require minimal node rotations.
 * 
 * If \c tree is empty, the routine will set \c size_ptr to 0 and \c array_ptr 
 *  to \c NULL, then return with ::AVL_SUCCESS.
 * 
 * If \c tree is \c NULL, the routine will set \c size_ptr to 0 and \c array_ptr 
 *  to \c NULL, then return with error ::AVL_ERR_INVALID.
 * 
 * If the required memory cannot be allocated \c size_ptr will contain the
 * number of nodes in the tree, but \c array_ptr will be \c NULL and the
 * routine will return with ::AVL_ERR_MEMALLOC.
 * 
 */
int MBI_AVLtree_dump(MBIt_AVLtree *tree, int **array_ptr, int *size_ptr) {
    
    int i, rc;
    int count;
    int *array;
    MBIt_AVLnode *node;
    struct _fifo_t *fifo;
    
    /* First, check for empty or NULL tree */
    *size_ptr  = 0;
    *array_ptr = NULL;
    if (tree == NULL) return AVL_ERR_INVALID;
    if (tree->count == 0)
    {
        assert(tree->root == NULL);
        return AVL_SUCCESS;
    }
    
    /* store node count and allocate memory for dump array */
    *size_ptr = count = tree->count;
    array = (int*)malloc(sizeof(int) * count);
    assert(array != NULL);
    if (array == NULL)
    {
        *array_ptr = array;
        return AVL_ERR_MEMALLOC;
    }
    
    /* assign return pointer for array */
    *array_ptr = array;
    
    /* perform breadth-first traversal of tree so our resulting dump
     * can be used to reconstruct a new tree with minimal node rotations
     */
    fifo = _fifo_new(); /* create a new fifo queue */
    assert(fifo != NULL);
    if (fifo == NULL) return AVL_ERR_MEMALLOC;
    
    /* get reference to root and start traversing */
    i = 0;
    node = tree->root;
    while (node != NULL)
    {
        /* push childern into FIFO queue */
        if (node->child[AVL_LEFT] != NULL)
        {
            rc = _fifo_push(fifo, (void*)node->child[AVL_LEFT]);
            assert(rc == AVL_SUCCESS);
            if (rc != AVL_SUCCESS) return rc;
        }
        if (node->child[AVL_RIGHT] != NULL)
        {
            rc = _fifo_push(fifo, (void*)node->child[AVL_RIGHT]);
            assert(rc == AVL_SUCCESS);
            if (rc != AVL_SUCCESS) return rc;
        }
        
        /* collect data */
        array[i++] = node->key;
        
        /* get next data */
        node = (MBIt_AVLnode *)_fifo_pop(fifo);
    }
    assert(i == count);
    if (i != count) return AVL_ERR_INTERNAL;
    
    /* free our fifo queue */
    _fifo_delete(&fifo);
    assert(fifo == NULL);
    
    return AVL_SUCCESS;
}

/*!
 * \brief Get lowest key value in tree
 * \param[in] tree Reference to tree object
 * \return Lowest key value, or ::AVL_CONTROL_VAL if tree is invalid of empty
 * 
 * This routine will traverse down the left-most branch of the tree
 * and return the lowest key value.
 * 
 * If tree is invalid or empty, ::AVL_CONTROL_VAL is returned. Users are advised
 * not to rely on this feature as it may change in the future. Instead,
 * always ensure that the tree is valid and not empty before using 
 * this routine.
 */
int MBI_AVLtree_getmin(MBIt_AVLtree *tree) {
    
    int value = AVL_CONTROL_VAL;
    MBIt_AVLnode* node;

    if (tree != NULL)
    {
        node  = tree->root;
        
        /* keep going left until we reach the end */
        while (node != NULL)
        {
            value = node->key;
            node  = node->child[AVL_LEFT];
        }
    }
    return value;
}

/*!
 * \brief Get highest key value in tree
 * \param[in] tree Reference to tree object
 * \return Highest key value, or ::AVL_CONTROL_VAL if tree is invalid of empty
 * 
 * This routine will traverse down the right-most branch of the tree
 * and return the highest key value.
 * 
 * If tree is invalid or empty, ::AVL_CONTROL_VAL is returned. Users are advised
 * not to rely on this feature as it may change in the future. Instead,
 * always ensure that the tree is valid and not empty before using 
 * this routine.
 */
int MBI_AVLtree_getmax(MBIt_AVLtree *tree) {
    
    int value = AVL_CONTROL_VAL;
    MBIt_AVLnode* node;

    if (tree != NULL)
    {
        node  = tree->root;
        
        /* keep going right until we reach the end */
        while (node != NULL)
        {
            value = node->key;
            node  = node->child[AVL_RIGHT];
        }
    }
    return value;
}

/*!
 * \brief Get reference to data pointer attached to a node
 * \param[in] tree Reference to tree object
 * \param[in] key Key value of target node
 * \return pointer to data, or \c NULL if node does not exist
 * 
 * Search for node with value \c key and return the pointer assigned
 * to \c node->data. If the node does not exist, or if \c tree is 
 * \c NULL, \c NULL is returned.
 * 
 */
void* MBI_AVLtree_getdata(MBIt_AVLtree *tree, int key) {
    
    MBIt_AVLnode* node;
    
    if (tree == NULL) return NULL;
    
    node = MBI_AVLtree_getnode(tree, key);
    if (node != NULL) return node->data;
    else return NULL;
}

/*!
 * \brief Get reference to node that has a certain value
 * \param[in] tree Reference to tree object
 * \param[in] key Key value of target node
 * \return pointer to node
 * 
 * Search for node with value \c key and return a pointer to the node
 * object. If the node does not exist, \c NULL is returned.
 * 
 */
MBIt_AVLnode* MBI_AVLtree_getnode(MBIt_AVLtree *tree, int key) {
    
    int rc;
    MBIt_AVLnode* node;
    
    if (tree == NULL) return NULL;
    
    rc = _locate_node(key, tree->root, &node);
    if (rc == AVL_SUCCESS) return node;
    else return NULL;
}

/*!
 * \brief Query if a node of a particular key value exists in tree
 * \param[in] tree Reference to tree object
 * \param[in] key Key value of target node
 * \return True of False
 * 
 * The tree is traversed to locate the existence of node with value \c key.
 * 
 */
int MBI_AVLtree_memberof(MBIt_AVLtree *tree, int key) {
    
    int rc;
    MBIt_AVLnode* node;
    
    /* sanity check */
    assert(tree != NULL);
    
    rc = _locate_node(key, tree->root, &node);
    if (rc == AVL_SUCCESS) return (1 == 1);
    else return (1 == 0); 
}

/*! 
 * \brief Traverse tree and run node_func on each node in tree
 * \param[in] tree Reference to AVL tree object
 * \param[in] func Function to run on each node
 * 
 * If \c tree is null, ::AVL_ERR_INVALID is returned.
 *  
 * If \c func returns a non-zero value, the traversal stops and the 
 * routine returns with the error code returned by \c func.
 * 
 * Traversal is performed depth-first, from left to right.
 */
int MBI_AVLtree_walk(MBIt_AVLtree *tree, int (*func)(MBIt_AVLnode *node)) {
    
    if (tree == NULL) return AVL_ERR_INVALID;
    if (tree->count == 0) return AVL_SUCCESS;
    
    return _walk_tree(tree->root, func);
}

static int _walk_tree(MBIt_AVLnode *node, int (*func)(MBIt_AVLnode *node)) {

    int rc;
    
    /* end of the line */
    if (node == NULL) return AVL_SUCCESS;
    
    /* left child node */
    rc = _walk_tree(node->child[AVL_LEFT], func);
    if (rc != AVL_SUCCESS) return rc;
    
    /* rigt child node */
    rc = _walk_tree(node->child[AVL_RIGHT], func);
    if (rc != AVL_SUCCESS) return rc;

    return func(node);
}

/*!
 * \brief Destrol AVLtree object and fee all node memory
 * \param[in] tree_ptr Reference to AVL Tree object
 * 
 * All memory allocated for nodes will be free and the \c tree pointer
 * set to \c NULL.
 * 
 * Memory assigned to \c data will not be freed. 
 * 
 */
void MBI_AVLtree_destroy(MBIt_AVLtree **tree_ptr) {
    
    MBIt_AVLtree *tree = *tree_ptr;
    if (tree == NULL) return;
    
    /* delete all nodes in tree */
    if (tree->root != NULL) _delete_node_data(tree->root);
    
    free(tree);
    *tree_ptr = NULL;
    return;
}

/*! 
 * \brief Recursively delete all nodes
 * \param[in] node pointer to AVL node
 * 
 */
static void _delete_node_data(MBIt_AVLnode *node) {
    
    if (node == NULL) return;
    
    _delete_node_data(node->child[0]);
    _delete_node_data(node->child[1]);
    
    free(node);
    node = NULL;
    return;
}

/*! 
 * \brief Allocate memory for a new node and initialise its values
 * \param[in] key Node value
 * \param[in] data data pointer to attach to node
 * \return Pointer to newly allocated node
 * 
 * If memory cannot be allocated, \c NULL is returned.
 * 
 */
static MBIt_AVLnode* _allocate_node(int key, void *data) {
    
    MBIt_AVLnode* node;
    
    node = (MBIt_AVLnode*)malloc(sizeof(MBIt_AVLnode));
    assert(node != NULL);
    if (node == NULL) return NULL;
    
    node->key  = key;
    node->data = data;
    node->balance  = 0;
    node->parent   = NULL;
    node->child[AVL_LEFT]  = NULL;
    node->child[AVL_RIGHT] = NULL;
    
    return node;
}

/*! 
 * \brief Locate node with a certain key valye
 * \param[in] key Node value
 * \param[in] root Reference to root node
 * \param[in,out] nodeaddr Address to write out refence to target or parent node 
 * \return ::AVL_SUCCESS of ::AVL_NOTFOUND depending on whether target node is found
 * 
 * If target node is found, the routine returns ::AVL_SUCCESS and the address of the 
 * node is return through \c nodeaddr.
 * 
 * If target node is not found, ::AVL_NOTFOUND is returned and the address of the 
 * parent node where the target should have been a child of is returned
 * through \c nodeaddr. If the tree is empty, such a parent would not exist and
 * \c nodeaddr will be set to \c NULL.
 * 
 */
static int _locate_node(int key, MBIt_AVLnode* root, MBIt_AVLnode** nodeaddr) {
    
    MBIt_AVLnode* node;
    int nodeval;
    int dir;
    
    node = root;
    if (node == NULL) 
    {
        *nodeaddr = NULL;
        return AVL_NOTFOUND;
    }
    
    nodeval = node->key;
    while (nodeval != key)
    {
        /* which direction shall we go down? */
        dir = (key > nodeval) ? AVL_RIGHT : AVL_LEFT;
        
        if (node->child[dir] == NULL) /* end of the road, Sir */
        {
            *nodeaddr = node; /* return parent node */
            return AVL_NOTFOUND;
        }

        /* proceed down the tree */
        node = node->child[dir];
        nodeval = node->key;

    }
    
    *nodeaddr = node;
    return AVL_FOUND;
}

/*! 
 * \brief Update balance factor and rotate if necessary
 * \param[in] tree Reference to AVL rree object
 * \param[in] node Reference to root node
 * \param[in] direction Direction of recently modified child node
 * \return ::AVL_SUCCESS or error code
 * 
 * This routine is recursive.
 * 
 */
static int _rebalance_tree(MBIt_AVLtree *tree, MBIt_AVLnode *node, int direction) {
    
    int dir2, modifier;
    MBIt_AVLnode *child, *grandchild;
    
    /* sanity check */
    assert(node != NULL);
    
    /* update balance factor of node */
    node->balance += BALANCE_MODIFIER(direction); /* gained ->child[dir] */
    
    /* now more balanced. Nothing left to do */
    if (node->balance == 0)  return AVL_SUCCESS;
    
    /* slightly unbalanced. Propagate factor up the tree */
    else if (node->balance == 1 || node->balance == -1)
    {
        /* ... unless we are root. Then we're done! */
        if (node->parent == NULL) 
        {
            assert(tree->root == node);
            return AVL_SUCCESS;
        }
        
        /* propagate upwards */
        return _rebalance_tree(tree, node->parent,
                               GET_PARENT_TO_CHILD_DIR(node->parent, node));
    }
    
    /* too imbalanced. Time to rotate */
    else 
    {
        assert((direction == AVL_LEFT && node->balance == -2) ||
               (direction == AVL_RIGHT && node->balance == 2) );
        assert(node->child[direction] != NULL);
        assert(node->child[direction]->balance != 0);
        
        /* locate path to grandchild */
        dir2 = (node->child[direction]->balance > 0) ? AVL_RIGHT:AVL_LEFT;
        
        /* get refence to descendents */
        child = node->child[direction];
        grandchild = child->child[dir2];
        
        /* use direction modifier so we can use same code for LR and RL */
        modifier = BALANCE_MODIFIER(direction);
        
        /* what kind of rotation do we need? */
        if (direction == dir2)  /* LL or RR case */
        {
            /* update balance factors */
            node->balance = child->balance = 0;
            
            /* perform single rotation */
            return _rotate_single(tree, node, direction); 
        }
        else /* LR or RL case */
        {
            /* update balance factors 
             * Final balance factors depend on balance factor of grandchild
             */
            if (grandchild->balance == 0)
            {
                node->balance  = 0; 
                child->balance = 0;
            }
            else if (grandchild->balance == modifier)
            {
                node->balance   = -1 * modifier;
                child->balance  = 0;
            }
            else 
            {
                node->balance  = 0;
                child->balance = modifier;
            }
            
            /* grandchild always end up with balance factor of 0 */
            grandchild->balance = 0;
            
            /* perform double rotation */
            return _rotate_double(tree, node, direction); /* LR or RL case */
        }
    }
}

/*! 
 * \brief Perform a single rotation
 * \param[in] tree Reference to AVL rree object
 * \param[in] node Reference to root node
 * \param[in] direction Direction of recently modified child node
 * \return ::AVL_SUCCESS or error code
 * 
 */
static int _rotate_single(MBIt_AVLtree *tree, MBIt_AVLnode *node, int direction) {
    
    MBIt_AVLnode *ori_parent, *temp, *child;
    int p2node_direction;
    
    assert(direction == AVL_LEFT || direction == AVL_RIGHT);
    
    /* remember original parent of this sub-tree */
    ori_parent = node->parent;

    /* child node to swap with */
    child = node->child[direction];
    assert(child != NULL);
    
    /* rotate node down */
    temp = child->child[!direction];
    child->child[!direction] = node;
    node->child[direction]   = temp;
    if (temp != NULL) temp->parent = node;
    node->parent = child;
    
    /* reattach to tree */
    if (ori_parent == NULL) /* node was the root of tree */
    {
        assert(tree != NULL);
        assert(tree->root == node);
        if (tree == NULL) return AVL_ERR_INTERNAL;
        
        /* child is now new root */
        child->parent = NULL;
        tree->root    = child;
    }
    else
    {
        /* attach child to parent of this sub-tree */
        p2node_direction = GET_PARENT_TO_CHILD_DIR(ori_parent, node);
        assert(p2node_direction == GET_PARENT_TO_CHILD_DIR(ori_parent, child));
        ori_parent->child[p2node_direction] = child;
        child->parent = ori_parent;
    }
    
    return AVL_SUCCESS;
}

/*! 
 * \brief Perform a double rotation
 * \param[in] tree Reference to AVL rree object
 * \param[in] node Reference to root node
 * \param[in] direction Direction of recently modified child node
 * \return ::AVL_SUCCESS or error code
 * 
 * This routine calls ::_rotate_single twice to achive an equivalent 
 * double rotation.
 * 
 */
static int _rotate_double(MBIt_AVLtree *tree, MBIt_AVLnode *node, int direction) {
    
    int rc;
    MBIt_AVLnode *child, *ori_grandchild;
    
    /* get ref to child and grandchild 
     * They should be in a LR or RL structure
     */
    child = node->child[direction];
    assert(child != NULL);
    assert(child->child[!direction] != NULL);
    ori_grandchild = child->child[!direction];
    
    /* perform first rotation */
    rc = _rotate_single(NULL, child, !direction);
    
    
    assert(node->child[direction] == ori_grandchild);
    assert(ori_grandchild->child[direction] == child);
    if (rc != AVL_SUCCESS) return rc;
    /* perform second rotation and we're done */
    return _rotate_single(tree, node, direction);
}

/*! 
 * \brief Create new FIFO queue
 * \return Reference to new FIFO queue object
 * 
 * \c NULL is returned if memory cannot be allocated.
 */
static struct _fifo_t* _fifo_new() {
    
    struct _fifo_t* queue;
    
    /* allocate memory */
    queue = (struct _fifo_t*)malloc(sizeof(struct _fifo_t));
    assert(queue != NULL);
    if (queue == NULL) return NULL;
    
    queue->head = NULL;
    queue->tail = NULL;
    
    return queue;
}

/*! 
 * \brief Push node into queue
 * \param[in] queue Reference to FIFO queue object
 * \param[in] ptr Pointer to add to queue
 * \return Error code
 */
static int _fifo_push(struct _fifo_t *queue, void *ptr) {
    
    struct _fifo_elem_t *elem;
    
    assert(queue != NULL);
    if (queue == NULL) return AVL_ERR_INTERNAL;
    
    /* allocate memory for elem */
    elem = (struct _fifo_elem_t *)malloc(sizeof(struct _fifo_elem_t));
    assert(elem != NULL);
    if (elem == NULL) return AVL_ERR_MEMALLOC;
    
    /* attach data to elem */
    elem->ptr  = ptr;
    elem->next = NULL;
    /* add elem to queue */
    if (queue->head == NULL) /* empty queue */
    {
        assert(queue->tail == NULL);
        queue->head = queue->tail = elem;
    }
    else /* append to back of queue */
    {
        queue->tail->next = elem;
        queue->tail = elem;
    }
    return AVL_SUCCESS;
}

/*! 
 * \brief Push value from front of queue
 * \param[in] queue Reference to FIFO queue object
 * \return Pointer data stored in queue, or \c NULL if queue is empty
 */
static void* _fifo_pop(struct _fifo_t *queue) {
    
    void *ptr;
    struct _fifo_elem_t *elem;
    
    assert(queue != NULL);
    if (queue == NULL) return NULL;
    
    if (queue->head == NULL) /* empty queue */
    {
        assert(queue->tail == NULL);
        return NULL;
    }
    else 
    {
        elem = queue->head;
        queue->head = elem->next;
        if (queue->head == NULL) /* that was last elem */
        {
            assert(queue->tail == elem);
            queue->tail = NULL;
        }
        
        ptr = elem->ptr;
        free(elem);
        return ptr;
    }
}

/*! 
 * \brief Delete queue object and all its contents
 * \param[in] queue_ptr Address of reference to queue object
 */
static void _fifo_delete(struct _fifo_t **queue_ptr) {
    
    struct _fifo_elem_t *elem, *next;
    struct _fifo_t *queue = *queue_ptr;
    
    *queue_ptr = NULL;
    elem = queue->head;
    free(queue);
    while (elem != NULL)
    {
        next = elem->next;
        free(elem);
        elem = next;
    }
}
