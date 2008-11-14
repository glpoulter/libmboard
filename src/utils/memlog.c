/* $Id$ */
/*!
 * \file memlog.c
 * 
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Aug 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Routines for instrumenting memory usage
 * 
 * \todo use khash instead of uthash. Then we can remove uthash.h from 
 * the distribution.
 * 
 */

/* splint directive needed due to uthash implementation */
/*@+ignoresigns -unrecog@*/

/*! \brief Flag to ensure that memory operations are not overridden in 
 *         this file
 */
#define NO_OVERRIDE_MEM_ROUTINES

#ifdef HAVE_CONFIG_H
    #include "mb_config.h"
#endif

#ifdef _PARALLEL
    #include "mb_parallel.h"

    #ifdef HAVE_PTHREAD
        #include <pthread.h>
    #endif /* HAVE_PTHREAD */
    
#else /* _PARALLEL */
    #include "mb_serial.h"
#endif /* _PARALLEL */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mb_memlog.h"
#include "mb_common.h"
#include "uthash.h"
#include "sqlite3.h"

/*! \brief Maximum length of filenames */
#define MAX_FILENAME_LENGTH 128

/*! \brief SQL statements to initialise the database 
 * 
 * PRAGMA statements are used to greatly improve write performance (in exchange for
 * lower reliability, which is not necessary in our case).
 * 
 * CREATE statements are used to create database tables.
 */
#define SQL_CREATE_TABLE_MEM_USAGE "\
PRAGMA auto_vacuum = none; \
PRAGMA synchronous = OFF; \
PRAGMA temp_store = memory; \
PRAGMA journal_mode = OFF; \
PRAGMA locking_mode = EXCLUSIVE; \
\
CREATE TABLE 'mem_usage' (\
    ts real primary key, \
    allocated int \
);\
CREATE TABLE 'milestone' (\
    ts real primary key, \
    label varchar\
);"

/*! \brief SQL statement to write current memory usage + timestamp */
#define SQL_INSERT_MEM_STATUS "INSERT INTO mem_usage VALUES (?,?)"

/*! \brief SQL statement to write label + timestamp */
#define SQL_INSERT_MILESTONE "INSERT INTO milestone VALUES (?,?)"

/*! \brief Data structure for storing allocation size per pointer in Hash table */
struct memlog_ptrsizemap_t {
    /* \brief Pointer value. Used as hash key */
    void *ptr;
    /* \brief Size of allocated memory assigned to ptr */
    size_t size; 
    /*! \brief metadata required by \c uthash */
    UT_hash_handle hh;
};

/*! \brief Handle to sqlite3 database connection */
sqlite3 *memlog_db;

/*! \brief Handle to prepared SQL statements for inserting current memory usage */
sqlite3_stmt *stmt_insert;

/*! \brief Handle to prepared SQL statements for inserting labels */
sqlite3_stmt *stmt_milestone;

/*! \brief Macro to capture mutex lock */
#define __LOCK__

/*! \brief Macro to release mutex lock */
#define __UNLOCK__

#ifdef _PARALLEL
    #ifdef HAVE_PTHREAD
        /*! \brief Mutex lock object */
        pthread_mutex_t memlog_lock;
        #undef __LOCK__
        #define __LOCK__ pthread_mutex_lock(&memlog_lock)
        #undef __UNLOCK__
        #define __UNLOCK__ pthread_mutex_unlock(&memlog_lock)
    #endif
#endif
        
/*! \brief Total number of bytes "lost" 
 * 
 * This happens when memory is freed outside our scope, e.g. when user code
 * does not include mb_memlog.h, or when they are not compiled using the
 * -D_LOG_MEMORY_USAGE flag. 
 */
int lost;

/*! \brief Total number of bytes allocated */
int allocated;

/*! \brief Timestamp at initialisation
 * 
 * This value is deducted from later timestamps to derive elapsed tie
 */
double init_ts;

/*! \brief String to store filename */
char memlog_filename[MAX_FILENAME_LENGTH];

/*! \brief Hash table to store map of allocated memory + size */
struct memlog_ptrsizemap_t *sizemap;

/*!
 * \brief Returns current timestamp
 * \return double representing current timestamp in seconds
 * 
 * Deduct the return value with ::init_ts to obtain elapsed time.
 */
static double __get_ts(void) {

    struct timeval now; 
    gettimeofday(&now, NULL);
        
    /* return time in secs */
    return now.tv_sec + (now.tv_usec * 1.e-6);

}

/*!
 * \brief Writes current memory usage to database
 * 
 * Elapsed time (__get_ts() - ::init_ts) and ::allocated written to database.
 */
