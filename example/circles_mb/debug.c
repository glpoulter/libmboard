/* $Id$ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : debug.c
 *  
 */

#include <assert.h>
#include <string.h>
#include "mboard.h"
#include "mb_common.h"
#include "mb_utils.h"
#include "pooled_list.h"
#include "circles.h"

#ifdef _PARALLEL
    #include "mb_parallel.h"
#else
    #include "mb_serial.h"
#endif

static void dump_board_data(void);
static void dump_iterator_data(void);

void PRINT_DEBUG_INFO(void) {
    PRINT("++++++++++++++ DEBUG INFO +++++++++++++++++++++\n\n");
    dump_board_data();
    dump_iterator_data();
    PRINT(".............. END DEBUG INFO .................\n\n");
}


static void dump_board_data(void) {
    
    pooled_list *pl;
    pl_iterator *pl_itr;
    struct m_Force *mforce;
    struct m_Location *mloc;
    MBIt_Board *bforce, *bloc;
    
    /* location message */
    if (b_Location == MB_NULL_MBOARD)
    {
        PRINT("Location Board UNINITIALISED!!!\n");
    }
    else
    {
        PRINT("Location Board Content:\n");
        bloc = (MBIt_Board*)MBI_getMBoardRef(b_Location);
        pl = bloc->data;
        assert(pl != NULL);
        PRINT(" * size = %d\n", pl->count_current);
        if (0 != pl->count_current) PRINT(" * content:\n");
        for (pl_itr = PL_ITERATOR(pl); pl_itr; pl_itr = pl_itr->next)
        {
            mloc = (struct m_Location*)PL_NODEDATA(pl_itr);
            PRINT("   (%d, %f, %f)\n", mloc->id, mloc->x, mloc->y);
        }
    }
    PRINT("\n");
    
    /* force message */
    if (b_Force == MB_NULL_MBOARD)
    {
        PRINT("Force Board UNINITIALISED!!!\n");
    }
    else
    {
        PRINT("Force Board Content:\n");
        bforce = (MBIt_Board*)MBI_getMBoardRef(b_Force);
        pl = bforce->data;
        assert(pl != NULL);
        PRINT(" * size = %d\n", pl->count_current);
        if (0 != pl->count_current) PRINT(" * content:\n");
        for (pl_itr = PL_ITERATOR(pl); pl_itr; pl_itr = pl_itr->next)
        {
            mforce = (struct m_Force*)PL_NODEDATA(pl_itr);
            PRINT("   (%d, %f, %f)\n", mforce->id, mforce->fx, mforce->fy);
        }
    }
    PRINT("\n");
}

static void dump_iterator_data(void) {
    
    pooled_list *pl;
    pl_iterator *pl_itr;
    struct m_Force *mforce  = NULL;
    struct m_Location *mloc = NULL;
    void *obj;
    MBIt_Iterator *iforce, *iloc;
    
    /* location iterator */
    if (i_Location == MB_NULL_ITERATOR)
    {
        PRINT("Location Iterator UNINITIALISED!!!\n");
    }
    else
    {
        PRINT("Location Itearator Content:\n");
        iloc = (MBIt_Iterator*)MBI_getIteratorRef(i_Location);
        pl = iloc->data;
        assert(pl != NULL);
        PRINT(" * size = %d\n", pl->count_current);
        if (0 != pl->count_current) PRINT(" * content:\n");
        for (pl_itr = PL_ITERATOR(pl); pl_itr; pl_itr = pl_itr->next)
        {
            obj = PL_NODEDATA(pl_itr);
            memcpy(mloc, obj, sizeof(void*));
            PRINT("   (%d, %f, %f)\n", mloc->id, mloc->x, mloc->y);
        }
    }
    PRINT("\n");
    
    /* Force iterator */
    if (i_Force == MB_NULL_ITERATOR)
    {
        PRINT("Force Iterator UNINITIALISED!!!\n");
    }
    else
    {
        PRINT("Force Itearator Content:\n");
        iforce = (MBIt_Iterator*)MBI_getIteratorRef(i_Force);
        pl = iforce->data;
        assert(pl != NULL);
        PRINT(" * size = %d\n", pl->count_current);
        if (0 != pl->count_current) PRINT(" * content:\n");
        for (pl_itr = PL_ITERATOR(pl); pl_itr; pl_itr = pl_itr->next)
        {
            obj = PL_NODEDATA(pl_itr);
            memcpy(mforce, obj, sizeof(void*));
            PRINT("   (%d, %f, %f)\n", mforce->id, mforce->fx, mforce->fy);
        }
    }
    PRINT("\n");
}
