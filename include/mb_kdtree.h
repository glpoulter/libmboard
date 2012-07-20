/* $Id$ */
/*!
 * \file kdtree.h
 *
 * \code
 *      Author: Shawn Chin
 *      Date  : July 2012
 *      Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \endcode
 *
 * \brief kd-tree to speed up space-based filtering of messages
 *
 */
#include "mb_pooled_list.h"

#ifndef MB_KDTREE_H_
#define MB_KDTREE_H_

/*! \brief Return Code: Success */
#define KDTREE_SUCCESS 0
/*! \brief Alias of ::AVL_SUCCESS, just to make some code more implicit */
#define AVL_FOUND AVL_SUCCESS
/*! \brief Return Code: Invalid param or operation */
#define KDTREE_ERR_INVALID 1
/*! \brief Return Code: Memory allocation error */
#define KDTREE_ERR_MEMALLOC 2
/*! \brief Return Code: Internal error, probably a bug */
#define KDTREE_ERR_INTERNAL 3
/*! \brief Return Code: Routine not yet implemented */
#define KDTREE_ERR_NOTIMPLEMENTED 100

/*! \defgroup KDTREE Utility: Balanced k-d Tree
 *
 * This is our implementation of a balanced k-d tree [Freidman et al, (1977).
 * "An algorithm for finding best matches in logarithmic expected time.",
 * ACM Trans. Math. Softw., 3:209–226.] representation of messages within the
 * board to allow fast filtering of the board based on a desired search area.
 *
 * The following approach is limited to a 2D and 3D search, although an
 * implementation which supports arbitrary dimensions is possible. That shall
 * be the domain of future updates.
 *
 * \TODO Extend our kd-tree implementation to support arbitrary dimensions.
 *
 *
 * \TODO Consider using a not-so-balanced kd-tree to save on the expensive
 * median-finding sorts. Perhaps consider only a random subset of points for
 * median calculation to get an approximation.
 *
 * \warning This utility was NOT designed to be thread-safe and should never
 * be accessed concurrently.
 *
 * @{*/

/*! \brief Data Structure representing a node within the k-d tree
 *
 * We use the same structure for leaf and non-leaf nodes.
 *
 * For leaf nodes, \c msg_ptr will be assigned while \c left and
 * \c right should be \c NULL. \c split will remain unassigned.
 *
 * For non-leaf nodes, \c left, \c right, and \c split should be
 * assigned while \c msg_ptr should be \c NULL.
 * */
struct _MBIt_KDtree_node {
  struct _MBIt_KDtree_node *left;  /*! should be NULL if leaf node */
  struct _MBIt_KDtree_node *right; /*! should be NULL if leaf node */
  void *msg_ptr; /*! pointer to internal message (if leaf node) */
  double *coord; /*! pointer to point coordinate (if leaf node) */
  double val; /* split point (if branch node) */
};
/*! \brief Shorthand for the k-d tree node datatype */
typedef struct _MBIt_KDtree_node MBIt_KDtree_node;

/*! \brief Data Structure representing a k-d tree */
typedef struct {
  size_t ndims; /*! The number of dimensions to consider */
  size_t next_node; /*! Number of nodes already assigned from \c node_mem */
  MBIt_KDtree_node *node_mem; /*! memory block to assign nodes from */
  MBIt_KDtree_node *root; /*! root node of the tree */
  double *coord_cache; /*! cache of coordinates for each message */
#ifdef _EXTRA_CHECKS
  size_t max_nodes; /*! Maximum number of nodes we can assign */
#endif
} MBIt_KDtree;



/*
 * \brief Creates a k-d tree of arbitrary dimension
 * \param[out] tree Address to write reference to newly created object
 * \param[in] data Reference to object where results will be appended to
 * \param[in] ndims Number of dimensions
 * \param[in] extract_funcs Array of functions that returns the coordinate value for
 * 	          a specific dimension given an anonymous pointer to a message.
 * 	          The number of entries must match \c ndims
 *
 * This will allocate and initialise the object representing the tree and
 * assign it to \c tree.
 *
 * If an error occurs, \c tree will be assigned \c NULL and the routine
 * returns with the appropriate error code.
 *
 * The actual building of the k-d tree is done by internal _build_kdtree()
 * routine.
 *
 * \warning There is no  way for us to check that the number of entries
 * provided through \c extract_funcs matches \c ndims.
 * It is therefore the users responsibility to ensure that the correct call
 * is made. Incompatible calls will lead to erroneous behaviour or
 * run-time failures.
 *
 * Possible return codes:
 *  - ::PL_SUCCESS
 *  - ::KDTREE_ERR_MEMALLOC (Unable to allocate required memory)
 */
int MBI_KDtree_Create(MBIt_KDtree **tree, pooled_list* data,
                      size_t ndims, double (*extract_funcs[])(void*));
/*
 * \brief Search a k-d tree that matches the given dimensions
 * \param[in] tree Reference to k-d tree
 * \param[out] result Address to write reference to result object
 * \param[in] data Reference to object where results will be appended to
 * \param[in] ndims Number of dimensions
 * \param[in] dim_boundaries Array of doubles defining the boundaries for
 *            each dimension. E.g. if there are two dimensions then this
 *            should be { dim1_min, dim1_max, dim2_min, dim2_max }.
 *
 * This will allocate and initialise a pooled_list object to store results
 * and assign it to \c result.
 *
 * If an error occurs, \c result will be assigned \c NULL and the routine
 * returns with the appropriate error code.
 *
 * This routine essentially allocates and initialises all the required
 * data before calling _searck_kdtree() to perform the actual search.
 *
 * It should be possible to search a tree using dims less than that
 * which the tree was built with. However, to keep things simple at
 * this stage, we enforce the requirement that the dims must match.
 * Mismatching \c ndims will result in an erroneous call and
 * \c KDTREE_ERR_INVALID will be returned.
 *
 * \warning There is no  way for us to check that the number of entries
 * provided through \c dim_boundaries matches \c ndims.
 * It is therefore the users responsibility to ensure that the correct call
 * is made. Incompatible calls will lead to erroneous behaviour or
 * run-time failures.
 *
 * Possible return codes:
 *  - ::PL_SUCCESS
 *  - ::KDTREE_ERR_MEMALLOC (Unable to allocate required memory)
 *  - ::KDTREE_ERR_INVALID (\c number of dimensions does not match tree)
 */
int MBI_KDtree_Search(MBIt_KDtree *tree, pooled_list **result,
                      size_t ndims, double dim_boundaries[]);

/*!
 * \brief Deletes k-d tree object and deallocates all memory
 * \param[in,out] tree_ptr Pointer to k-d tree reference
 *
 * Upon deletion, \c tree_ptr will be set to \c NULL.
 */
void MBI_KDtree_Destroy(MBIt_KDtree **tree_ptr);

/*! @} */
#endif /*MB_KDTREE_H_*/
