/* $Id: searchtree_delete.c 2934 2012-07-27 14:08:11Z lsc $ */
/*!
 * \file serial/searchtree_create.c
 * \code
 *      Author: Shawn Chin
 *      Date  : July 2012
 *      Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \endcode
 *
 * \brief Serial implementation of MB_SearchTree_Delete()
 *
 */
#include "mb_serial.h"

/*!
 * \brief Deletes a Search Tree
 * \ingroup MB_API
 * \param[in,out] tree_ptr Address of SearchTree Handle
 *
 * Upon successful removal of the SearchTree object from the
 * ::MBI_OM_searchtree ObjectMap, we first delete the k-d tree associated
 * with the SearchTree and then SearchTree the Iterator object.
 *
 * \note It is valid to delete a null SearchTree (::MB_NULL_SEARCHTREE).
 * The routine will return immediately with ::MB_SUCCESS
 *
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (invalid SearchTree given)
 */
int MB_SearchTree_Delete(MBt_SearchTree *tree_ptr) {
                         MBIt_SearchTree *tree;

  /* nothing to do for null iterator */
  if (*tree_ptr == MB_NULL_SEARCHTREE) {
    P_WARNING("Deletion of null SearchTree (MB_NULL_SEARCHTREE)");
    return MB_SUCCESS;
  }

  /* pop iterator from object map */
  assert(MBI_OM_searchtree != NULL);
  assert(MBI_OM_searchtree->type == OM_TYPE_SEARCHTREE);
  tree = (MBIt_SearchTree *)MBI_objmap_pop(MBI_OM_searchtree, (OM_key_t)*tree_ptr);
  if (tree == NULL) {
    P_FUNCFAIL("Invalid SearchTree handle (%d)", (int)*tree_ptr);
    return MB_ERR_INVALID;
  }

  assert(tree != NULL);
  assert(tree->tree != NULL);

  /* free memory used by internal k-d tree object */
  MBI_KDtree_Destroy(&(tree->tree));

  /* deallocate iterator object */
  free(tree);

  P_INFO("Deleted SearchTree (%d)", (int)*tree_ptr);
  *tree_ptr = MB_NULL_SEARCHTREE;

  return MB_SUCCESS;
}
