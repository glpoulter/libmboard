/* $Id$ */
/*!
 * \file parallel/env_init.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Mar 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * As the MB_Env_Init() is the routine that must be called in every
 * program that uses libmboard, we place the definitions of all
 * global variables (defined in mboard.h) in this file.
 * 
 * \brief Parallel implementation of MB_Env_Init()
 * 
 * \todo Call MPI_Init() on behalf of user code if necessary. We will need
 *       to run MPI_Finalize() during ::MB_Env_Finalise(). This will allow
 *       user code to be written without any trace of MPI, and have exactly
 *       then same codebase for both serial and parallel versions! We may need
 *       to provide additional routines like MB_Env_GetProcID() and 
 *       MB_Env_GetProcCount().
 */

#include "mb_parallel.h"
#include "mb_banner.h"
#include <time.h>

/* ---- Initialise global variables (Defined as extern in mb_common.h) ---- */
/*! \brief 'initialised' environment flag */
int MBI_STATUS_initialised   = MB_FALSE; 
/*! \brief 'finalised' environment flag */
int MBI_STATUS_finalised     = MB_FALSE; 
/*! \brief reference to MessageBoard ObjectMap */
MBIt_objmap *MBI_OM_mboard   = NULL;  
/*! \brief reference to Iterator ObjectMap */
MBIt_objmap *MBI_OM_iterator = NULL;   
/*! \brief reference to Function ObjectMap */
MBIt_objmap *MBI_OM_filter   = NULL;   
/*! \brief reference to IndexMap ObjectMap */
MBIt_objmap *MBI_OM_indexmap = NULL;   
/*! \brief reference to IndexMap ObjectMap */
MBIt_objmap *MBI_OM_searchtree = NULL;

/*! \brief Our world communicator for MPI Communication */
MPI_Comm MBI_CommWorld;
/*! \brief MPI Task ID */
int MBI_CommRank;
/*! \brief Number of MPI Tasks */
int MBI_CommSize;

/*! \brief string map to names used for creating index maps */
MBIt_stringmap *MBI_indexmap_nametable;

/*!
 * \brief Initialises the libmboard environment
 * \ingroup MB_API
 * 
 * ObjectMaps are instantiated and assigned to global reference.
 * 
 * The environment status flags are set, and the RNG seeded with the time.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_MEMALLOC (error allocating memory for ObjectMaps)
 *  - ::MB_ERR_ENV (libmboard environment already started)
 *  - ::MB_ERR_MPI (MPI Environment not yet initialised)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 * 
 */
int MB_Env_Init(void) {
    
    int flag;
    
    /* Check if environment already initialised */
    if (MBI_STATUS_initialised == MB_TRUE)
    {
    	P_FUNCFAIL("Message Board environment already initialised");
    	return MB_ERR_ENV;
    }
    
    /* Check if MPI environment has been initialised */
    MPI_Initialized(&flag);
    if (!flag)
    {
    	P_FUNCFAIL("MPI environment not initialised");
    	return MB_ERR_MPI;
    }
    
    /* Set up our communicator */
    MPI_Comm_dup(MPI_COMM_WORLD, &MBI_CommWorld);
    MPI_Comm_rank(MBI_CommWorld, &MBI_CommRank);
    MPI_Comm_size(MBI_CommWorld, &MBI_CommSize);
    
    /* print banner */
    #ifdef _EXTRA_CHECKS
    MBI_print_banner();
    #endif

    /* set and update (from env vars) library-wide settings */ 
    MBI_update_settings();
    
    /* seed rng during production runs */
    #ifndef _EXTRA_CHECKS
    srand((unsigned int)time(NULL)); 
    #endif
    
    #ifdef _LOG_MEMORY_USAGE
        memlog_init();
    #endif
    
    /* Allocate object maps */
    MBI_OM_mboard   = (MBIt_objmap*)MBI_objmap_new();
    MBI_OM_iterator = (MBIt_objmap*)MBI_objmap_new();
    MBI_OM_filter   = (MBIt_objmap*)MBI_objmap_new();
    MBI_OM_indexmap = (MBIt_objmap*)MBI_objmap_new();
    MBI_OM_searchtree = (MBIt_objmap*)MBI_objmap_new();
    if (!MBI_OM_mboard || !MBI_OM_iterator ||
    	!MBI_OM_filter || !MBI_OM_indexmap || !MBI_OM_searchtree)
    {   /* if allocation failed, release mem and return with error */
        MBI_objmap_destroy(&MBI_OM_mboard);
        MBI_objmap_destroy(&MBI_OM_iterator);
        MBI_objmap_destroy(&MBI_OM_filter);
        MBI_objmap_destroy(&MBI_OM_indexmap);
        MBI_objmap_destroy(&MBI_OM_searchtree);
        P_FUNCFAIL("Could not allocate required memory for Object Maps");
        return MB_ERR_MEMALLOC;
    }
    else
    {   /* set type */
        MBI_OM_mboard   ->type = OM_TYPE_MBOARD;
        MBI_OM_iterator ->type = OM_TYPE_ITERATOR;
        MBI_OM_filter   ->type = OM_TYPE_FILTER;
        MBI_OM_indexmap ->type = OM_TYPE_INDEXMAP;
        MBI_OM_searchtree->type = OM_TYPE_SEARCHTREE;
    }
    
    /* initialise Communication Thread */
    flag = MBI_CommThread_Init();
    assert(flag == MB_SUCCESS);
    if (flag != MB_SUCCESS) return flag;
    
    /* Allocate string map */
    MBI_indexmap_nametable = MBI_stringmap_Create();
    assert(MBI_indexmap_nametable != NULL);
    if (MBI_indexmap_nametable == NULL) 
    {
    	P_FUNCFAIL("Could not allocate String Map for indexmap name table");
    	return MB_ERR_INTERNAL;
    }
    
    P_INFO("Message Board environment initialised");
    
    /* set initialised status and return */
    MBI_STATUS_initialised = MB_TRUE;
    return MB_SUCCESS;
}
