/* $Id$ */
/*!
 * \file parallel/searchtree_create.c
 * \code
 *      Author: Shawn Chin
 *      Date  : July 2012
 *      Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \endcode
 *
 * \brief Parallel implementation of MB_SearchTree_Create*
 *
 */
#include "mb_parallel.h"


/*!
 * \brief Creates a new search tree or arbitrary dimensions
 * \param[in] mb MessageBoard handle
 * \param[out] tree_ptr Address to write reference to newly created object
 * \param[in] ndims Number of dimensions to partition search tree on
 * \param[in] extract_funcs array of function pointers that returns the
 *            the coordinate value of a specific dimension given an anonymous
 *            pointer to a message
 *
 * The SearchTree object is allocated and populated with a k-d tree object that
 * partitions messages found within the target MessageBoard. The SearchTree is
 * then registered with the ::MBI_OM_searchtree and the reference ID returned
 * by ObjectMap is then written to \c tree_ptr as the handle.
 *
 * A balanced K-d tree is used to store the partitioned data.
 * See MBI_KDtree_Create() for more details.
 *
 * There is no  way for us to check that the number of entries
 * provided through \c extract_funcs matches \c ndims. Incompatible calls
 * will lead to erroneous behaviour or run-time failures.
 *
 * This is one of the reasons why this routine is not exposed to end-users and
 * its functionality provided via wrapper functions. The number of wrapper
 * functions therefore artificially limited number of dimensions supported.
 *
 * \note The internal tree structure stores only references to the actual
 * messages within the board. This search tree is therefore invalid once
 * the board is cleared or deleted.
 *
 *
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (invalid or null board given)
 *  - ::MB_ERR_MEMALLOC (error allocating memory for search tree object)
 *  - ::MB_ERR_LOCKED (\c mb is locked)
 *  - ::MB_ERR_DISABLED (\c mb is disabled)
 *  - ::MB_ERR_INTERNAL (possible bug. Recompile and run in debug mode for hints)
 *  - ::MB_ERR_OVERFLOW (Object map overflow. Too many SearchTrees created.)
 */
static int _SearchTree_Create(MBt_Board mb, MBt_SearchTree *tree_ptr,
                              size_t ndims,
                              double (*extract_funcs[])(void*)) {
  int rc;
  OM_key_t rc_om;
  MBIt_Board *board;
  MBIt_KDtree *kdtree;
  MBIt_SearchTree *searchtree;
  *tree_ptr = MB_NULL_SEARCHTREE;

  /* Check for NULL message board */
  if (mb == MB_NULL_MBOARD) {
    P_FUNCFAIL("Cannot generate tree for null board (MB_NULL_MBOARD)");
    return MB_ERR_INVALID;
  }

  /* get ptr to board */
  board = (MBIt_Board*)MBI_getMBoardRef(mb);
  if (board == NULL) {
    P_FUNCFAIL("Invalid board handle (%d)", (int)mb);
    return MB_ERR_INVALID;
  }

  /* check if board is locked */
  if (board->locked == MB_TRUE) {
    P_FUNCFAIL("Board (%d) is locked", (int)mb);
    return MB_ERR_LOCKED;
  }

  /* check if board is "unreadable" */
  if (board->is_reader == MB_FALSE) {
    P_FUNCFAIL("Board access mode was set to non-readable");
    return MB_ERR_DISABLED;
  }

  /* Create search tree */
  rc = MBI_KDtree_Create(&kdtree, board->data, ndims, extract_funcs);
  if (rc != KDTREE_SUCCESS) {
    P_FUNCFAIL("Failed to create k-d tree object (err: %d)", rc);
    return MB_ERR_INTERNAL;
  }
  assert(kdtree != NULL);

  /* allocate SearchTree object */
  searchtree = malloc(sizeof(MBIt_SearchTree));
  assert(searchtree != NULL);
  if (searchtree == NULL) {
    P_FUNCFAIL("Could not allocate required memory");
    MBI_KDtree_Destroy(&kdtree);
    return MB_ERR_MEMALLOC;
  }

  /* populate */
  searchtree->mb = mb;
  searchtree->ndims = ndims;
  searchtree->tree = kdtree;

  /* register object */
  rc_om = MBI_objmap_push(MBI_OM_searchtree, (void*)searchtree);
  if (rc_om > OM_MAX_INDEX) {
    MBI_KDtree_Destroy(&kdtree);
    free(searchtree);

    if (rc_om == OM_ERR_MEMALLOC) {
      P_FUNCFAIL("Could not allocate required memory");
      return MB_ERR_MEMALLOC;
    } else if (rc_om == OM_ERR_OVERFLOW) {
      P_FUNCFAIL("Too many iterators created. Objmap key overflow");
      return MB_ERR_OVERFLOW;
    } else {
      P_FUNCFAIL("MBI_objmap_push() returned with err code %d", rc);
      return MB_ERR_INTERNAL;
    }
  }

  /* assign return pointer */
  *tree_ptr = (MBt_SearchTree)rc_om;
  P_INFO("SearchTree created (tree:%d, board:%d, dims:%d",
         (int)rc_om, (int)mb, (int)ndims);

  return MB_SUCCESS;
}