static void __record_memory_usage(void) {
    int rc;
    
    rc = sqlite3_bind_double(stmt_insert, 1, (__get_ts() - init_ts));
    assert(rc == SQLITE_OK);
    rc = sqlite3_bind_int(stmt_insert, 2, allocated);
    assert(rc == SQLITE_OK);
    
    rc = sqlite3_step(stmt_insert);
    assert(rc == SQLITE_DONE);
    
    sqlite3_reset(stmt_insert);

}


/*!
 * \brief Stores size of allocated memory assigned to a pointer
 * \param[in] ptr Pointer
 * \param[in] size Size of allocated memory
 * \return Integer value of allocated memory size 
 * 
 * ptr-size pair is store in the ::sizemap hash table.
 * 
 * If value of \c ptr already exists in the hash table, remove it and 
 * increment the ::lost variable by the previous size. Users must have
 * previously freed the memory in a code beyond our scope. Nothing we 
 * can do about that except print a notice later on.
 *  
 * If \c ptr is \c NULL, or if \c size is 0, do nothing except return 0.
 * 
 * The value of size is also returned to users so the function call can be
 * chained to an variable assignment.
 */
/* remember memory size allocated to a pointer */
static int __sizemap_add(void *ptr, size_t size) {
    
    struct memlog_ptrsizemap_t *new;
    struct memlog_ptrsizemap_t *old;
    
    /* don't add if empty ptr or 0 sized */
    if (size == 0 || ptr == NULL) return 0;

    HASH_FIND(hh, sizemap, &ptr, sizeof(void *), old);
    if (old != NULL)
    {
        /* note: this isn't really a memory leak. Just the fact that
         * the pointer in question was freed in a code beyond our reach
         * and which we therefore have no record off.
         */ 
        lost += (int)old->size;
        HASH_DEL(sizemap, old);
        free(old);
    }
    
    new = (struct memlog_ptrsizemap_t*)malloc(sizeof(struct memlog_ptrsizemap_t));
    new->ptr  = ptr;
    new->size = size;

    HASH_ADD(hh, sizemap, ptr, sizeof(void *), new);
    return (int)size;
}

/*!
 * \brief Remove hash table entry for pointer
 * \param[in] ptr Pointer
 * \return Integer value of size of memory allocated to ptr 
 * 
 * \c ptr entry is  deleted from the ::sizemap hash table. The corresponding
 * size assigned to \c ptr is returned.
 * 
 * If value of \c ptr does not exists in the hash table, do nothing 
 * except return 0.
 * 
 */
/* forget memory size allocated to a pointer */
static int __sizemap_del(void *ptr) {

    struct memlog_ptrsizemap_t *old;
    size_t size = 0;

    /* first get pointer to struct so we can deallocate mem
     * since HASH_DELETE does not free the struct
     */
    HASH_FIND(hh, sizemap, &ptr, sizeof(void *), old);

    if (old != NULL) /* if found */
    {
        size = old->size;
        HASH_DEL(sizemap, old);
        free(old);
    }
    return (int)size;
}

/*!
 * \brief Delete sizemap hash table
 * \return Integer value of the sum of sizes assigned to pointers still 
 * referenced to by the hash table
 * 
 * The ::sizemap is emptied. Nodes still present in the hash table are
 * freed, and the sum of \c size is accumulated and returned. 
 * 
 * If the hash table is empty, do nothing except return 0.
 */
/* free all internal data and return size of unallocated memory */
static int __sizemap_flush(void) {
    size_t remainder = 0;
    struct memlog_ptrsizemap_t *node;
    
    /* iterate map to sum unallocated memory and free data*/
    while(sizemap)
    {
        node = sizemap;
        sizemap = sizemap->hh.next;
        
        remainder += node->size;
        free(node);
    }
    
    return (int)remainder;
}

/*!
 * \brief Writes a label + timestamp to database
 * \param[in] label Text string of label
 * 
 * Associates a string label to a specific elapsed time by writing a 
 * database entry. This will allow the output memory usage charts to be 
 * annotated with labels (to denote stages in a simulation).
 * 
 */
void memlog_milestone(const char *label) {
    int rc;
    
    rc = sqlite3_bind_double(stmt_milestone, 1, __get_ts() - init_ts);
    assert(rc == SQLITE_OK);
    rc = sqlite3_bind_text(stmt_milestone, 2, label, -1, NULL);
    assert(rc == SQLITE_OK);
    
    rc = sqlite3_step(stmt_milestone);
    assert(rc == SQLITE_DONE);
    
    sqlite3_reset(stmt_milestone);
}

/*!
 * \brief Initialises memlog
 * 
 * Creates the sqlite3 database file and initialises necessary variables,
 * sqlite prepared statements, mutex locks, etc.
 * 
 * Must be called before any memlog routines are used.
 */
