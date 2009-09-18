/* $Id$ */
/*!
 * \file parallel/indexmap_sync.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : June 2009 
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Parallel implementation of MB_IndexMap_Sync()
 * 
 */

#include "mb_parallel.h"
#include "mb_objmap.h"

#ifdef _EXTRA_CHECKS
#include "mb_commqueue.h"
#include "mb_syncqueue.h"

static void _check_map_equal(OM_key_t key);
#endif

static int _assign_bitboard_to_nodes(MBIt_AVLnode *node);
inline static void _get_bitdata(int pid, int *byte_offset, char *mask);
static void _update_tree_with_new_data(MBIt_AVLtree *tree, int pid, int *databuf, int count);

/*!
 * \brief Distributes/gathers the map content across/from all processors
 * \ingroup MB_API
 * \param[in] im Index Map handle
 * 
 * This routine calls MPI Routines, and should therefore never be called
 * when there are any Message Board syncs in progress as this will cause
 * problems with MPI Libraries that do not support full threading.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c im is null or invalid)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 *  - ::MB_ERR_MPI (Error returned from MPI calls, possibly a bug)
 */
int MB_IndexMap_Sync(MBt_IndexMap im) {
    
    int i, rc;
    int treecount;
    int pending_in;
    MBIt_IndexMap *im_obj;
    int *outcount, *count;
    MPI_Request *sendreq, *recvreq;
    int *outbuf, **inbuf;
    
    assert(MBI_CommQueue_isEmpty() && MBI_SyncQueue_isEmpty());
    
    /* check for null boards */
    if (im == MB_NULL_INDEXMAP)
    {
        P_FUNCFAIL("Cannot sync a null map (MB_NULL_INDEXMAP)");
        return MB_ERR_INVALID;
    }
    
    /* get reference to indexmap object */
    im_obj = (MBIt_IndexMap *)MBI_getIndexMapRef(im);
    if (im_obj == NULL) 
    {
        P_FUNCFAIL("Invalid map handle (%d)", (int)im);
        return MB_ERR_INVALID;
    }
    #ifdef _EXTRA_CHECKS
    if (im_obj->synced == MB_TRUE) 
    {
        assert(im_obj->tree != NULL);
    }
    #endif
    
    /* make sure local tree is intact. This will be tripped if users
     * define their own object instead of using _Create()
     */
    assert(im_obj->tree_local != NULL);
    if (im_obj->tree_local == NULL)
    {
        P_FUNCFAIL("Corrupted map object");
        return MB_ERR_INVALID;
    }
    
    /* -------- Check that we're syncing the same map ------ */
#ifdef _EXTRA_CHECKS
    _check_map_equal((OM_key_t)im);
#endif 
    
    P_INFO("Synching Index Map (%d) '%s' ", (int)im, im_obj->name);
    
    /* -------- Allocate required memory ---------------- */

    /* allocate memory for i/o counters */
    outcount = (int *)malloc(sizeof(int) * MBI_CommSize);
    assert(outcount != NULL);
    count = (int *)malloc(sizeof(int) * MBI_CommSize);
    assert(count != NULL);
    
    /* allocate memory for incoming bufferS */
    inbuf = (int **)malloc(sizeof(int*) * MBI_CommSize);
    assert(inbuf != NULL);
    
    /* allocate memory for mpi request objects */
    sendreq = (MPI_Request *)malloc(sizeof(MPI_Request) * MBI_CommSize);
    assert(sendreq != NULL);
    recvreq = (MPI_Request *)malloc(sizeof(MPI_Request) * MBI_CommSize);
    assert(recvreq != NULL);

    /* if any of the memory allocations failed, clean up and quit */
    if (!outcount || !count ||  !inbuf || !sendreq || !recvreq)
    {
        if (outcount) free(outcount);
        if (count) free(count);
        if (inbuf) free(inbuf);
        if (sendreq) free(sendreq);
        if (recvreq) free(recvreq);
        
        P_FUNCFAIL("Could not allocate required memory");
        return MB_ERR_MEMALLOC;
    }
    
    /* -------- Let all procs know what to expect ---------------- */
    
    /* get dump of avl tree content */
    rc = MBI_AVLtree_dump(im_obj->tree_local, &outbuf, &treecount);
    if (rc != AVL_SUCCESS)
    {
        if (rc == AVL_ERR_MEMALLOC) 
        {
            P_FUNCFAIL("Could not allocate required memory");
            return MB_ERR_MEMALLOC;
        }
        else 
        {
            P_FUNCFAIL("MBI_AVLtree_dump() return with err code %d", rc);
            return MB_ERR_INTERNAL;
        }
    }
    #ifdef _EXTRA_CHECKS
    if (treecount != 0) 
    {
        assert(outbuf != NULL);
    }
    #endif
    
    /* assign outgoing count */
    for (i = 0; i < MBI_CommSize; i++) outcount[i] = treecount;
    outcount[MBI_CommRank] = 0;
    
    /* distribute/gather to/from all procs */
    rc = MPI_Alltoall(outcount, 1, MPI_INT, count, 1, MPI_INT, MBI_CommWorld);
    assert(rc == MPI_SUCCESS);
    if (rc != MPI_SUCCESS)
    {
        P_FUNCFAIL("MPI_Alltoall() return with err code %d", rc);
        return MB_ERR_MPI;
    }
    /* we no longer need outcount */
    free(outcount);
    
    /* -------------- do recv --------------------------------------- */
    
    pending_in = 0;
    for (i = 0; i < MBI_CommSize; i++)
    {
        /* if we're not expecting anything for proc i */
        if (count[i] == 0 || i == MBI_CommRank)
        {
            recvreq[i] = MPI_REQUEST_NULL;
            inbuf[i]   = NULL;
            continue;
        }
        
        /* allocate buffer */
        inbuf[i] = (int*)malloc(sizeof(int) * count[i]);
        assert(inbuf[i] != NULL);
        /* returning ERR on failure here may hang MPI procs. rely on assert */
        
        /* issue non-blocking receive */
        rc = MPI_Irecv(inbuf[i], count[i], MPI_INT, i, MBI_TAG_INDEXMAX_SYNC,\
                MBI_CommWorld, &(recvreq[i]));
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) 
        {
            P_FUNCFAIL("MPI_Irecv() returned with err code %d", rc);
            return MB_ERR_MPI;
        }
        
        /* update counter */
        pending_in++;
    }
    
    /* --------------- do sends ---------------------------------- */
    
    /* issue non-blocking sends to all proc (except self) */
    for (i = 0; i < MBI_CommSize; i++)
    {
        if (i == MBI_CommRank || treecount == 0)
        {
            sendreq[i] = MPI_REQUEST_NULL;
            continue;
        }
        
        rc = MPI_Issend(outbuf, treecount, MPI_INT, i, MBI_TAG_INDEXMAX_SYNC,\
                MBI_CommWorld, &(sendreq[i]));
        assert(rc == MPI_SUCCESS);
        if (rc != MPI_SUCCESS) 
        {
            P_FUNCFAIL("MPI_Issend() returned with err code %d", rc);
            return MB_ERR_MPI;
        }
    }
    
    /* -------------- move tree_local data into main tree ------------- */
    
    if (im_obj->tree == NULL) /* first sync. recycle tree */ 
    { 
        assert(im_obj->synced == MB_FALSE);
        
        /* recycle tree structure and nodes from tree_local */
        im_obj->tree = im_obj->tree_local; /* take over tree */
        
        /* tree_local gets a brand new tree */
        im_obj->tree_local = MBI_AVLtree_create(); 
        assert(im_obj->tree_local != NULL);
        /* returning ERR on failure here may hang MPI procs. rely on assert */
        
        /* walk tree and assign bitboard to data */
        rc = MBI_AVLtree_walk(im_obj->tree, &_assign_bitboard_to_nodes);
        assert(rc == AVL_SUCCESS);
        /* returning ERR on failure here may hang MPI procs. rely on assert */
        
    }
    else /* this map has been synced before. Update */
    {
        assert(im_obj->synced == MB_TRUE);
        
        /* destroy local tree (data already in outbuf */
        MBI_AVLtree_destroy(&(im_obj->tree_local));
        assert(im_obj->tree_local == NULL);
        
        /* create empty one */
        im_obj->tree_local = MBI_AVLtree_create();
        assert(im_obj->tree_local != NULL);
        
        /* update main tree */
        _update_tree_with_new_data(im_obj->tree, MBI_CommRank, outbuf, treecount);

    }
    
    /* ------------- wait for recv to complete, then handle data ------ */
    
    while(pending_in > 0)
    {
        /* wait for any recv to complete */
        rc = MPI_Waitany(MBI_CommSize, recvreq, &i, MPI_STATUS_IGNORE);
        assert(rc == MPI_SUCCESS);

        /* sanity checks */
        assert(recvreq[i] == MPI_REQUEST_NULL);
        assert(i != MPI_UNDEFINED);
        assert(inbuf[i] != NULL);
        
        /* decrement counter */
        pending_in--;
        
        /* update tree with received data */
        _update_tree_with_new_data(im_obj->tree, i, inbuf[i], count[i]);
        
        /* free allocated buffer */
        free(inbuf[i]);
        inbuf[i] = NULL;
    }
    
    /* free buffers */
    free(inbuf);
    free(recvreq);
    
    /* ----------- wait for sends to complete ------------------------- */
    
    rc = MPI_Waitall(MBI_CommSize, sendreq, MPI_STATUSES_IGNORE);
    assert(rc == MPI_SUCCESS);
    
    /* -------------- clean up and finish ----------------------------- */
    
    /* free all buffers */
    free(count);
    free(outbuf);
    free(sendreq);
    
    /* set synced flag */
    im_obj->synced = MB_TRUE;
    
    return MB_SUCCESS;
}

