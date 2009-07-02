/* $Id$ */
/*!
 * \file mb_avltree.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : May 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Header file for AVL Tree 
 * 
 * \warning This utility was NOT designed to be thread-safe and should never
 * be accessed concurrently.
 */
#ifndef MB_AVLTREE_H_
#define MB_AVLTREE_H_

/*! \brief Return Code: Success */
#define AVL_SUCCESS 0
/*! \brief Alias of ::AVL_SUCCESS, just to make some code more implicit */
#define AVL_FOUND AVL_SUCCESS
/*! \brief Return Code: Invalid param or operation */
#define AVL_ERR_INVALID 1
/*! \brief Return Code: Memory allocation error */
#define AVL_ERR_MEMALLOC 2
/*! \brief Return Code: Internal error, probably a bug */
#define AVL_ERR_INTERNAL 3
/*! \brief Return Code: Routine not yet implemented */
#define AVL_ERR_NOTIMPLEMENTED 100
/*! \brief Return Code: Object not found */
#define AVL_NOTFOUND 404

#define AVL_LEFT  0
#define AVL_RIGHT 1
#define AVL_CONTROL_VAL -9999

/*! \defgroup AVLTREE Utility: AVL Balanced Binary Search Tree
 * 
 * This is our implementation of of the AVL Balanced Binary Search tree as 
 * defined in [G.M. Adelson-Velskii and E.M. Landis, "An algorithm for the 
 * organization of information.", 1962].
 * 
 * This utility was designed to be used by the parallel IndexMap routines.
 * 
 * For now, node deletion is not implemented as we only need to do insertions
 * and traversal. 
 * 
 * Each node stores a key (int) and a void* pointer to any data we wish to
 * attach to the node.
 * 
 * \warning This utilily was not designed to be thread-safe
 * 
 * @{*/

/*! \brief Data Structure representing an AVL node */
struct _MBIt_AVLnode {
    /*! \brief Node balance 
     * 
     * We only need values between -2 and 2 
     */ 
    signed int balance :4;
    
    /*! \brief value of the node */
    int key;
    /*! \brief pointer to any data attached to the node */
    void* data;
    
    /*! \brief pointers to parent nodes */
    struct _MBIt_AVLnode *parent;
    /*! \brief pointers to child nodes, where index 0 == LEFT and 1 == RIGHT */
    struct _MBIt_AVLnode *child[2];
};
/*! \brief Shorthand for the AVL node datatype */
typedef struct _MBIt_AVLnode MBIt_AVLnode;

/*! \brief Data Structure representing an AVL tree */
typedef struct {
    /*! \brief Number of nodes in tree */
    int count;
    
    /*! \brief pointer to root node */
    MBIt_AVLnode *root;
    
} MBIt_AVLtree;

/* create a new AVLtree */
MBIt_AVLtree* MBI_AVLtree_create(void);

/* Add new node to tree */
/* if duplicate key exist, *data is replaced */
int MBI_AVLtree_insert(MBIt_AVLtree *tree, int key, void* data);

/* Get a dump of all keys in tree */
int MBI_AVLtree_dump(MBIt_AVLtree *tree, int **array_ptr, int *size_ptr);

/* Get lowest keys in tree */
int MBI_AVLtree_getmin(MBIt_AVLtree *tree);

/* Get highest keys in tree */
int MBI_AVLtree_getmax(MBIt_AVLtree *tree);

/* Get reference to data attached to node. NULL if none (data or node) */
void* MBI_AVLtree_getdata(MBIt_AVLtree *tree, int key);

/* Get reference to node object referenced by key. NULL if none*/
MBIt_AVLnode* MBI_AVLtree_getnode(MBIt_AVLtree *tree, int key);

/* Query if a node of a particular key value exists in tree */
int MBI_AVLtree_memberof(MBIt_AVLtree *tree, int key);

/* Walk the tree and run node_func on each node in tree */
int MBI_AVLtree_walk(MBIt_AVLtree *tree, int (*func)(MBIt_AVLnode *node));

/* destroy tree */
void MBI_AVLtree_destroy(MBIt_AVLtree **tree_ptr);

/*! @} */
#endif /*MB_AVLTREE_H_*/