/*!
 * \brief Creates a 1D search tree
 * \param[in] mb MessageBoard handle
 * \param[out] tree_ptr Address to write reference to newly created object
 * \param[in] extract_dim1 Pointer to function that returns the the value of
 *            the first dimension given an anonymous pointer to a message
 *
 * This is a wrapper function to the internal _SearchTree_Create() routine.
 */
int MB_SearchTree_Create1D(MBt_Board mb, MBt_SearchTree *tree_ptr,
                          double (*extract_dim1)(void*)) {
  double (*extract_funcs[])(void*) = { extract_dim1 };
  return _SearchTree_Create(mb, tree_ptr, 1, extract_funcs);
}

/*!
 * \brief Creates a 2D search tree
 * \param[in] mb MessageBoard handle
 * \param[out] tree_ptr Address to write reference to newly created object
 * \param[in] extract_dim1 Pointer to function that returns the the value of
 *            the first dimension given an anonymous pointer to a message
 * \param[in] extract_dim2 Pointer to function that returns the the value of
 *            the second dimension given an anonymous pointer to a message
 *
 * This is a wrapper function to the internal _SearchTree_Create() routine.
 */
int MB_SearchTree_Create2D(MBt_Board mb, MBt_SearchTree *tree_ptr,
                           double (*extract_dim1)(void*),
                           double (*extract_dim2)(void*)) {
  double (*extract_funcs[])(void*) = { extract_dim1, extract_dim2 };
  return _SearchTree_Create(mb, tree_ptr, 2, extract_funcs);
}

/*!
 * \brief Creates a 1D search tree
 * \param[in] mb MessageBoard handle
 * \param[out] tree_ptr Address to write reference to newly created object
 * \param[in] extract_dim1 Pointer to function that returns the the value of
 *            the first dimension given an anonymous pointer to a message
 * \param[in] extract_dim2 Pointer to function that returns the the value of
 *            the second dimension given an anonymous pointer to a message
 * \param[in] extract_dim3 Pointer to function that returns the the value of
 *            the third dimension given an anonymous pointer to a message
 *
 * This is a wrapper function to the internal _SearchTree_Create() routine.
 */
int MB_SearchTree_Create3D(MBt_Board mb, MBt_SearchTree *tree_ptr,
                          double (*extract_dim1)(void*),
                          double (*extract_dim2)(void*),
                          double (*extract_dim3)(void*)) {
  double (*extract_funcs[])(void*) = { extract_dim1,
                                       extract_dim2,
                                       extract_dim3 };
  return _SearchTree_Create(mb, tree_ptr, 3, extract_funcs);
}
