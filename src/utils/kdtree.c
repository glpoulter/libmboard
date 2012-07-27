/* $Id$ */
/*!
 * \file kdtree.c
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
#include <string.h>
#include <float.h>
#include "mb_settings.h"
#include "mb_kdtree.h"

/* ------------ data types used only in this file ------------ */

/*! \brief alias for function signature used to extract coordinate value
 * from a void pointer to a message */
typedef double (*extract_func_t)(void*);

/*! \brief alias for function signature of comparison functions used for
 * sorting coordinate arrays by a specific dimension */
typedef int (*sort_func_t)(const void*, const void*);

/*! \brief data structure to map a message pointer to an offset within
 * a \c coord_array.
 *
 * This allows us the flexibility to assign the number of
 * dimensions for each coordinate at runtime, at the expense of an
 * extra level of indirection.
 */
struct data_map {
  double *coord; /*! Pointer to the first coordinate of this message */
  void *msg_ptr; /*! Pointer to the message */
};

/*! \brief boundaries for a specific dimension */
struct boundary {
	double min; /*! Lower bound */
	double max; /*! Upper bound */
};

/* -------------- declaration of internal routines ------------- */
static MBIt_KDtree_node* _build_kdtree(size_t idx_from, size_t idx_to,
                                       size_t depth, size_t ndims,
                                       MBIt_KDtree *tree,
                                       struct data_map *msg_array);
static void _search_kdtree(MBIt_KDtree *tree, MBIt_KDtree_node *root,
                           size_t ndims, size_t depth,
                           struct boundary* search_area,
                           struct boundary* boundaries,
                           pooled_list* result);
static inline MBIt_KDtree_node* _get_next_node(MBIt_KDtree *tree);
static inline MBIt_KDtree_node* _get_leaf_node(MBIt_KDtree *tree, 
                                               struct data_map msg);
static inline MBIt_KDtree_node* _get_branch_node(MBIt_KDtree *tree, 
                                                 double split);
static void _add_to_result(MBIt_KDtree_node *node, pooled_list *result);
static inline int _point_within_search_area(size_t ndims, double* coord,
                                            struct boundary* boundaries);
static inline int _search_area_intersects(size_t ndims,
                                          struct boundary* search_area,
                                          struct boundary* boundaries);
static inline int _completely_enclosed(size_t ndims,
                                       struct boundary* search_area,
                                       struct boundary* boundaries);

#ifdef _EXTRA_CHECKS
/*!
 * \brief Returns 1 if a node is a leaf node
 * \param[in] node A node object
 * \return 1 (true) or 0 (false)
 * Leaf nodes have both branches set to \c NULL and its \c msg_ptr
 * assigned.
 */
static inline int _is_leaf_node(MBIt_KDtree_node *node) {
  assert(node != NULL);
  if ((node->left == NULL) && (node->right == NULL)) {
    assert(node->msg_ptr != NULL);
    assert(node->coord != NULL);
    return 1;
  } else {
    return 0;
  }
}
#else
/* Assuming the tree was formed properly, all branch nodes should have both
 * branches assigned and all leaf nodes should have both branches empty.
 * In production code, it should be sufficient simply check one branch.
 */
#define _is_leaf_node(node) (node->left == NULL)
#endif

/*! \brief compare doubles return values expected by qsort() */
#define CMP_DOUBLE(v1,v2) ((v1 > v2) ? 1 : ((v1 < v2) ? -1 : 0))

/*! \brief inline function used in all the cmp functions for various dims */
static inline int cmp_dimX(const void *d1, const void *d2, size_t dim) {
  const struct data_map *m1 = (const struct data_map*)d1;
  const struct data_map *m2 = (const struct data_map*)d2;
  return CMP_DOUBLE(m1->coord[dim], m2->coord[dim]);
}

