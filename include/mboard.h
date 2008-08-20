/* $Id$ */
/*!
 * \file mboard.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : August 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief This should be the only header file that has to 
 *        be included by libmboard users
 * \warning This library is designed to work only on homogenous systems
 * 
 * 
 * \todo When in debug mode, print useful messages if we know what is wrong
 *       instead of just using \c assert()
 * 
 * \todo Generate/install libmboard-conf script to help users use the right
 * flags for linking libmboard to their code (include flags for pthread 
 * and MPI support). 
 * \ifnot userdoc
 *     \code
 *     [shawn@aphek]$ libmboard-conf --ldflags --parallel --debug
 *     -L/usr/libmboard/lib -L/usr/mpich2/lib -lmpi -pthread -lmboard_pd
 *     \endcode 
 * \endif
 */
/*!
 * \mainpage libmboard (Message Board Library)
 * 
 * \section OVERVIEW Overview
 * The Message Board Library provides memory management and message data
 * synchronisation facilities for multi-agent simulations generated using the 
 * FLAME framework (http://www.flame.ac.uk). 
 * 
 * \image html mboard_flame.png
 * \image latex mboard_flame.eps "Message Board Library" width=10cm
 * 
 * As agents only interact with its environment (and each other) via messages, 
 * the Message Board library serves as a means of achieving parallelisation. 
 * Agents can be farmed out across multiple processors and simulated 
 * concurrently, while a coherent simulation is maintained through a unified 
 * view of the distributed Message Boards.
 * 
 * Synchronisation of the message boards are non-blocking as they are 
 * performed on a separate communication thread, allowing much of the 
 * communication time to be overlapped with computation.
 * 
 * 
 * \ifnot userdoc
 * - \ref todo
 * \else
 * 
 * \section SOURCE Obtaining the source
 * 
 * You can download the latest release from CCPForge 
 * (http://ccpforge.cse.rl.ac.uk/frs/?group_id=8). We currently only provide
 * private releases, so you will need to be logged in as a member of the 
 * FLAME framework project.
 * 
 * \subsection SOURCE_DEVEL Developers and maintainers
 * 
 * If you are a developer and wish to use the development version (unstable), 
 * you can check out a copy from SVN (http://ccpforge.cse.rl.ac.uk/svn/xagents/trunk/libmboard). 
 * Within the checked out directory, you can either: 
 * -# directly use the maintainer source by running <tt>./autogen.sh</tt> to generate the 
 *    \c Makefiles and \c configure script, or
 * -#  generate your own release file by running <tt>./create_distribution.sh</tt>. 
 *    A \c *.tar.gz file will be generate (and tested). 
 * 
 * 
 * \section INSTALLATION Building and installing the Message Board library
 * 
 * -# Within the source directory, run <tt>"./configure"</tt>. This will configure 
 *    the source code for your system. 
 *  - If you do not have \c root access, or do not wish to install the library 
 *    into the default location (\c /usr/local), you can specify an alternative
 *    location by running <tt>"./configure --prefix=/your/target/dir"</tt> instead. 
 *  - You can also provide further information to the \c configure script as
 *    arguments. Run <tt>"./configure --help"</tt> for a list of possible options.
 * -# Upon successful configuration, run <tt>"make"</tt> to compile the project. 
 * -# (optional) You can run <tt>"make test"</tt> to compile and run the unit
 *    tests. You will need to have CUnit (http://cunit.sourceforge.net/) installed.
 * -# To install your newly built library, run <tt>"make install"</tt>. This 
 *    will install the libraries, header files, and scripts to either the 
 *    default location or the directory you may have specified earlier.
 * 
 * \section USAGE Using the library
 * 
 * To use the Message Board library with your code, you will need to include
 * the mboard.h header file, and call the appropriate \ref FUNC. All Message
 * Board routines return integer-based \ref RC. It is recommended that 
 * you always check the return code, and include sufficient error handling 
 * if the routine ends errorneously.
 * 
 * When linking your executable, you will need to link in the appropriate
 * Message Board library. There are four versions available:
 * - link with <tt>-lmboard_s</tt> for the serial version.
 * - link with <tt>-lmboard_sd</tt> for the serial \b DEBUG version.
 * - link with <tt>-lmboard_p</tt> for the parallel version.
 * - link with <tt>-lmboard_pd</tt> for the parallel \b DEBUG version
 * 
 * Always use the \b DEBUG version for during the development and testing
 * stage of your project. They may incur performance overheads, but the \c DEBUG
 * versions include crucial checks and assertions to ensure that the library 
 * is used correctly. 
 * Once your code has been validated and verified, you can switch to the
 * standard version for your production runs.
 * 
 * If your library was install to a non-default location (by configuring with
 * <tt>"./configure --prefix=/your/target/dir"</tt>), you will need to inform
 * your compiler/linker where to locate the Message Board libraries and header
 * files.
 * - append '<tt>-I/your/target/dir/<b>include</b></tt>' to 
 *   your compilation flags (\c CFLAGS).
 * - append '<tt>-L/your/target/dir/<b>lib</b></tt>' to 
 *   your linker flags (\c LDFLAGS).
 * 
 * The parallel versions of the library uses \c MPI and \c pthreads. Therefore,
 * you may need additional compilation options or specific compilers when using
 * then with you code. This depends on how you system was set up.
 * \note In the next version, we plan to include a <tt>libmboard-conf</tt>
 * script that will assist you in generating the necessary flags for compiling
 * your code with the Message Board library.
 * 
 * \section EXAMPLE Example
 * 
 * The \c ./example/circle_mb directory within the source contains an example 
 * of how libmboard can be used within a project.
 * 
 *  
 * \endif
 */

