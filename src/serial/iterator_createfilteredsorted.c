/* $Id$ */
/*!
 * \file serial/iterator_createfilteredsorted.c
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Feb 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Serial implementation of MB_Iterator_CreateFilteredSorted()
 * 
 */


#include "mb_serial.h"
#include <string.h> /* for memcpy() */

/* function ptr */
static int (*funcPtr)(const void *msg1, const void *msg2);

/* sort routine that dereferencs pointers to pass on to softFunc */
static int ptrSort(const void *p1, const void *p2);

/* routine to create array of message pointes (sorted by sortFunc) */
static void ** get_sorted_filtered_ptr_list(MBt_Board mb, int mcount, \
        int (*cmpFunc)(const void *msg1, const void *msg2), \
        int (*filterFunc)(const void *msg, const void *params), \
        void *filterFuncParams, int *elemOut);

/*!
 * \brief Creates a filtered and sorted Iterator for accessing a selection of 
 *        messages in the MessageBoard
 * \ingroup MB_API
 * \param[in] mb MessageBoard handle
 * \param[out] itr_ptr Address of Iterator Handle
 * \param[in] filterFunc Pointer to user-defined filter function
 * \param[in] filterFuncParams Pointer to input data that will be passed into \c filterFunc 
 * \param[in] cmpFunc Pointer to user-defined compariosn function
 * 
 * 
 * The Iterator object is allocated and populated with messages from an array 
 * returned by the \c get_sorted_filtered_ptr_list() (static function defined 
 * in iterator_createfilteredsorted.c). The Iterator is then registered 
 * with the ::MBI_OM_iterator and the reference ID returned by ObjectMap 
 * is then written to \c itr_ptr as the handle.
 * 
 * \c get_sorted_filtered_ptr_list() populates a message array by traversing 
 * the MessageBoard and selecting messages that are accepted by \c filterFunc.
 * It then uses \c cmpFunc with \c qsort() (from \c stdlib.h) for sorting before 
 * returning a pointer to the message array. 
 * 
 * The use of \c qsort() got a little 
 * messy as we need to sort the list based on the \em value of the messages whereas
 * the array stores only \em pointers to those messages. For now, \c qsort() is given 
 * \c ptrSort() -- a static function which dereferences the message pointers before
 * calling \c cmpFunc. The messy part is that  \c cmpFunc cannot be passed to 
 * \c ptrSort() as an argument (for compatibily with \c qsort()) so it has to be
 * handed over using a global variable (\c funcPtr). This global variable makes
 * our routine non thread-safe.
 * 
 *  
 * We expect \c filterFunc() to return a \c 0 if a message is to be rejected,
 * and a non-zero \c int if it is to be accepted.
 * 
 * We expect \c cmpFunc() to return an integer 
 * less than, equal to, or greater than zero if the first message is 
 * considered to be respectively less than, equal to, or greater than the 
 * second. In short:
 *  - <tt>0 if (msg1 == msg2)</tt>
 *  - <tt>\< 0 if (msg1 \< msg2)</tt>
 *  - <tt>\> 0 if (msg1 \> msg2)</tt>
 * 
 * References to messages are stored within a pooled_list and traversed as 
 * a linked list. The memory block size for the Iterator pooled_list is chosen to
 * be half that of the default MessageBoard block size.
 * 
 * Message references are stored in the same order they appear in the board.
 * 
 * \note We only store pointers to message objects within the Iterator, and
 *       not the actual messages. These pointers will be invalid if the 
 *       MessageBoard is deleted of modified. Checking the validity of 
 *       messages each time it is accessed would be too great an overhead.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS 
 *  - ::MB_ERR_INVALID (invalid or null board given) 
 *  - ::MB_ERR_MEMALLOC (error allocating memory for Iterator object or pooled_list)
 *  - ::MB_ERR_LOCKED (\c mb is locked)
 *  - ::MB_ERR_INTERNAL (possible bug. Recompile and run in debug mode for hints)
 *  - ::MB_ERR_OVERFLOW (MessageBoard overflow. Too many Iterators created.)
 */