void memlog_init(void) {
        
    int rc;
    char *err_msg, *sql;
    
    /* print BANNER notifying user of memlog inclusion */
    if (MBI_CommRank == 0)
    {
        printf("=============================================================\n");
        printf("                libmboard memory instrumentation             \n");
        printf("-------------------------------------------------------------\n");
        printf(" The message board library (libmboard) used  by this program \n");
        printf(" was compiled with memory instrumentation enabled.           \n\n");
        
        printf(" This instrumentation  is useful  for  exploring  the memory \n");
        printf(" requirements  and  usage  of  your program, but do be aware \n");
        printf(" that it may have a signifcation impact on performance.      \n\n");
        
        printf(" Please  include  -D_LOG_MEMORY_USAGE  in  your  CFLAGS  and \n");
        printf(" ensure that mboard.h is included in all your source files.  \n\n");
        
        printf(" You  may  also  label  stages  of  you simulation using the \n");
        printf(" 'memlog_milestone' routine within your code.                \n");
        printf(" For example:                                                \n");
        printf(" memlog_milestone(\"I\"); /* mark start of new iteration */  \n\n");
        
        printf(" Memory usage logs will be written to memlog-*.txt           \n");
        printf(" These files  can be converted to gnuplot  scripts and  data \n");
        printf(" files using the mboard-parse-memlogdb program included with \n");
        printf(" the installation.                                           \n");
        printf("-------------------------------------------------------------\n\n");
    }
    
    /* initialise memory counters */
    lost = 0;
    allocated = 0;
    init_ts   = __get_ts();
    
    /* initialise ptr-size hash table */
    sizemap = NULL;
    
    /* determine output filename */
    snprintf(memlog_filename, MAX_FILENAME_LENGTH, "memlog-%d_of_%d.db", 
             MBI_CommRank, MBI_CommSize - 1);
    
    /* delete old db file if exist */
    remove(memlog_filename);
    
    /* create new DB */
    memlog_db = NULL;
    rc = sqlite3_open_v2(memlog_filename, &memlog_db, 
            SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, NULL);
    assert(rc == SQLITE_OK);
    assert(NULL != memlog_db);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "MEMLOG ERROR: Could not open %s for writing.\n", memlog_filename);
        #ifdef _PARALLEL
                MPI_Abort(MBI_CommWorld, 1);
        #else
                exit(1);
        #endif
    }
    
    /* create mem_usage table */
    sql = sqlite3_mprintf(SQL_CREATE_TABLE_MEM_USAGE);
    rc = sqlite3_exec(memlog_db, sql, NULL, NULL, &err_msg);
    sqlite3_free(sql);
    assert(rc == SQLITE_OK);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "MEMLOG ERROR: Could not create table (%s)\n", err_msg);
        sqlite3_free(err_msg);
#ifdef _PARALLEL
        MPI_Abort(MBI_CommWorld, 1);
#else  /* _PARALLEL */
        exit(1);
#endif /* _PARALLEL */
    }
    
    /* prepare mem_usage insert statement */
    rc = sqlite3_prepare_v2(memlog_db, SQL_INSERT_MEM_STATUS, -1, &stmt_insert, NULL);
    assert(rc == SQLITE_OK);
    assert(stmt_insert != NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "MEMLOG ERROR: Could not prepare SQL statement for INSERT.\n");
        sqlite3_close(memlog_db);
        #ifdef _PARALLEL
                MPI_Abort(MBI_CommWorld, 1);
        #else
                exit(1);
        #endif
    }
    
    /* prepare milestone insert statement */
    rc = sqlite3_prepare_v2(memlog_db, SQL_INSERT_MILESTONE, -1, &stmt_milestone, NULL);
    assert(rc == SQLITE_OK);
    assert(stmt_milestone != NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "MEMLOG ERROR: Could not prepare SQL statement for INSERT.\n");
        sqlite3_close(memlog_db);
        #ifdef _PARALLEL
                MPI_Abort(MBI_CommWorld, 1);
        #else
                exit(1);
        #endif
    }
    
    /* issue first data entry to denote start time (and test writing to db) */
    __record_memory_usage();
    
    /* initialise lock */
    #ifdef _PARALLEL
        #ifdef HAVE_PTHREAD
            rc = pthread_mutex_init(&memlog_lock, NULL);
            assert(0 == rc);
            if (0 != rc)
            {
                fprintf(stderr, "MEMLOG ERROR: Could not initialise mutex lock\n");
                MPI_Abort(MBI_CommWorld, 1);
            }
        #endif /* HAVE_PTHREAD */
    #endif /* _PARALLEL_ */
}

/*!
 * \brief Finalises memlog
 * 
 * Cleans up by closing the sqlite3 database file and deallocating/finalising
 * sqlite prepared statements, mutex locks, etc.
 * 
 * Must be the last memlog routine called.
 */