/* ---- EXTEND THIS SECTION SUPPORT MORE DIMENSIONS ------------- */
#define KDTREE_MAX_DIMS_SUPPORTED 3
/*! \brief cmp function to sort coordinates by dim 1 */
static int cmp_dim1(const void *d1, const void *d2) {
  return cmp_dimX(d1, d2, 0);
}
/*! \brief cmp function to sort coordinates by dim 2 */
static int cmp_dim2(const void *d1, const void *d2) {
  return cmp_dimX(d1, d2, 1);
}
/*! \brief cmp function to sort coordinates by dim 3 */
static int cmp_dim3(const void *d1, const void *d2) {
  return cmp_dimX(d1, d2, 2);
}
/*! \brief static array of cmp functions */
sort_func_t cmp_funcs[] = { cmp_dim1, cmp_dim2, cmp_dim3 };


/*  ------------- public interface ---------------------------- */

/* create kd tree
 * allocate and initialise required data then pass on to
 * _create_kdtree() to do the actual creation
 */
int MBI_KDtree_Create(MBIt_KDtree **tree, pooled_list* data,
                      size_t ndims, double (*extract_funcs[])(void*)) {
  size_t i, m, idx, count, num_nodes;
  void *msg;
  pl_address_node *pl_itr;
  double *coord_array; /* list of coordinates for messages */
  struct data_map *msg_array; /* cache of msg ptr + ref to coordinates */
  MBIt_KDtree *t = NULL;

  /* first, initialise the output tree to NULL in case of error conditions */
  *tree = NULL;

  /* sanity checks */
  assert(data != NULL);
  assert(ndims <= KDTREE_MAX_DIMS_SUPPORTED);

  /* determine the number of messages */
  count = data->count_current;
  assert(count < (size_t)DBL_MAX); /* as tree->val is used to store offset */

  /* allocate memory for k-d tree */
  t = malloc(sizeof(MBIt_KDtree));
  assert(t != NULL);
  if (t == NULL) return KDTREE_ERR_MEMALLOC;
  
  /* initialise values */
  t->ndims = ndims;
  t->root = NULL;
  t->next_node = 0;
  
  /* special case for empty board */
  if (count < 1) { 
    t->coord_cache = NULL;
    t->node_mem = NULL;
#ifdef _EXTRA_CHECKS
    t->max_nodes = 0;
#endif
    *tree = t;
    return KDTREE_SUCCESS;
  }

  /* allocate memory for nodes within k-d tree */
  num_nodes = ((count - 1) * 2) + 1;
  t->node_mem = malloc(sizeof(MBIt_KDtree_node) * num_nodes);
  assert(t->node_mem != NULL);
  if (t->node_mem == NULL) {
    free(t);
    return KDTREE_ERR_MEMALLOC;
  }
#ifdef _EXTRA_CHECKS
  t->max_nodes = num_nodes;
#endif

  /* allocate memory for coordinate/cache arrays */
  coord_array = malloc(sizeof(double) * ndims * count);
  assert(coord_array != NULL);
  if (coord_array == NULL) {
    free(t);
    return KDTREE_ERR_MEMALLOC;
  }

  msg_array = malloc(sizeof(struct data_map) * count);
  assert(msg_array != NULL);
  if (msg_array == NULL) {
    free(t);
    free(coord_array);
    return KDTREE_ERR_MEMALLOC;
  }

/* traverse list of messages and populate coordinate/cache arrays */
  m = 0;
  for (pl_itr = PL_ITERATOR(data); pl_itr; pl_itr = pl_itr->next) {
    msg = PL_NODEDATA(pl_itr); /* get next message */
    assert(m < count); /* sanity check */
  
    /* extract coordinates from message */
    idx = m * ndims;
    for (i = 0; i < ndims; i++) {
      coord_array[idx + i] = extract_funcs[i](msg);
    }

    /* cache ptr to messages and map it to the coordinates */
    msg_array[m].msg_ptr = msg;
    msg_array[m].coord = coord_array + idx;
  
    /* next index */
    m++;
  }
  assert(m == count);

  /* build tree */
  t->coord_cache = coord_array;
  t->root = _build_kdtree(0, count - 1, 0, ndims, t, msg_array);
  if (t->root == NULL) {
    free(msg_array);
    free(coord_array);
    return KDTREE_ERR_MEMALLOC;
  }

  /* deallocate temp arrays */
  free(msg_array);

  /* assign output ptr and return successfully */
  *tree = t;
  return KDTREE_SUCCESS;
}