int MB_Iterator_CreateFilteredSorted(MBt_Board mb, MBt_Iterator *itr_ptr, \
        int (*filterFunc)(const void *msg, const void *params), \
        void *filterFuncParams, \
        int (*cmpFunc)(const void *msg1, const void *msg2) ) {
    
    int rc, mcount, i, elemOut = 0;
    OM_key_t rc_om;
    void **ptr_array = NULL;
    void *new;
    MBIt_Board *board;
    MBIt_Iterator *iter;
    
    /* Check for NULL message board */
    if (mb == MB_NULL_MBOARD) 
    {
        P_FUNCFAIL("Cannot create iterator for null board (MB_NULL_MBOARD)");
        return MB_ERR_INVALID;
    }
    
    /* get ptr to board */
    board = (MBIt_Board*)MBI_getMBoardRef(mb);
    if (board == NULL) 
    {
        P_FUNCFAIL("Invalid board handle (%d)", (int)mb);
        return MB_ERR_INVALID;
    }
    mcount = (int)board->data->count_current;
    
    /* check if board is locked */
    if (board->locked == MB_TRUE) 
    {
        P_FUNCFAIL("Board (%d) is locked", (int)mb);
        return MB_ERR_LOCKED;
    }
    
    /* Allocate Iterator object */
    iter = (MBIt_Iterator*)malloc(sizeof(MBIt_Iterator));
    assert(iter != NULL);
    if (iter == NULL) 
    {
        P_FUNCFAIL("Could not allocate required memory");
        return MB_ERR_MEMALLOC;
    }
    
    /* assign mb handle to iterator */
    iter->mb        = mb;
    iter->msgsize   = board->data->elem_size;
    iter->cursor    = NULL;
    iter->iterating = 0;
    
    /* allocate memory for address list */
    rc = pl_create(&(iter->data), sizeof(void *), MB_CONFIG_SERIAL_POOLSIZE / 2);
    if (rc != PL_SUCCESS)
    {
        free(iter);
        if (rc == PL_ERR_MALLOC) 
        {
            P_FUNCFAIL("Could not allocate required memory");
            return MB_ERR_MEMALLOC;
        }
        else 
        {
            P_FUNCFAIL("pl_create() returned with err code %d", rc);
            return MB_ERR_INTERNAL;
        }
    }
    
    /* generate sorted message address array */
    if (mcount > 0)
    {
        ptr_array = get_sorted_filtered_ptr_list(mb, mcount, cmpFunc, \
                filterFunc, filterFuncParams, &elemOut);
        assert(ptr_array != NULL);
    }
    
    /* populate iterator */
    for (i = 0; i < elemOut; i++)
    {
        rc = pl_newnode(iter->data, &new);
        assert(rc == PL_SUCCESS);
        memcpy(new, &ptr_array[i], sizeof(void*));

    }
    
    if (ptr_array != NULL) free(ptr_array);
    
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
    
    P_INFO("Iterator created (iter:%d, board:%d, mcount:%d) - FILTERED+SORTED", 
            (int)rc_om, (int)mb, (int)iter->data->count_current);
    
    return MB_SUCCESS;
}

static void ** get_sorted_filtered_ptr_list(MBt_Board mb, int mcount, \
        int (*cmpFunc)(const void *msg1, const void *msg2), \
        int (*filterFunc)(const void *msg, const void *params), \
        void *filterFuncParams, \
        int *elemOut) {
    
    int rc, index;
    void ** array;
    void * obj;
    MBIt_Board *board;
    pl_address_node *pl_itr;
    array = (void **)malloc(sizeof(void *) * mcount);
    
    /* populate ptr array */
    board = (MBIt_Board *)MBI_getMBoardRef(mb);
    assert(board != NULL);
    assert(mcount == (int)board->data->count_current);
    index = 0;
    
    assert(board->data != NULL);
    for (pl_itr = PL_ITERATOR(board->data); pl_itr; pl_itr = pl_itr->next)
    {
        obj = PL_NODEDATA(pl_itr);
        assert(obj != NULL);
        
        /* run filter on message */
        rc = (*filterFunc)(obj, filterFuncParams);
        
        /* don't add if rejected by filter */
        if (rc == 0) continue; 
        
        /* add obj to list of objects */
        array[index++] = obj;
    }
    
    /* assign cmpFunc to file global ptr
     * Note that this makes the whole setup non-thread safe
     */
    funcPtr = cmpFunc;
    qsort((void *)array, (size_t)index, sizeof(void *), ptrSort);
    
    /* assign output value for number of elements */
    *elemOut = index;
    
    return array;
}

static int ptrSort(const void *p1, const void *p2) {
    return (*funcPtr)(*((void**)p1), *((void**)p2));
}