/*!\ifnot userdoc 
 * \defgroup MB_API Message Board API
 * 
 * Message Board routines exposed to users.
 * \endif
 */
/*!\if userdoc 
 * \defgroup FUNC Message Board API Routines
 * 
 * Routines to create and use Message Boards
 * \endif
 */
/*!\if userdoc 
 * \defgroup  RC Return Codes
 * 
 * All Message Board routines return an <tt>int</tt>-based return code. 
 * It is recommended that users always check the return code of all 
 * routine calls, and include sufficient error handling if the routine
 * ends errorneously.
 * 
 * The following is a list of possible return codes and their description.
 * \endif
 */
/*!\if userdoc 
 * \defgroup DT Datatypes
 * 
 * The following is a list datatypes defined in libmboard. These datatypes
 * are handles that represent opaque objects used during the interation with 
 * the Message Board library.
 *  
 * \endif
 */
/*!\if userdoc 
 * \defgroup CONST Constants
 * 
 * The following is a list constants defined in libmboard.
 * \endif
 */


#ifndef MBOARD_H_
#define MBOARD_H_

#include <stddef.h>
#include <limits.h>
#include "mb_objmap.h"

/*! 
 * \var MBt_handle
 * \ingroup MB_API
 * \brief Mapping of opaque object handle to internal representation
 */
typedef OM_key_t MBt_handle; 

/* ========== DATATYPES ========== */

/*!
 * \var MBt_Board
 * \ingroup MB_API
 * \ingroup DT
 * \brief A handle to store Message Board objects
 * 
 * Boards are objects that store messages. A board can be created (using
 * ::MB_Create()) to store data structures of arbitrary type. To store 
 * messages/data of different types, you will need to create diffent 
 * Boards.
 * 
 * Once a board is created, it will remain valid until it is deleted using
 * ::MB_Delete(). It can also be emptied/cleared using ::MB_Clear().
 * 
 * Messages can be added to the Board using ::MB_AddMessage(). However, 
 * messages can only be accessed through Iterators (see 
 * ::MB_Iterator_Create()).
 * 
 * When working in a parallel environment, a unified view of the Message 
 * Board will only be available after it has been synchronised. See:
 * - ::MB_SyncStart()
 * - ::MB_SyncComplete()
 * - ::MB_SyncTest()
 * 
 * \par See also:
 * - ::MB_NULL_MBOARD
 */
typedef MBt_handle MBt_Board;

/*!
 * \var MBt_Iterator
 * \ingroup MB_API
 * \ingroup DT
 * \brief A handle to store Iterator objects
 * 
 * Iterators are objects that allow users to traverse the contents of a
 * Message Board (::MBt_Board). Iterators can be created from a valid 
 * board using the following routines:
 * 
 * - ::MB_Iterator_Create()
 * - ::MB_Iterator_CreateFiltered()
 * - ::MB_Iterator_CreateSorted()
 * - ::MB_Iterator_CreateFilteredSorted()
 * 
 * Once an Iterator is created, it will remain valid as long as the corresponding
 * board remains intact, and until it is deleted using ::MB_Iterator_Delete().
 * 
 * Messages can be read from Iterators by making repeated calls to 
 * ::MB_Iterator_GetMessage().
 * 
 * \par See also:
 * - ::MB_NULL_ITERATOR
 * - ::MB_Iterator_Rewind()
 * - ::MB_Iterator_Randomise() 
 * 
 */
typedef MBt_handle MBt_Iterator;

/*!
 * \var MBt_Function
 * \ingroup MB_API
 * \ingroup DT
 * \brief A handle to store Registered Functions
 * 
 * Registered Functions are objects that represent user functions that have
 * been registered with the Message Board Library using 
 * ::MB_Function_Register(). 
 * 
 * This registration provides a unique handle to the function that is 
 * recognised across all processing nodes and can therefore be passed on as 
 * filter functions to ::MB_Function_Assign().
 * 
 * The Registered Function is valid until it is freed using 
 * ::MB_Function_Free().
 * 
 * \par See also:
 * - ::MB_NULL_FUNCTION
 * 
 */
typedef MBt_handle MBt_Function;

/* ========== User Routines ========== */

/* Initialise libmboard environment */
int MB_Env_Init(void);

/* Terminate libmboard environment */
int MB_Env_Finalise(void);

/* Indicate whether MPI_Env_Init has been called */
int MB_Env_Initialised(void);

/* Indicate whether MPI_Env_Finalised has been called */
int MB_Env_Finalised(void);

