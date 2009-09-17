/* $Id$ */
/*!
 * \file mb_settings.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : Aug 2009
 *      Copyright (c) 2009 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Header file to store library wide configuration parameters
 * 
 * Holds datastructure to store configuration parameters. These parameters
 * control how libmboard performs certain operations.
 * 
 * Each of these parameters are set to a default value, but can be modified
 * by users using environment variables. See src/utils/settings.c
 * 
 */

#ifndef MB_SETTINGS_H_
#define MB_SETTINGS_H_

#ifndef BITFIELD_T_DEFINED
#define BITFIELD_T_DEFINED
/*! \brief Dummy type used to indicate that a specific struct member
 * will be used as a bit field
 */
typedef unsigned int bitfield_t;
#endif

/*! \brief Enumeration of different communication protocols available
 * 
 * The different strategies are:
 *  - Old comm routines (OLD): Comm routine pre 0.2.1. Similar to HANDSHAKE
 *    but does not consider board access modes.
 *  - Staged Issends (HANDSHAKE): similar to OLD, but takes into account
 *    access modes and sync patterns.
 */
enum MBIt_config_protocols {
    MB_CONFIG_PROTO_START,
    
    MB_CONFIG_PROTO_HANDSHAKE,
    MB_CONFIG_PROTO_OLD,
    
    MB_CONFIG_PROTO_END
};

/*! \brief Datastructure to store global settings */
struct MBIt_config_data {
    /*! \brief Should MB_Clear() recycle allocated memory? */
    bitfield_t mempool_recycle :1;
    
    /*! \brief number of elements per memory block 
     * 
     * Used to define memory pool size in MB_Create()
     */
    unsigned int mempool_blocksize;
    
    /*! \brief ID of communication protocol to use 

     * 
     */
    enum MBIt_config_protocols comm_protocol;
};

/*! \brief Variable storing global settings
 * 
 * Instantiated and populated in src/utils/settings.c
 */
extern struct MBIt_config_data MBI_CONFIG;

void MBI_update_settings(void);

#endif /*MB_SETTINGS_H_*/
