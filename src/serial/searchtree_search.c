/* $Id$ */
/*!
 * \file serial/searchtree_search.c
 * \code
 *      Author: Shawn Chin
 *      Date  : July 2012
 *      Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \endcode
 *
 * \brief Serial implementation of MB_SearchTree_Search*
 *
 */
#include "mb_serial.h"

/*!
 * \brief Searches a new 3D search tree and returns a list of results
 * \param[in] tree_ptr Reference to the 3D tree object
 * \param[in] Reference to pooled_list object where results will be appended to
 * \param[in] dim1_min Lower boundary for search (dimension 1)
 * \param[in] dim1_max Higher boundary for search (dimension 1)
 * \param[in] dim2_min Lower boundary for search (dimension 2)
 * \param[in] dim2_max Higher boundary for search (dimension 2)
 * \param[in] dim3_min Lower boundary for search (dimension 3)
 * \param[in] dim3_max Higher boundary for search (dimension 3)
 *
 * The k-d tree is searched using MBI_KDtree_Search() and the results returned
 * as an Iterator.
 *
 * There is no guaranteed that messages will appear in the same order as
 * they are in the boards.
 *
 * There is no  way for us to check that the number of entries
 * provided through \c dim_boundaries matches \c ndims. Incompatible calls
 * will lead to erroneous behaviour or run-time failures.
 *
 * This is one of the reasons why this routine is not exposed to end-users and
 * its functionality provided via wrapper functions. The number of wrapper
 * functions therefore artificially limited number of dimensions supported.
 *
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (invalid search tree of mismatching dimensions)
 *  - ::MB_ERR_MEMALLOC (error allocating memory)
 *  - ::MB_ERR_LOCKED (Associated message board is locked)
 *  - ::MB_ERR_INTERNAL (possible bug. Recompile and run in debug mode for hints)
 *  - ::MB_ERR_OVERFLOW (Object map overflow. Too many Iterators created.)
 */
static int _SearchTree_Search(MBt_SearchTree tree, MBt_Iterator *itr_ptr,
								size_t ndims, double dim_boundaries[]) {

	int rc;
    OM_key_t rc_om;
    pooled_list *result;
	MBIt_Board *board;
	MBIt_SearchTree *searchtree;
	MBIt_Iterator *iter;
	*itr_ptr = MB_NULL_ITERATOR;

	/* Check for NULL search tree */
    if (tree == MB_NULL_SEARCHTREE)
    {
        P_FUNCFAIL("Cannot search a null tree (MB_NULL_SEARCHTREE)");
        return MB_ERR_INVALID;
    }

    /* get ptr to search tree */
    searchtree = (MBIt_SearchTree*)MBI_getSearchTreeRef(tree);
    if (searchtree == NULL) {
        P_FUNCFAIL("Invalid SearchTree handle (%d)", (int)tree);
        return MB_ERR_INVALID;
    }

    /* check ndims match */
    if (searchtree->ndims != ndims) {
    	P_FUNCFAIL("SearchTree was created with %u dims, not %u",
    			searchtree->ndims, ndims);
    	return MB_ERR_INVALID;
    }

    /* get ptr to board */
    board = (MBIt_Board*)MBI_getMBoardRef(searchtree->mb);
    if (board == NULL)
    {
        P_FUNCFAIL("Board no longer valid (%d)", (int)searchtree->mb);
        return MB_ERR_INTERNAL;
    }

    /* check if board is locked */
    if (board->locked == MB_TRUE)
    {
        P_FUNCFAIL("Board (%d) is locked", (int)searchtree->mb);
        return MB_ERR_LOCKED;
    }

    /* do search */
    rc = MBI_KDtree_Search(searchtree->tree, &result, ndims, dim_boundaries);
    if (rc != KDTREE_SUCCESS) {
    	P_FUNCFAIL("Failed to search k-d tree object (err: %d)" % rc);
    	return MB_ERR_INTERNAL;
    }
    assert(result != NULL);

    /* create iterator object */
    iter = malloc(sizeof(MBIt_Iterator));
    assert(iter != NULL);
    if (iter == NULL)
    {
        P_FUNCFAIL("Could not allocate required memory");
        return MB_ERR_MEMALLOC;
    }

    /* assign mb handle and result to iterator */
    iter->mb        = searchtree->mb;
    iter->msgsize   = board->data->elem_size;
    iter->cursor    = NULL;
    iter->iterating = 0;
    iter->data      = result;

    /* register iter object */
    rc_om = MBI_objmap_push(MBI_OM_iterator, (void*)iter);
    if (rc_om > OM_MAX_INDEX)
    {
        if (rc_om == OM_ERR_MEMALLOC)
        {
            P_FUNCFAIL("Could not allocate required memory");
            return MB_ERR_MEMALLOC;
        }
        else if (rc_om == OM_ERR_OVERFLOW)
        {
            P_FUNCFAIL("Too many iterators created. Objmap key overflow");
            return MB_ERR_OVERFLOW;
        }
        else
        {
            P_FUNCFAIL("MBI_objmap_push() returned with err code %d", rc);
            return MB_ERR_INTERNAL;
        }
    }

    /* assign return pointer */
    *itr_ptr  = (MBt_Iterator)rc_om;

    P_INFO("Iterator created from search (iter:%d, board:%d, tree:%d, mcount:%d)",
            (int)rc_om, (int)iter->mb, (int)tree, (int)result->count_current);

    return MB_SUCCESS;
}