/* Create a new message board */
int MB_Create(MBt_Board *mb_ptr, size_t msgsize);

/* Add message to board */
int MB_AddMessage(MBt_Board mb, void *msg);

/* Clear all messages in board */
int MB_Clear (MBt_Board mb);

/* Delete board and free allocated memory */
int MB_Delete(MBt_Board *mb_ptr);

/* Create a message iterator */
int MB_Iterator_Create(MBt_Board mb, MBt_Iterator *itr_ptr);

/* Create a sorted message iterator */
int MB_Iterator_CreateSorted(MBt_Board mb, MBt_Iterator *itr_ptr, 
    int (*cmpFunc)(const void *msg1, const void *msg2) );

/* Create a filtered message iterator */
int MB_Iterator_CreateFiltered(MBt_Board mb, MBt_Iterator *itr_ptr, 
        int (*filterFunc)(const void *msg, const void *params), 
        void *filterFuncParams );

/* Create a filtered and sorted message iterator */
int MB_Iterator_CreateFilteredSorted(MBt_Board mb, MBt_Iterator *itr_ptr, 
        int (*filterFunc)(const void *msg, const void *params), 
        void *filterFuncParams, 
        int (*cmpFunc)(const void *msg1, const void *msg2) );

/* Delete an iterator */
int MB_Iterator_Delete(MBt_Iterator *itr_ptr);

/* Get next message from iterator */
int MB_Iterator_GetMessage(MBt_Iterator itr, void **msg_ptr);

/* Rewind Iterator */
int MB_Iterator_Rewind(MBt_Iterator itr);

/* Randomise an Iterator */
int MB_Iterator_Randomise(MBt_Iterator itr);

/* Initiate non-blocking synchronisation of message board */
int MB_SyncStart(MBt_Board mb);

/* Non-blocking routine to test synchronisation completion of message board  */
int MB_SyncTest(MBt_Board mb, int *flag);

/* Complete non-blocking synchronisation of message board */
int MB_SyncComplete(MBt_Board mb);

/* Register a function */
int MB_Function_Register(MBt_Function *fh_ptr, 
        int (*filterFunc)(const void *msg, const void *params) );

/* Assign function handle to a message board */
int MB_Function_Assign(MBt_Board mb, MBt_Function fh, 
        void *params, size_t param_size);

/* Deallocate a registered function */
int MB_Function_Free(MBt_Function *fh_ptr);


/* =========== Constants ================= */



/*!
 * \def MB_NULL_MBOARD
 * \ingroup MB_API
 * \ingroup CONST
 * \brief Null Message Board
 * 
 * This value represents an non-existent or invalid Message Board. It
 * is typically returned in place of a Message Board that has been 
 * deleted, or after an erroneous creation of a Message board.
 * 
 */
#define MB_NULL_MBOARD     (MBt_Board)OM_NULL_INDEX

/*!
 * \def MB_NULL_ITERATOR
 * \ingroup MB_API
 * \ingroup CONST
 * \brief Null Iterator
 * 
 * This value represents an non-existent or invalid Iterator object. It
 * is typically returned in place of an Iterator that has been 
 * deleted, or after an erroneous creation of an Iterator.
 * 
 */
#define MB_NULL_ITERATOR   (MBt_Iterator)OM_NULL_INDEX

/*!
 * \def MB_NULL_FUNCTION
 * \ingroup MB_API
 * \ingroup CONST
 * \brief Null Function
 * 
 * This value represents an non-existent or invalid Registered Function. It
 * is typically returned in place of a Registered Function that has been 
 * deleted, or after an erroneous registration of an function.
 */
#define MB_NULL_FUNCTION   (MBt_Iterator)OM_NULL_INDEX


/*!
 * \def MB_TRUE
 * \ingroup MB_API
 * \ingroup CONST
 * \brief Internal representation of a logical \c TRUE 
 */

#define MB_TRUE 1
/*!
 * \def MB_FALSE
 * \ingroup MB_API
 * \ingroup CONST
 * \brief Internal representation of a logical \c FALSE 
 */
#define MB_FALSE 0

/* ========== RETURN TYPES ========== */

/*!
 * \def MB_SUCCESS
 * \ingroup MB_API
 * \ingroup RC
 * \brief Return Code: Success
 * 
 * Specifies a successful execution.
 */
#define MB_SUCCESS     0

/*!
 * \def MB_ERR_MEMALLOC
 * \ingroup MB_API
 * \ingroup RC
 * \brief Return Code: Memory allocation error
 * 
 * Failed to allocate required memory. We have most likely exhausted all
 * available memory on the system. Use the \c DEBUG version of libmboard
 * for more information on where this occured.
 */
#define MB_ERR_MEMALLOC 1

/*!
 * \def MB_ERR_INVALID
 * \ingroup MB_API
 * \ingroup RC
 * \brief Return Code: Input error
 * 
 * One or more of the given input parameter is invalid. 
 */
#define MB_ERR_INVALID  2 