/* Search kd tree
 * allocate and initialise required data then pass on to
 * _search_kdtree() to do the actual search
 */
int MBI_KDtree_Search(MBIt_KDtree *tree, pooled_list **result,
                      size_t ndims, double dim_boundaries[]) {
  int rc;
  size_t i;
  double *point;
  struct boundary *boundaries;
  struct boundary *search_area;

  *result = NULL;
  assert(tree != NULL);
  if (tree->ndims != ndims) {
    return KDTREE_ERR_INVALID;
  }

  /* allocate and initialise dimension boundaries to INF */
  boundaries = malloc(sizeof(struct boundary) * ndims);
  if (!boundaries) return KDTREE_ERR_MEMALLOC;
  for (i = 0; i < ndims; i++) {
    boundaries[i].min = -DBL_MAX;
    boundaries[i].max = DBL_MAX;
  }

  /* allocate and initialise search area based on input args */
  search_area = malloc(sizeof(struct boundary) * ndims);
  if (!search_area) {
    free(boundaries);
    return KDTREE_ERR_MEMALLOC;
  }
  for (i = 0; i < ndims; i++) {
    search_area[i].min = dim_boundaries[(i * 2)];
    search_area[i].max = dim_boundaries[(i * 2) + 1];
  }


  /* allocate memory for results (ptr to messages) */
  /* use pool sizes half of that of MBoards. If not set, default to 100 */
  rc = pl_create(result, sizeof(void*),
                (MBI_CONFIG.mempool_blocksize) ?
                (int)(MBI_CONFIG.mempool_blocksize / 2) : 100);
  if (rc != PL_SUCCESS) {
    free(boundaries);
    free(search_area);
    if (rc == PL_ERR_MALLOC) return KDTREE_ERR_MEMALLOC;
    else return KDTREE_ERR_INTERNAL;
  }

  if (tree->root != NULL) {
    if (!_is_leaf_node(tree->root)) {
      /* perform recursive search which appends results to "result" */
      _search_kdtree(tree, tree->root, ndims, 0,
                     search_area, boundaries, *result);
    } else { /* manually deal with trees with only a single leaf */
      point = tree->coord_cache; /* cache should only have one point */
      if (_point_within_search_area(ndims, point, search_area)) {
        _add_to_result(tree->root, *result);
      }
    }
  }

  free(boundaries);
  free(search_area);
  return KDTREE_SUCCESS;
}


/* deallocates object data and sets ptr to NULL */
void MBI_KDtree_Destroy(MBIt_KDtree **tree_ptr) {
  MBIt_KDtree *tree = *tree_ptr;
  if (tree == NULL) return;

  if (tree->coord_cache) free(tree->coord_cache);
  if (tree->node_mem) free(tree->node_mem);

  *tree_ptr = NULL;
  free(tree);

  return;
}

/* --------------- implementation begins ----------------------- */