static void _update_tree_with_new_data(MBIt_AVLtree *tree, int pid, int *databuf, int count) {
    
    int i, rc, val;
    tag_table *bitboard;
    int byte_offset;
    char mask;
    
    assert(databuf != NULL);
    assert(tree != NULL);
    
    for (i = 0; i < count; i++)
    {
        val = databuf[i];
        
        _get_bitdata(pid, &byte_offset, &mask);
        bitboard = MBI_AVLtree_getdata(tree, val);
        if (bitboard == NULL) /* node does not exist yet */
        {
            /* create new bitboard. Use tag_table with single row */
            rc = tt_create(&bitboard, 1, MBI_CommSize);
            assert(rc == TT_SUCCESS);
            assert(bitboard != NULL);
            
            /* set bit in board */
            rc = tt_setbyte(bitboard, 0, byte_offset, mask);
            assert(rc == TT_SUCCESS);
            
            /* create new node and attach bitboard */
            rc = MBI_AVLtree_insert(tree, val, (void*)bitboard);
            assert(rc == AVL_SUCCESS);
        } 
        else /* node exist, just update data */
        {
            /* use setbits instead of setbyte so we don't overwrite 
             * previous values. setbits does an OR on the data
             */
            rc = tt_setbits(bitboard, 0, byte_offset, mask);
            assert(rc == TT_SUCCESS);
        }
    }
}