/*!
 * \def MB_ERR_LOCKED
 * \ingroup MB_API
 * \ingroup RC
 * \brief Return Code: Object locked
 * 
 * Object has being locked by another process.
 */
#define MB_ERR_LOCKED   3 

/*!
 * \def MB_ERR_MPI
 * \ingroup MB_API
 * \ingroup RC
 * \brief Return Code: MPI Error
 * 
 * An MPI related error has occured. Use the \c DEBUG version of libmboard
 * for more information on where this occured.
 */
#define MB_ERR_MPI      4

/*!
 * \def MB_ERR_ENV
 * \ingroup MB_API
 * \ingroup RC
 * \brief Return Code: Environment Error
 * 
 * Specifies error due to uninitalised or invalid environment state. This may
 * be due to users calling Message Board routines before initialising the 
 * environment with ::MB_Env_Init(), or after the environment has been 
 * finalised with ::MB_Env_Finalise().
 */
#define MB_ERR_ENV      5 

/*!
 * \def MB_ERR_OVERFLOW
 * \ingroup MB_API
 * \ingroup RC
 * \brief Return Code: Overflow Error
 * 
 * Specifies error due overflow in internal variable or storage.
 * Use the \c DEBUG version of libmboard
 * for more information on where this occured.
 */
#define MB_ERR_OVERFLOW     6

/*!
 * \def MB_ERR_INTERNAL
 * \ingroup MB_API
 * \ingroup RC
 * \brief Return Code: Internal Error
 * 
 * Specifies internal implementation error.  Possibly a bug.
 * Use the \c DEBUG version of libmboard
 * for more information on where this occured.
 */
#define MB_ERR_INTERNAL     7

/*!
 * \def MB_ERR_USER
 * \ingroup MB_API
 * \ingroup RC
 * \brief Return Code: User Error
 * 
 * Specifies error due to something the user has done (or not done). See 
 * documentation or any output message for details.
 */
#define MB_ERR_USER     8

/*!
 * \def MB_SUCCESS_2
 * \ingroup MB_API
 * \ingroup RC
 * \brief Return Code: Success 
 * 
 * Specifies a successful execution (but with routine specific connotations).
 */
#define MB_SUCCESS_2    100

/*!
 * \def MB_ERR_NOT_IMPLEMENTED
 * \ingroup MB_API
 * \ingroup RC
 * \brief Return Code: Not Implemented
 * 
 * Requested operation has not been implemented.
 * 
 */
#define MB_ERR_NOT_IMPLEMENTED      111



#endif /*MBOARD_H_*/


/* =================== BEGIN USER DOCUMENTATION =========================== */
/* -- Doxygen comments below will be used only in the User Documentation -- */

/*!\if userdoc
 * \fn MB_Env_Init(void)
 * \ingroup FUNC
 * \brief Initialises the libmboard environment
 * 
 * This routine must be called before any other libmboard routines 
 * (apart for MB_Env_Initialised() and MB_Env_Finalised()). It launches the 
 * communication thread and initialises all internal data structures required
 * by the library.
 * 
 * The libmboard environment should be initialised only once, and never 
 * re-initialised once it has been finalised (using ::MB_Env_Finalise()).
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_MPI (MPI Environment not yet started)
 *  - ::MB_ERR_ENV (libmboard environment already started)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 * 
 * \endif
 */

/*!\if userdoc
 * \fn MB_Env_Finalise (void)
 * \ingroup FUNC
 * \brief Finalises the libmboard environment
 * 
 * This should be the last libmboard routine called within a program 
 * (apart for MB_Env_Initialised() and MB_Env_Finalised()). It deallocates
 * all internal data structures and terminates the communication thread.
 * 
 * It is erroneous to finalise the environment while there are pending 
 * board synchronisations, i.e. all ::MB_SyncStart() must be completed 
 * with a matching ::MB_SyncComplete() (or successful ::MB_SyncTest()). 
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_ENV (libmboard environment not yet started, or already finalised)
 * 
 * \endif
 */

/*!\if userdoc
 * \fn MB_Env_Initialised (void)
 * \ingroup FUNC
 * \brief Indicates whether MB_Env_Init() has been called successfully
 * 
 * This routine will return ::MB_SUCCESS if the environment has been initialised, 
 * or ::MB_ERR_ENV otherwise.
 * 
 *  Possible return codes:
 *  - ::MB_SUCCESS 
 *  - ::MB_ERR_ENV (libmboard environment was not successfully set up, 
 *                 or, has already been finalised)
 * 
 * \endif
 */

/*!\if userdoc
 * \fn MB_Env_Finalised(void)
 * \ingroup FUNC
 * \brief Indicates whether MB_Env_Finalise() has been called
 * 
 * This routine will return ::MB_SUCCESS if the environment has been finalised, 
 * or ::MB_ERR_ENV otherwise.
 * 
 *  Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_ENV (libmboard environment has not been finalised)
 * 
 * \endif
 */