/*!
 * \brief Recursively builds a k-d tree
 * \param[in] idx_from Starting index of points to consider
 * \param[in] idx_to End index (inclusive) of points to consider
 * \param[in] depth Current tree depth
 * \param[in] ndims Number of dimensions
 * \param[in] tree Reference to tree object
 * \param[in] msg_array Map between messages and their cached coordinates
 * \return Root node of the tree
 *
 * Builds the tree based on the given points (delimited by \c idx_from and
 * \c idx_to).
 *
 * If there is only one point, a leaf node is created.
 *
 * If there is more than one point, the domain is split across the median point
 * and a recursive call is made to build a tree down each branch. The dimension
 * to split the domain on is based on the depth and the number of dimensions,
 * where each dimension is chosen in turn as we go down the tree.
 *
 * The benefit of median-point splitting is that we end up with a balanced
 * tree which is great for queries. The drawback is that median calculation
 * is rather expensive.
 *
 * For now, we do a sort on the appropriate dimension to obtain the
 * median point. This is not optimal with an average time complexity of O(nlogn).
 * There are alternative approaches in the wild, some which claim linear time
 * median-finding, however they all come at the expense of either higher memory
 * requirements and are a lot more complex to implement and test.
 *
 * To minimise memory copies which sorting (and to allow for arbitrary number of
 * domains), we sort a proxy array (\c msg_array) rather than the array of
 * messages. This proxy array stores a pointer to the message and a pointer
 * to the list of coordinates (in \c coord_array) associated with that message.
 */
static MBIt_KDtree_node* _build_kdtree(size_t idx_from, size_t idx_to,
                                       size_t depth, size_t ndims,
                                       MBIt_KDtree *tree,
                                       struct data_map *msg_array) {
  double split_point;
  MBIt_KDtree_node *node;
  size_t split_dim = depth % ndims;
  size_t count = idx_to - idx_from + 1;
  size_t median_idx = idx_from + ((idx_to - idx_from) / 2);
  sort_func_t cmp = cmp_funcs[split_dim];

  /* if one point left, return as leaf node */
  assert(idx_to >= idx_from);
  if (count == 1) return _get_leaf_node(tree, msg_array[idx_to]);

  /* sort data points so we can determine the median */
  if ((ndims != (size_t)1) || (depth == (size_t)0)) { /* 1D: sort once */
    qsort((void*)(msg_array + idx_from), count,
        sizeof(struct data_map), cmp);
  }

  /* split the regions and recursively build tree down each path */
  split_point = msg_array[median_idx].coord[split_dim];
  node = _get_branch_node(tree, split_point);
  assert(node != NULL);
  assert(node->msg_ptr == NULL);
  assert(node->coord == NULL);

  node->left = _build_kdtree(idx_from, median_idx, depth + 1, ndims,
                             tree, msg_array);
  node->right = _build_kdtree(median_idx + 1, idx_to, depth + 1, ndims,
                              tree, msg_array);
  assert(node->left != NULL);
  assert(node->right != NULL);
  return node;
}


/*!
 * \brief Recursively searches tree for points that lie within a search area
 * \param[in] tree Reference to parent tree object
 * \param[in] root Root node to begin search from
 * \param[in] ndims Number of dimensions
 * \param[in] depth Current tree depth
 * \param[in] search_area Search area
 * \param[in] boundaries Boundaries of the current region
 * \param[in] result Object to append results to
 *
 * Single-leaf trees _is_leaf_node(tree->root) are handled in the caller
 * so we can assume that \c root is always a branch.
 *
 * For each branch (left and right):
 * - If it refers to a leaf, determine if the associated point is within
 *   the search area and add to result if it is.
 * - Else, calculate the new boundaries associated with this branch
 *   - Ignore this branch if it does not intersect with the search area
 *   - If the region is completely enclosed within the search area, add
 *     all leaves under this branch as a result
 *   - Else, make a recursive call to search down this branch
 */