/*!
 * \brief Searches a 1D search tree and returns an iterator
 * \param[in] tree Handle to the 1D tree object
 * \param[in] itr_ptr Address of Iterator Handle
 * \param[in] dim1_min Lower boundary for search (dimension 1)
 * \param[in] dim1_max Higher boundary for search (dimension 1)
 *
 * This is a wrapper function to the internal _SearchTree_Search() routine.
 */
int MB_SearchTree_Search1D(MBt_SearchTree tree, MBt_Iterator *itr_ptr,
							double dim1_min, double dim1_max) {
	double dim_boundaries[] = { dim1_min, dim1_max };
	return _SearchTree_Search(tree, itr_ptr, 1, dim_boundaries);
}

/*!
 * \brief Searches a 2D search tree and returns an iterator
 * \param[in] tree Handle to the 2D tree object
 * \param[in] itr_ptr Address of Iterator Handle
 * \param[in] dim1_min Lower boundary for search (dimension 1)
 * \param[in] dim1_max Higher boundary for search (dimension 1)
 * \param[in] dim2_min Lower boundary for search (dimension 2)
 * \param[in] dim2_max Higher boundary for search (dimension 2)
 *
 * This is a wrapper function to the internal _SearchTree_Search() routine.
 */
int MB_SearchTree_Search2D(MBt_SearchTree tree, MBt_Iterator *itr_ptr,
							double dim1_min, double dim1_max,
							double dim2_min, double dim2_max) {
	double dim_boundaries[] = { dim1_min, dim1_max, dim2_min, dim2_max };
	return _SearchTree_Search(tree, itr_ptr, 2, dim_boundaries);
}

/*!
 * \brief Searches a 3D search tree and returns an iterator
 * \param[in] tree Handle to the 3D tree object
 * \param[in] itr_ptr Address of Iterator Handle
 * \param[in] dim1_min Lower boundary for search (dimension 1)
 * \param[in] dim1_max Higher boundary for search (dimension 1)
 * \param[in] dim2_min Lower boundary for search (dimension 2)
 * \param[in] dim2_max Higher boundary for search (dimension 2)
 * \param[in] dim3_min Lower boundary for search (dimension 3)
 * \param[in] dim3_max Higher boundary for search (dimension 3)
 *
 * This is a wrapper function to the internal _SearchTree_Search() routine.
 */
int MB_SearchTree_Search3D(MBt_SearchTree tree, MBt_Iterator *itr_ptr,
							double dim1_min, double dim1_max,
							double dim2_min, double dim2_max,
							double dim3_min, double dim3_max) {
	double dim_boundaries[] = { dim1_min, dim1_max,
								dim2_min, dim2_max,
								dim3_min, dim3_max};
	return _SearchTree_Search(tree, itr_ptr, 3, dim_boundaries);
}