void memlog_finalise(void) {
    
    int remainder;
    
    /* memory that remains allocated is considered to be lost */
    /* lost -= __sizemap_flush(); */
    remainder = __sizemap_flush();
    __record_memory_usage();
    
    /* finalise sql statements */
    sqlite3_finalize(stmt_insert);
    sqlite3_finalize(stmt_milestone);
    
    /* close database */
    assert(memlog_db != NULL);
    sqlite3_close(memlog_db);
    
    #ifdef _PARALLEL
        #ifdef HAVE_PTHREAD
            /* destroy mutex obj */
            pthread_mutex_destroy(&memlog_lock);
        #endif /* HAVE_PTHREAD */
    #endif /* _PARALLEL */
            
            
    printf("=============================================================\n");
    printf("                libmboard memory instrumentation             \n");
    printf("-------------------------------------------------------------\n");
    printf("--> [%d] MEMLOG output written to %s\n\n", MBI_CommRank, memlog_filename);
    
    /* have there been memory operations outside our view? */
    if (lost != 0 || remainder != 0)
    {
        printf("\
!!! [%d] MEMLOG: If your program was not compiled with -D_LOG_MEMORY_USAGE,\n\
                memory operations performed within your own code would not \n\
                have been included in the memory usage estimates.\n\
                Please add -D_LOG_MEMORY_USAGE to your CFLAGS, and include\n\
                mboard.h in all your source files.\n\n",  
                MBI_CommRank);
    }
    
    printf("You can now compile your results by running:              \n");
    printf(" \"${LIBMBOARD_INSTALL_DIR}/bin/mboard-parse-memlogdb %d\"\n", MBI_CommSize);
    printf("-------------------------------------------------------------\n\n");
}

/*!
 * \brief Replacement for free()
 * \param[in] ptr Pointer to free
 * 
 * Removes \c ptr entry from ::sizemap, decrements ::allocated, records 
 * current memory usage in db, then calls the real free() on behalf of user.
 */
void memlog_free(void *ptr) {
    
    __LOCK__; /* capture lock */
    allocated -= __sizemap_del(ptr);
    __record_memory_usage();
    __UNLOCK__; /* release lock */
    
    free(ptr);
}

/*!
 * \brief Replacement for calloc()
 * \param[in] nmemb Number of elements
 * \param[in] size Size of each element
 * \return Pointer to allocated memory
 * 
 * Calls the real calloc() on behalf of user.
 * 
 * If return pointer is not \c NULL, adds \c ptr entry to ::sizemap, 
 * increment ::allocated, and record current memory usage in db. 
 */
void *memlog_calloc(size_t nmemb, size_t size) {
    
    void * ret;
    
    ret = calloc(nmemb, size);
    
    if (ret != NULL)
    {
        __LOCK__; /* capture lock */
        
        /* increment counter */
        allocated += __sizemap_add(ret, (size_t)(size * nmemb));
        
        /* write DB entry */
        __record_memory_usage();
        
        __UNLOCK__; /* release lock */
    }
    
    return ret;
}

/*!
 * \brief Replacement for malloc()
 * \param[in] size Size of memory required
 * \return Pointer to allocated memory
 * 
 * Calls the real malloc() on behalf of user.
 * 
 * If return pointer is not \c NULL, adds \c ptr entry to ::sizemap, 
 * increment ::allocated, and record current memory usage in db. 
 */
void *memlog_malloc(size_t size) {
    
    void * ret;
    ret = malloc(size);
    
    if (ret != NULL)
    {
        __LOCK__;/* capture lock */
        
        /* increment counter */
        allocated += __sizemap_add(ret, size);
        
        /* write DB entry */
        __record_memory_usage();
        
        __UNLOCK__; /* release lock */
    }
    
    return ret;
}

/*!
 * \brief Replacement for malloc()
 * \param[in] ptr Pointer to currenly allocated memory
 * \param[in] size Size of memory required
 * \return Pointer to newly allocated memory
 * 
 * Calls the real realloc() on behalf of user.
 * 
 * If return pointer is not \c NULL, delete the old \c ptr entry from ::sizemap
 * and add the newly returned pointer to ::sizemap.
 *  
 * The old value of \c size is decremented from ::allocated, the new value is
 * added to ::allocated. Then, record current memory usage in db. 
 */
void *memlog_realloc(void *ptr, size_t size) {
    
    void * ret;
    
    ret = realloc(ptr, size);
    if (ret != NULL)
    {
        __LOCK__; /* capture lock */
        
        /* decrement previous value */
        allocated -= __sizemap_del(ptr);
        
        /* increment counter */
        allocated += __sizemap_add(ret, size);
        
        /* write DB entry */
        __record_memory_usage();
        
        __UNLOCK__; /* release lock */
    }
    return ret;
}