static void _search_kdtree(MBIt_KDtree* tree, MBIt_KDtree_node* root,
                           size_t ndims, size_t depth,
                           struct boundary* search_area,
                           struct boundary* boundaries,
                           pooled_list* result) {
  struct boundary *new_boundaries;
  MBIt_KDtree_node *node;

  /* sanity check */
  assert(root != NULL);
  assert(root->left != NULL);
  assert(root->right != NULL);
  assert(result != NULL);
  assert(boundaries != NULL);
  assert(search_area != NULL);

  /* allocate memory to store new boundaries after domain is split */
  new_boundaries = malloc(sizeof(struct boundary) * ndims);

  /* explore the left branch */
  node = root->left;
  if (_is_leaf_node(node)) {
    if (_point_within_search_area(ndims, node->coord, search_area)) {
      _add_to_result(node, result);
    }
  } else { /* not leaf node. Split domain and search */
    assert(node->msg_ptr == NULL);
    memcpy(new_boundaries, boundaries, sizeof(struct boundary)*ndims);
    new_boundaries[depth % ndims].max = root->val;
    if (_search_area_intersects(ndims, search_area, new_boundaries)) {
      if (_completely_enclosed(ndims, search_area, new_boundaries)) {
        _add_to_result(node, result);
      } else {
        _search_kdtree(tree, node, ndims, depth + 1,
                       search_area, new_boundaries, result);
      }
    } /* else, ignore this branch */
  }

  /* explore the right branch */
  node = root->right;
  if (_is_leaf_node(node)) {
    if (_point_within_search_area(ndims, node->coord, search_area)) {
      _add_to_result(node, result);
    }
  } else { /* not leaf node. Split domain and search */
    assert(node->msg_ptr == NULL);
    memcpy(new_boundaries, boundaries, sizeof(struct boundary)*ndims);
    new_boundaries[depth % ndims].min = root->val;
    if (_search_area_intersects(ndims, search_area, new_boundaries)) {
      if (_completely_enclosed(ndims, search_area, new_boundaries)) {
        _add_to_result(node, result);
      } else {
        _search_kdtree(tree, node, ndims, depth + 1,
                       search_area, new_boundaries, result);
        }
    } /* else, ignore this branch */
  }

  /* clean up */
  free(new_boundaries);
}

/*!
 * \brief Returns reference to next available node from \c node_mem
 * \param[in] tree Reference to tree object
 * \return Reference to new leaf node
 */
static inline MBIt_KDtree_node* _get_next_node(MBIt_KDtree *tree) {
  assert(tree->node_mem != NULL);
#ifdef _EXTRA_CHECKS
  assert(tree->next_node < tree->max_nodes);
#endif
  return &(tree->node_mem[tree->next_node++]);
}

/*!
 * \brief Creates and returns a leaf node
 * \param[in] tree Reference to tree object
 * \param[in] msg Data map entry with the ptr and coordinates of the message
 * \return Reference to new leaf node
 *
 * \c coord_idx will be cast to \c double and stored within
 * \c node->val. Assertions are in place to ensure that \c coord_idx
 * is never larger than \c DBL_MAX.
 */
static inline MBIt_KDtree_node* _get_leaf_node(MBIt_KDtree *tree, 
                                               struct data_map msg) {
  MBIt_KDtree_node* node = _get_next_node(tree);

  node->left = NULL;
  node->right = NULL;
  node->msg_ptr = msg.msg_ptr;
  node->coord = msg.coord;
  return node;
}

/*!
 * \brief Creates and returns a branch node
 * \param[in] tree Reference to tree object
 * \param[in] split The value in which the domain is to be split
 * \return Reference to new branch node
 *
 * The dimension in which the domain is split depends on the
 * tree depth and is not encoded within the branch node.
 */
static inline MBIt_KDtree_node* _get_branch_node(MBIt_KDtree *tree, 
                                                 double split) {
  MBIt_KDtree_node* node = _get_next_node(tree);

  node->left = NULL;
  node->right = NULL;
  node->msg_ptr = NULL;
  node->coord = NULL;
  node->val = split;
  return node;
}

/*!
 * \brief Traverses a tree and adds contents of all leaf nodes to result
 * \param[in] root Root node to begin traversal from
 * \param[in,out] result Pooled list to append results to
 *
 * Recursively traverses the tree and adds the value of \c msg_ptr on all
 * leaf nodes to \c result.
 */