static int _assign_bitboard_to_nodes(MBIt_AVLnode *node) {
    
    int rc;
    int byte_offset;
    char mask;
    tag_table *bitboard;
    
    assert(node != NULL);
    assert(node->data == NULL);
    
    /* use a single row tag table as bitboard */
    rc = tt_create(&bitboard, 1, MBI_CommSize);
    assert(rc == TT_SUCCESS);
    if (rc != TT_SUCCESS) 
    {
        if (rc == TT_ERR_MEMALLOC) return AVL_ERR_MEMALLOC;
        else return AVL_ERR_INTERNAL;
    }
    
    /* set bit for local proc */
    _get_bitdata(MBI_CommRank, &byte_offset, &mask);
    rc = tt_setbyte(bitboard, 0, byte_offset, mask);
    assert(rc == TT_SUCCESS);
    
    /* assign bitboard to node data */
    node->data = (void *)bitboard;
    
    return 0;
}

inline static void _get_bitdata(int pid, int *byte_offset, char *mask) {

    assert(byte_offset != NULL);
    assert(mask != NULL);
    assert(pid < MBI_CommSize);
    
    *byte_offset = pid / 8;
    *mask = 0x80 >> (pid % 8);
}

#ifdef _EXTRA_CHECKS
static void _check_map_equal(OM_key_t key) {
    
    int rc;
    OM_key_t mkey;
    if (MASTERNODE) mkey = key;
    
    /* master broadcast key to all other procs */
    rc = MPI_Bcast(&mkey, (int)sizeof(OM_key_t), MPI_BYTE, 0, MBI_CommWorld);
    
    #ifdef _PRINT_WARNINGS
    if (rc != MPI_SUCCESS)
    {
        P_WARNING("MPI_Bcast() returned with err code %d", rc);
    }
    if (key != mkey)
    {
        P_WARNING("IndexMap sync being done out of order (master:%d, mine:%d)", 
                (int)mkey, (int)key);
    }
    #endif
    
    assert(rc == MPI_SUCCESS);
    assert(key == mkey); /* compare master key with our key */
}
#endif