/*!\if userdoc
 * \fn MB_Create(MBt_Board *mb_ptr, size_t msgsize)
 * \ingroup FUNC
 * \brief Instantiates a new Message Board object
 * \param[out] mb_ptr Address of Message Board handle
 * \param[in] msgsize Size of message that this Message Board will be used for
 * 
 * Creates a new board for storing messages of size \c msgsize and returns a 
 * handle to the board via \c mb_ptr .
 *  
 * In the parallel debug version, this routine is blocking and will return when 
 * all processes have issued and completed the call. This effectively 
 * synchronises all processes. It is the users' responsibility to ensure 
 * that all processes issue the call (with the same values of \c msgsize) 
 * to prevent deadlocks.
 * 
 * If this routine returns with an error, \c mb_ptr will be set to ::MB_NULL_MBOARD.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c msgsize is invalid)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_OVERFLOW (too many boards created)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 *  - ::MB_ERR_ENV (Message Board environment not yet initialised)
 * 
 * Usage example:
 * \include ex_mb_create.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_AddMessage(MBt_Board mb, void *msg)
 * \ingroup FUNC
 * \brief Adds a message to the Message Board
 * \param[in] mb Message Board handle
 * \param[in] msg Address of the message to be added
 * 
 * Messages added to the board must be of the size specified
 * during the creation of the board. Adding messages of a different size
 * may not cause an error code to be returned, but will lead to unexpected
 * behavior and possible segmentation faults.
 * 
 * The message data addressed by \c msg is cloned and stored in the message 
 * board. Users are free to modify, reuse, or deallocate their copy of the 
 * message after this routine has completed.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb is null or invalid)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_LOCKED (\c mb is locked by another process)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 * 
 * Usage example:
 * \include ex_mb_addmessage.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_Clear(MBt_Board mb)
 * \ingroup FUNC
 * \brief Clears the Message Board
 * \param[in] mb Message Board handle
 * 
 * Deletes all messages from the board. The board can be reused
 * for adding more messages of the same type.
 * 
 * Once a board is cleared, all Iterators associated with the board is no longer
 * valid and has to be recreated. It is the users' responsibility to ensure 
 * that invalidated Iterators are never used.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb is null or invalid)
 *  - ::MB_ERR_LOCKED (\c mb is locked by another process)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 * 
 * Usage example:
 * \include ex_mb_clear.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_Delete(MBt_Board *mb_ptr)
 * \ingroup FUNC
 * \brief Deletes a Message Board
 * \param[in,out] mb_ptr Address of Message Board handle
 * 
 * Upon successful deletion, the handle referenced by \c mb_ptr will be set 
 * to ::MB_NULL_MBOARD . This handle can be reused when creating a new board.
 * 
 * If an error occurs, this routine will return an error code, and \c mb_ptr 
 * will remain unchanged.
 * 
 * If a null board (::MB_NULL_MBOARD) is given, the routine will return 
 * immediately with ::MB_SUCCESS
 * 
 * Once a board is deleted, all Iterators associated with the board is no longer
 * valid. It is the users' responsibility to ensure that invalidated Iterators 
 * are never used.
 * 
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb is invalid)
 *  - ::MB_ERR_LOCKED (\c mb is locked by another process)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 * 
 * Usage example:
 * \include ex_mb_delete.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_Iterator_Create(MBt_Board mb, MBt_Iterator *itr_ptr)
 * \ingroup FUNC
 * \brief Creates a new Iterator for accessing messages in board \c mb
 * \param[in] mb Message Board handle
 * \param[out] itr_ptr Address of Iterator Handle
 * 
 * Upon successful creation of the Iterator, the routine  
 * returns a handle to the Iterator via \c itr_ptr .
 *  
 * Attempts to create an Iterator against a null board (::MB_NULL_MBOARD) will 
 * result in an ::MB_ERR_INVALID error.
 * 
 * If this routine returns with an error, \c itr_ptr will remain unchanged.
 * 
 * \warning The Iterator will remain valid as long as the board it was created for
 * is not modified, cleared or deleted. Reading messages from an invalid 
 * Iterator will lead to undefined behaviour and possible segmentation 
 * faults. It is the users' responsibility to ensure that only valid Iterators 
 * are used.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb is null or invalid)
 *  - ::MB_ERR_LOCKED (\c mb is locked by another process)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 * 
 * Usage example:
 * \include ex_mb_it_create.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_Iterator_CreateSorted(MBt_Board mb, MBt_Iterator *itr_ptr, int (*cmpFunc)(const void *msg1, const void *msg2))
 * \ingroup FUNC
 * \brief Creates a new Iterator for accessing sorted messages in board \c mb
 * \param[in] mb Message Board handle
 * \param[out] itr_ptr Address of Iterator Handle
 * \param[in] cmpFunc Pointer to user-defined comparison function
 * 
 * Creates a new Iterator for accessing messages in board \c mb, and 
 * returns a handle to the iterator via \c itr_ptr. This Iterator will allow 
 * users to retrieve ordered messages from \c mb without modifying the board
 * itself.
 * 
 * The user-defined comparison function (\c cmpFunc) must return an integer 
 * less than, equal to, or greater than zero if the first message is 
 * considered to be respectively less than, equal to, or greater than the 
 * second. In short:
 *  - <tt>0 if (msg1 == msg2)</tt>
 *  - <tt>< 0 if (msg1 < msg2)</tt>
 *  - <tt>> 0 if (msg1 > msg2)</tt>
 *  
 * If two members compare as equal, their order in the sorted Iterator is undefined.
 * 
 * Attempts to create an Iterator against a null board (::MB_NULL_MBOARD) will 
 * result in an ::MB_ERR_INVALID error.
 * 
 * If this routine returns with an error, \c itr_ptr will remain unchanged.
 * 
 * \warning The Iterator will remain valid as long as the board it was created for
 * is not modified, cleared or deleted. Reading messages from an invalid 
 * Iterator will lead to undefined behaviour and possible segmentation 
 * faults. It is the users' responsibility to ensure that Iterators are
 * not invalidated before they are used.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb is null or invalid)
 *  - ::MB_ERR_LOCKED (\c mb is locked by another process)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 * 
 * Usage example:
 * \include ex_mb_it_create_s.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_Iterator_CreateFiltered(MBt_Board mb, MBt_Iterator *itr_ptr, int (*filterFunc)(const void *msg, const void *params), void *filterFuncParams )
 * \ingroup FUNC
 * \brief Creates a new Iterator for accessing a selection of messages 
 *        in board \c mb 
 * \param[in] mb Message Board handle
 * \param[out] itr_ptr Address of Iterator Handle
 * \param[in] filterFunc Pointer to user-defined filter function
 * \param[in] filterFuncParams Pointer to input data that will be passed into \c filterFunc 
 * 
 * Creates a new Iterator for accessing messages in board \c mb, and 
 * returns a handle to the iterator via \c itr_ptr. This Iterator will allow 
 * users to retrieve a filtered selection of messages from \c mb without modifying the board
 * itself.
 * 
 * The user-defined filter function (\c filterFunc) must return \c 0 if a message is to be
 * rejected by the filter, or \c 1 if it is to be accepted.
 * 
 * The \c filterFuncParam argument allows users to pass on additional information
 * to \c filterFunc (see example code below). Users may use\c NULL in place of 
 * \c filterFuncParam if \c filterFunc does not require additional information.
 * 
 * Attempts to create an Iterator against a null board (::MB_NULL_MBOARD) will 
 * result in an ::MB_ERR_INVALID error.
 * 
 * If this routine returns with an error, \c itr_ptr will remain unchanged.
 * 
 * \warning The Iterator will remain valid as long as the board it was created for
 * is not modified, cleared or deleted. Reading messages from an invalid 
 * Iterator will lead to undefined behaviour and possible segmentation 
 * faults. It is the users' responsibility to ensure that Iterators are
 * not invalidated before they are used.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb is null or invalid)
 *  - ::MB_ERR_LOCKED (\c mb is locked by another process)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 * 
 * Usage example:
 * \include ex_mb_it_create_f.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_Iterator_CreateFilteredSorted(MBt_Board mb, MBt_Iterator *itr_ptr, int (*filterFunc)(const void *msg, const void *params), void *filterFuncParams, int (*cmpFunc)(const void *msg1, const void *msg2))
 * \ingroup FUNC
 * \brief Instantiates a new Iterator for accessing a sorted selection of 
 *        messages in board \c mb 
 * \param[in] mb Message Board handle
 * \param[out] itr_ptr Address of Iterator Handle
 * \param[in] filterFunc Pointer to user-defined filter function
 * \param[in] filterFuncParams Pointer to input data that will be passed into \c filterFunc 
 * \param[in] cmpFunc Pointer to user-defined compariosn function
 *  
 * Creates a new Iterator for accessing messages in board \c mb, and 
 * returns a handle to the iterator via \c itr_ptr. This Iterator will allow 
 * users to retrieve a filtered selection of ordered messages from \c mb 
 * without modifying the board itself.
 * 
 * The user-defined filter function (\c filterFunc) must return \c 0 if a message is to be
 * rejected by the filter, or \c 1 if it is to be accepted.
 * 
 * The \c filterFuncParam argument allows users to pass on additional information
 * to \c filterFunc (see example code below). Users may use\c NULL in place of 
 * \c filterFuncParam if \c filterFunc does not require additional information.
 * 
 * The user-defined comparison function (\c cmpFunc) must return an integer 
 * less than, equal to, or greater than zero if the first message is 
 * considered to be respectively less than, equal to, or greater than the 
 * second. In short:
 *  - <tt>0 if (msg1 == msg2)</tt>
 *  - <tt>\< 0 if (msg1 \< msg2)</tt>
 *  - <tt>\> 0 if (msg1 \> msg2)</tt>
 * 
 * Attempts to create an Iterator against a null board (::MB_NULL_MBOARD) will 
 * result in an ::MB_ERR_INVALID error.
 * 
 * If this routine returns with an error, \c itr_ptr will remain unchanged.
 * 
 * \warning The Iterator will remain valid as long as the board it was created for
 * is not modified, cleared or deleted. Reading messages from an invalid 
 * Iterator will lead to undefined behaviour and possible segmentation 
 * faults. It is the users' responsibility to ensure that Iterators are
 * not invalidated before they are used.
 *
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb is null or invalid)
 *  - ::MB_ERR_LOCKED (\c mb is locked by another process)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 * 
 * Usage example:
 * \include ex_mb_it_create_fs.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_Iterator_Delete(MBt_Iterator *itr_ptr)
 * \ingroup FUNC
 * \brief Deletes an Iterator
 * \param[in,out] itr_ptr Address of Iterator Handle
 * 
 * Upon successful deletion, the handle referenced by \c itr_ptr will be set 
 * to ::MB_NULL_ITERATOR. This handle can be reused when creating a new 
 * Iterator of any kind.
 * 
 * If an error occurs, \c itr_ptr will remain unchanged.
 * 
 * If a null Iterator (::MB_NULL_ITERATOR) is passed in, the routine will return 
 * immediately with ::MB_SUCCESS
 * 
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c itr is invalid)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 * 
 * Usage example:
 * \include ex_mb_it_delete.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_Iterator_GetMessage(MBt_Iterator itr, void **msg_ptr)
 * \ingroup FUNC
 * \brief Returns next available message from Iterator
 * \param[in] itr Iterator Handle
 * \param[out] msg_ptr Address where pointer to message will be written to
 * 
 * After a successful call to the routine, \c msg_ptr will be assigned with 
 * a pointer to a newly allocated memory block containing the message 
 * data. It is the user's responsibility to free the memory associated with the 
 * returned msg.
 *
 * When there are no more messages to return, \c msg_ptr will be assigned with
 * \c NULL and the routine shall complete with the ::MB_SUCCESS return code.
 *
 * Any attempts to retrieve a message from a null Iterator (::MB_NULL_ITERATOR) will 
 * result in an ::MB_ERR_INVALID error.
 * 
 * In the event of an error, msg will be assigned \c NULL and the routine shall
 * return with an appropriate error code.
 * 
 * \warning If the given Iterator is invalidated due to a deletion or clearance 
 * of the target board, calling this routine on the invalid board may result in
 * either an undefined block of data or a segmentation fault.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c itr is null of invalid)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 * 
 * Usage example:
 * \include ex_mb_it_getmessage.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_Iterator_Rewind(MBt_Iterator itr)
 * \ingroup FUNC
 * \brief Rewinds an Iterator
 * \param[in] itr Iterator Handle
 * 
 * Resets the internal counters such that the next MB_Iterator_GetMessage() 
 * call on the given Iterator will obtain the first message in the list 
 * (or \c NULL if the Iterator is empty).
 * 
 * Rewinding a null Iterator (::MB_NULL_ITERATOR) will result in an 
 * ::MB_ERR_INVALID error.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c itr is null or invalid)
 * 
 * Usage example:
 * \include ex_mb_it_rewind.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_Iterator_Randomise(MBt_Iterator itr)
 * \ingroup FUNC
 * \brief Randomises the order of entries in an Iterator
 * \param[in] itr Iterator Handle
 * 
 * Apart from randomising the order of entries in the Iterator, this routine 
 * will also reset the internal counters leading to an effect similar to that
 * of MB_Iterator_Rewind().
 * 
 * Randomising a null Iterator (::MB_NULL_ITERATOR) will result in an 
 * ::MB_ERR_INVALID error.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c itr is null or invalid)
 * 
 * Usage example:
 * \include ex_mb_it_randomise.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_SyncStart(MBt_Board mb)
 * \ingroup FUNC
 * \brief Synchronises the content of the board across all processes
 * \param[in] mb Message Board Handle
 * 
 * This is a non-blocking routine which returns immediately after 
 * locking the message board and intialising the synchronisation 
 * process. The board should not be modified, cleared, or deleted 
 * until the synchronisation process is completed using
 * MB_SyncComplete() (or until MB_SyncTest() results in a ::MB_TRUE
 * flag).
 * 
 * In the serial version, this routine will do nothing apart from 
 * locking the message board. 
 * 
 * Synchronisation of a null board (::MB_NULL_MBOARD) is valid, and will 
 * return immediately with ::MB_SUCCESS
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb is invalid)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 *  - ::MB_ERR_LOCKED (\c mb is locked by another process)
 * 
 * Usage example:
 * \include ex_mb_sync.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_SyncTest(MBt_Board mb, int *flag)
 * \ingroup FUNC
 * \brief Inspects the completion status of a board synchronisation
 * \param[in] mb Message Board Handle
 * \param[out] flag address where return value will be written to
 * 
 * This routine is non-blocking, and will return after setting the
 * \c flag value to either ::MB_TRUE or ::MB_FALSE depending on the 
 * synchronisation completion status.
 * 
 * If synchronisation has completed, the ::MB_TRUE flag is returned, 
 * and the board is unlocked. The synchronisation process is considered 
 * to be completed, and users no longer need to call MB_SyncComplete() 
 * on this board.
 * 
 * Testing a null board (::MB_NULL_MBOARD) will always return with 
 * the ::MB_TRUE flag and ::MB_SUCCESS return code.
 * 
 * Testing a board that is not being synchronised is invalid,  
 * and will return with the ::MB_FALSE flag and ::MB_ERR_INVALID 
 * return code.
 * 
 * In the serial version, this routine will always return ::MB_TRUE as
 * synchronisation is assumed to be completed immediately after it started.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb is invalid or not being synchronised)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 * 
 * Usage example: see MB_SyncStart()
 * \endif
 */