static void _add_to_result(MBIt_KDtree_node *root, pooled_list *result) {
  void *new;
  assert(root != NULL);
  assert(result != NULL);

  if (_is_leaf_node(root)) {
    /* add to result */
    pl_newnode(result, &new);
    assert(new != NULL);
    memcpy(new, &(root->msg_ptr), sizeof(void*));
    return;
  }

  assert(root->left != NULL);
  assert(root->right != NULL);
  _add_to_result(root->left, result);
  _add_to_result(root->right, result);
}

/*!
 * \brief determines if a point lies within a search area
 * \param[in] ndims Number of dimensions
 * \param[in] coord Array of coordinates, one for each dimension
 * \param[in] boundaries Array of boundaries, one for each dimension
 * \return 1 (true) or 0 (false)
 *
 * A point is considered to be within the search area if, for each dimension,
 * the coordinate value is not higher than the upper bounds and not lower
 * than the lower bounds.
 *
 * This routine is used when searching the k-d tree.
 */
inline static int _point_within_search_area(size_t ndims, double* coord,
                                            struct boundary* boundaries) {
  size_t i;
  assert(coord != NULL);
  assert(boundaries != NULL);
  for (i = 0; i < ndims; i++) { /* for each dimension */
    if ((coord[i] > boundaries[i].max) ||
        (coord[i] < boundaries[i].min)) {
      return 0; /* beyond boundary. return 0 (false) */
    }
  }
  return 1; /* point is within boundaries for all dimensions */
}

/*!
 * \brief determines if the search area intersects with a given region
 * \param[in] ndims Number of dimensions
 * \param[in] search_area Boundaries of the search area
 * \param[in] boundaries Boundaries of the region in question
 * \return 1 (true) or 0 (false)
 *
 * It is easier to determine the opposite of intersection, i.e. a
 * complete separation of two regions.
 *
 * The regions are considered to be completely separate if for all the
 * dimensions, the lower bound of one region is higher than the higher
 * bound of the other region (and vice versa).
 *
 * This routine is used when searching the k-d tree.
 */
inline static int _search_area_intersects(size_t ndims,
                                          struct boundary* search_area,
                                          struct boundary* boundaries) {
  size_t i;
  int completely_separate = 0;

  /* sanity check */
  assert(search_area != NULL);
  assert(boundaries != NULL);

  /* for each dimension, determine if search area is definitely beyond
   * the boundaries.
   */
  for (i = 0; i < ndims; i++) {
    /*
    completely_separate = ((search_area[i].min > boundaries[i].max) ||
                 (search_area[i].max <= boundaries[i].min));
    */
    /*
     * Because of the way we currently select the separation point
     * (sort + middle elem) points that lie on the axis may end up
     * in both regions (left and right).
     *
     * We therefore cannot discriminate against intesections on either
     * boundaries.
    */
    completely_separate = ((search_area[i].min > boundaries[i].max) ||
                           (search_area[i].max < boundaries[i].min));
    if (!completely_separate) break; /* no need to search further */
  }
  return !completely_separate;
}

/*!
 * \brief determines if a region is completely enclosed within the search area
 * \param[in] ndims Number of dimensions
 * \param[in] search_area Boundaries of the search area
 * \param[in] boundaries Boundaries of the region in question
 * \return 1 (true) or 0 (false)
 *
 * The region is considered to be completely enclosed within the search area
 * if, for each dimension, its lower and upper bounds are within the lower
 * and upper bounds of the search area.
 *
 * This routine is used when searching the k-d tree.
 */
inline static int _completely_enclosed(size_t ndims,
                                       struct boundary* search_area,
                                       struct boundary* boundaries) {
  size_t i;
  assert(search_area != NULL);
  assert(boundaries != NULL);

  for (i = 0; i < ndims; i++) {
    if ((boundaries[i].min < search_area[i].min) ||
      (boundaries[i].min > search_area[i].max) ||
      (boundaries[i].max < search_area[i].min) ||
      (boundaries[i].max > search_area[i].max)) {
      return 0;
    }
  }
  return 1;
}
