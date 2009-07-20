/* $Id$ */
/*!
 * \file serial/env_init.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * As the MB_Env_Init() is the routine that must be called in every
 * program that uses libmboard, we place the definitions of all
 * global variables (defined in mboard.h) in this file.
 * 
 * \brief Serial implementation of MB_Env_Init()
 * 
 */

#include "mb_serial.h"
#include "mb_banner.h"
#include <time.h>

/* ---- Initialise global variables (Defined as extern in mb_common.h) ---- */
/*! \brief 'initialised' environment flag */
int MBI_STATUS_initialised = MB_FALSE; 
/*! \brief 'finalised' environment flag */
int MBI_STATUS_finalised   = MB_FALSE; 
/*! \brief reference to MessageBoard ObjectMap */
MBIt_objmap *MBI_OM_mboard   = NULL;  
/*! \brief reference to Iterator ObjectMap */
MBIt_objmap *MBI_OM_iterator = NULL;   
/*! \brief reference to Function ObjectMap */
MBIt_objmap *MBI_OM_filter = NULL;   
/*! \brief reference to IndexMap ObjectMap */
MBIt_objmap *MBI_OM_indexmap = NULL;   

/*! \brief Dummy MPI Task ID */
int MBI_CommRank;
/*! \brief Dummy Number of MPI Tasks */
int MBI_CommSize;

/*! \brief string map to names used for creating index maps */
MBIt_stringmap *MBI_indexmap_nametable;

/*!
 * \brief Initialises the libmboard environment
 * \ingroup MB_API
 * 
 * 
 * ObjectMaps needed to hash MessageBoards (::MBI_OM_mboard) and Iterators 
 * (::MBI_OM_iterator) are instantiated and assigned to the global reference.
 * 
 * The environment status flags are set, and the RNG seeded with the time.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_MEMALLOC (error allocating memory for ObjectMaps)
 *  - ::MB_ERR_ENV (libmboard environment already started)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 */
int MB_Env_Init(void) {
    
    /* Check if environment already initialised */
    if (MBI_STATUS_initialised == MB_TRUE) 
    {
        P_FUNCFAIL("Message Board environment already intialised");
        return MB_ERR_ENV;
    }
    
    /* set dummy values */
    MBI_CommSize = 1;
    MBI_CommRank = 0;
    
    /* print banner */
    MBI_print_banner();
    
    /* seed rng */
    #ifndef _EXTRA_CHECKS
    P_INFO("Seeding RNG");
    srand((unsigned)time(NULL)); 
    #endif
    
    #ifdef _LOG_MEMORY_USAGE
        memlog_init();
    #endif
    
    /* Allocate object maps */
    MBI_OM_mboard   = (MBIt_objmap*)MBI_objmap_new();
    MBI_OM_iterator = (MBIt_objmap*)MBI_objmap_new();
    MBI_OM_indexmap = (MBIt_objmap*)MBI_objmap_new();
    if (!MBI_OM_mboard || !MBI_OM_iterator || !MBI_OM_indexmap) 
    {   /* if allocation failed, release mem and return with error */
        MBI_objmap_destroy(&MBI_OM_mboard);
        MBI_objmap_destroy(&MBI_OM_iterator);
        MBI_objmap_destroy(&MBI_OM_indexmap);
        P_FUNCFAIL("Could not allocate required memory");
        return MB_ERR_MEMALLOC;
    }
    else
    {   /* set type */
        MBI_OM_mboard   ->type = OM_TYPE_MBOARD;
        MBI_OM_iterator ->type = OM_TYPE_ITERATOR;
        MBI_OM_indexmap ->type = OM_TYPE_INDEXMAP;
    }
    
    /* Allocate string map */
    MBI_indexmap_nametable = MBI_stringmap_Create();
    assert(MBI_indexmap_nametable != NULL);
    if (MBI_indexmap_nametable == NULL) 
    {
        P_FUNCFAIL("Could not initialise string map. Call to MBI_stringmap_Create() failed");
        return MB_ERR_INTERNAL;
    }
    
    /* set initialised status and return */
    P_INFO("Message Board environment initialised");
    MBI_STATUS_initialised = MB_TRUE;
    return MB_SUCCESS;
}