/*!\if userdoc
 * \fn MB_SyncComplete(MBt_Board mb)
 * \ingroup FUNC
 * \brief Completes the synchronisation of a board
 * \param[in] mb Message Board Handle
 * 
 * This routine will block until the synchronisation of the board has
 * completed. Upon successful execution of this routine, the board
 * will be unlocked and ready for access.
 * 
 * In the serial version, this routine will do nothing apart from 
 * unlocking the message board. 
 * 
 * Synchronisation of a null board (::MB_NULL_MBOARD) is valid, and will 
 * return immediately with ::MB_SUCCESS
 * 
 * Completing synchronisation a board that is not being synchronised is invalid,  
 * and will return with the ::MB_ERR_INVALID error code.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c mb is invalid or not being synchronised)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 * 
 * Usage example: see MB_SyncStart()
 * \endif
 */

/*!\if userdoc
 * \fn MB_Function_Register(MBt_Function *fh_ptr, int (*filterFunc)(const void *msg, const void *params) );
 * \ingroup FUNC
 * \brief Registers a function
 * \param[out] fh_ptr Address to write Function Handle to
 * \param[in] filterFunc Pointer to user-defined function
 * 
 * Registers a filter function and returns a handle to the function via
 * \c fh_ptr. The handle is unique to that function, and is recognised across 
 * all processing nodes. 
 * 
 * Registered functions can be assigned to message boards
 * using MB_Function_Assign() to act as a filtering mechanism when retrieving
 * messages from remote nodes during a synchronisation. This reduces the number of
 * messages that need to be transferred and stored on each node.
 * 
 * If this routine returns with an error, \c fh_ptr will be set to :: MB_NULL_FUNCTION.
 * 
 * In the parallel debug version, this routine is blocking and will return when 
 * all processes have issued and completed the call. This effectively 
 * synchronises all processing nodes. It is the users' responsibility to ensure 
 * that all processing nodes issue the call (with the same values for \c filterFunc) 
 * to prevent deadlocks.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c filterFunc is NULL)
 *  - ::MB_ERR_MEMALLOC (unable to allocate required memory)
 *  - ::MB_ERR_INTERNAL (internal error, possibly a bug)
 * 
 * Usage example:
 * \include ex_mb_func.c
 * \endif
 */

