/* $Id$ */
/*!
 * \file mb_tag_table.h
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : July 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief Header file for Tag Table
 * 
 */
/*! \defgroup TTABLE Utility: Tag Table
 * 
 * 
 * @{
 */ 
#ifndef MB_TAG_TABLE_H_
#define MB_TAG_TABLE_H_

#ifdef HAVE_CONFIG_H
#include "mb_config.h"
#endif

#include <stdlib.h>
#include <assert.h>

/*! \brief Return Code: Success */
#define TT_SUCCESS       0 
/*! \brief Return Code: Fatal Error */
#define TT_ERR_FATAL     1 
/*! \brief Return Code: Memory Allocation Error */
#define TT_ERR_MEMALLOC  2 
/*! \brief Return Code: Invalid Input */
#define TT_ERR_INVALID   3 


/* ----- Data Structures ----- */

/*! \brief Tag Table object */
typedef struct {
    
    /*! \brief Number of rows (number of messages in board) */
    int rows;
    
    /*! \brief Number of relevant columns (number of mpi tasks) */
    int cols;
    
    /*! \brief Size of row in bytes (ceil(cols / 8)) */ 
    size_t row_size;
    
    /*! \brief pointer to tag table data */
    char *table;
    
} tag_table;


/* ----- Function Prototypes ----- */
/* .... see src/utils/tag_table.c .... */
int tt_create(tag_table **tt_ptr_loc, int rows, int cols);
int tt_delete(tag_table **tt_ptr_loc);

int tt_setbits(tag_table *tt, int row, int byte_offset, char bitmask);
int tt_setbyte(tag_table *tt, int row, int byte_offset, char byte);

int tt_getrow(tag_table *tt, int row, char **row_ptr);
int tt_getcount_row(tag_table *tt, int row, int *num_ptr);
int tt_getcount_col(tag_table *tt, int col, int *num_ptr);

/*! @} */
#endif /*MB_TAG_TABLE_H_*/