/*!\if userdoc
 * \fn MB_Function_Assign(MBt_Board mb, MBt_Function fh, void *params, size_t param_size);
 * \ingroup FUNC
 * \brief Assigns function handle to a message board
 * \param[in] mb Message Board Handle
 * \param[in] fh Function Handle
 * \param[in] params Pointer to function parameters
 * \param[in] param_size Size of function parameters
 * 
 * This routine assigns a registered function to a Message Board. The function
 * will act as a filtering mechanism when retrieving
 * messages from remote nodes during a synchronisation. This reduces the number of
 * messages that need to be transferred and stored on each node.
 * 
 * For efficiency, boards must be assigned with the same \c fh on all
 * MPI processes. It is left to the user to ensure that this is so.
 * (this limitation may be removed or changed in the future if there is
 * a compelling reason to do so).
 * 
 * \c param_size can be of diffent across all processing nodes.
 * 
 * If \c params is \c NULL, \c param_size will be ignored. \c param can only
 * be \c NULL if all processing nodes also sets it to \c NULL.
 * 
 * \c fh can be ::MB_NULL_FUNCTION, in which case \c mb will be deassociated with any 
 * function that it was previously assigned with.
 * 
 * It is the users' responsibility to ensure that \c params is valid and
 * populated with the right data before board synchronisation. Data referenced
 * to by \c param must not be modified during the synchronisation process or
 * results from the synchronisation process will be erroneous, and may result
 * in a segmentation fault.
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (at least one of the input parameters is invalid.)
 *  - ::MB_ERR_LOCKED (\c mb is locked by another process)
 * \endif
 * 
 * Usage example: see MB_Function_Register()
 */

/*!\if userdoc
 * \fn MB_Function_Free(MBt_Function *fh_ptr);
 * \ingroup FUNC
 * \brief Deallocates a registered function
 * \param[in,out] fh_ptr Address of Function Handle
 * 
 * The function associated with \c fh_ptr will be deregistered, and \c fh_ptr 
 * will be set to ::MB_NULL_FUNCTION.  
 * 
 * Synchronisation of a Message Board assigned with a deregistered function
 * will result in an error. It is the users' responsibility to ensure that
 * this does not happen. 
 * 
 * Possible return codes:
 *  - ::MB_SUCCESS
 *  - ::MB_ERR_INVALID (\c fh_ptr is NULL or invalid)
 * \endif
 * 
 * Usage example: see MB_Function_Register()
 */


