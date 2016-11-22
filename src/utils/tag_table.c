/* $Id: tag_table.c 2934 2012-07-27 14:08:11Z lsc $ */
/*!
 * \file tag_table.c
 * 
 * \code
 *      Author: Lee-Shawn Chin 
 *      Date  : July 2008 
 *      Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * \endcode
 * 
 * \brief ADT for indexing messages within the board that are tagged.
 * 
 */

#include "mb_tag_table.h"

/* use macros for bit operations */
/*! \brief get address of bits representing a row in the tag table */
#define GET_ROW_FROM_TT(tt, row) (tt->table + (row * tt->row_size))
/*! \brief Binary octet with only MSB set to 1 */
#define MSBMASK  (0x80)
/*! \brief query if most significant bit is set */
#define MSB_IS_SET(octet) (((octet) & MSBMASK) == MSBMASK)

/*!
 * \brief Creates a new Tag Table object 
 * \ingroup TTABLE
 * \param[out] tt_ptr_loc Address to write reference to newly created object
 * \param[in] rows Number of rows in table (usually, number of messages in board)
 * \param[in] cols Minimum number of columns in a row (usually, number of MPI tasks) 
 * 
 * Memory for tag table object and the table itself are allocated. All bits in table
 * are set to zero. Internal variables are also initialised.
 * 
 * The table itself (tag_table->table) is represented by a single contiguous 
 * block of memory (row-major). It is allocated with calloc(), so all allocated
 * bits are initialised to 0.
 * 
 * If an error occurs, \c tt_ptr_loc will be assigned \c NULL and the routine 
 * returns with the appropriate error code. All allocated memory should also 
 * be freed.
 * 
 * Possible return codes:
 *  - ::TT_SUCCESS
 *  - ::TT_ERR_INVALID (Invalid value for \c rows or \c cols)
 *  - ::TT_ERR_MEMALLOC (Unable to allocate required memory)
 */
int tt_create(tag_table **tt_ptr_loc, int rows, int cols) {
    
    tag_table *new;
    
    /* set return ptr to NULL before we begin, in case of errors */
    *tt_ptr_loc = NULL;
    
    if (rows <= 0 || cols <= 0) return TT_ERR_INVALID;
    
    /* allocate memory for tt object */
    new = (tag_table *)malloc(sizeof(tag_table));
    assert(new != NULL);
    if (new == NULL) return TT_ERR_MEMALLOC;
    
    /* assign values */
    new->rows = rows;
    new->cols = cols;
    new->row_size = (size_t)((int)(cols / 8) + 1);
    
    /* allocate memory for table data */
    new->table = (char *)calloc((size_t)new->rows, new->row_size);
    assert(new->table != NULL);
    if (new->table == NULL) 
    {
        free(new);
        return TT_ERR_MEMALLOC;
    }
    
    /* assign allocated object to return ptr */
    *tt_ptr_loc = new;
    
    return TT_SUCCESS;
}

/*!
 * \brief Deletes Tag Table object and deallocates all memory
 * \ingroup TTABLE
 * \param[in,out] tt_ptr_loc Pointer to Tag Table reference
 * 
 * This will deallocate all memory assigned to the object, and set the given
 * pointer to NULL.
 * 
 * It is an error to call this routine on a NULL pointer.
 * 
 * On error, \c tt_ptr_loc will remain unchanged.
 * 
 * Possible return codes:
 *  - ::TT_SUCCESS
 *  - ::TT_ERR_INVALID (\c tt_ptr_loc is \c NULL or corrupted)
 */
int tt_delete(tag_table **tt_ptr_loc) {
    
    tag_table *obj;
    
    if (tt_ptr_loc == NULL) return TT_ERR_INVALID;
    
    /* get reference to input object */
    obj = *tt_ptr_loc;
    if (obj == NULL || obj->table == NULL) return TT_ERR_INVALID; 
    
    /* set return ptr to NULL */
    *tt_ptr_loc = NULL;
    
    free(obj->table);
    free(obj);
    
    return TT_SUCCESS;
}

/*!
 * \brief Sets bits of a specified byte within the table
 * \ingroup TTABLE
 * \param[in] tt Pointer to Tag Table
 * \param[in] row Target row within Table
 * \param[in] byte_offset Offset within row (in unit of bytes, starts from 0)
 * \param[in] bitmask Binary mask or bits to set within target byte
 * 
 * Set the bits of a target byte (defined by \c row and \c byte_offset) within
 * the table.
 * 
 * This routine is different from ::tt_setbyte in that it performs a binary
 * OR of \c bitmask with the target byte. Bits that were previously set within
 * the target byte will remain set even if it is unset in \c bitmask.
 * 
 * On error, the tagtable will remain unchanged.
 * 
 * Possible return codes:
 *  - ::TT_SUCCESS
 *  - ::TT_ERR_INVALID (input in invalid, or \c tt is \c NULL or corrupted)
 */
int tt_setbits(tag_table *tt, int row, int byte_offset, char bitmask) {
    
    char *target_byte;
    
    if (tt == NULL || tt->table == NULL) return TT_ERR_INVALID;
    if (row >= tt->rows || byte_offset >= (int)tt->row_size) return TT_ERR_INVALID;
    
    target_byte  = GET_ROW_FROM_TT(tt, row) + byte_offset;
    *target_byte = *target_byte | bitmask;
    
    return TT_SUCCESS;
}


/*!
 * \brief Replaces the value of a specified byte within the table
 * \ingroup TTABLE
 * \param[in] tt Pointer to Tag Table
 * \param[in] row Target row within Table (starts from 0)
 * \param[in] byte_offset Offset within row (in unit of bytes, starts from 0)
 * \param[in] bitmask Value to write within target byte
 * 
 * Replace the value of a target byte (defined by \c row and \c byte_offset) within
 * the table.
 * 
 * This routine is different from tt_setbit() in that it replaces all bits
 * within the target byte. Bits that were previously set within
 * the target byte may be unset depending on the value of \c bitmask.
 * 
 * On error, the tagtable will remain unchanged.
 * 
 * Possible return codes:
 *  - ::TT_SUCCESS
 *  - ::TT_ERR_INVALID (input in invalid, or \c tt is \c NULL or corrupted)
 */
int tt_setbyte(tag_table *tt, int row, int byte_offset, char bitmask) {
    
    char *target_byte;
    
    if (tt == NULL || tt->table == NULL) return TT_ERR_INVALID;
    if (row >= tt->rows || byte_offset >= (int)tt->row_size) return TT_ERR_INVALID;
    
    target_byte  = GET_ROW_FROM_TT(tt, row) + byte_offset;
    *target_byte = bitmask;
        
    return TT_SUCCESS;
}

/*!
 * \brief Returns a pointer to a table row.
 * \ingroup TTABLE
 * \param[in] tt Pointer to Tag Table
 * \param[in] row Target row within Table (starts from 0)
 * \param[out] row_ptr Address to write reference to target row
 * 
 * Sets \c row_ptr to the address of the datablock containing a tag row.
 * 
 * On error, the \c row_ptr will be set to \c NULL.
 * 
 * Possible return codes:
 *  - ::TT_SUCCESS
 *  - ::TT_ERR_INVALID (input in invalid, or \c tt is \c NULL or corrupted)
 */
int tt_getrow(tag_table *tt, int row, char **row_ptr) {
    
    if (tt == NULL || tt->table == NULL || row >= tt->rows) {
        *row_ptr = NULL;
        return TT_ERR_INVALID;
    }
    
    *row_ptr = GET_ROW_FROM_TT(tt, row);
    
    return TT_SUCCESS;
}

/*!
 * \brief Returns the number of bits set within a row
 * \ingroup TTABLE
 * \param[in] tt Pointer to Tag Table
 * \param[in] row Target row within Table (starts from 0)
 * \param[out] num_ptr Address to write results in
 * 
 * Sets \c num_ptr to the number of bits set within a given row.
 * 
 * On error, the \c num_ptr will remain unchanged
 * 
 * To calculate the number of bits set in each byte, we take make a compromise
 * between looping thru the bits (slow, but no extra mem required) and a full
 * 256-int lookup table (fast, but needs extra 256 bytes). We use a lookup
 * table representing 2 bytes and sum the bits of the byte in four chunks.
 * 
 * Possible return codes:
 *  - ::TT_SUCCESS
 *  - ::TT_ERR_INVALID (input in invalid, or \c tt is \c NULL or corrupted)
 */
int tt_getcount_row(tag_table *tt, int row, int *num_ptr) {
    
    int i;
    int count;
    char *row_ptr;
    char byte; 
    const unsigned int blt[] = {0,1,1,2};
    
    if (tt == NULL || tt->table == NULL || row >= tt->rows) {
        return TT_ERR_INVALID;
    }
    
    count = 0;
    row_ptr = GET_ROW_FROM_TT(tt, row);
    
    for (i = 0; i < (int)tt->row_size; i++)
    {
        byte = *(row_ptr + i);
        count += blt[(int)(byte & 0x03)] +
                 blt[(int)(byte>>2 & 0x03)] +
                 blt[(int)(byte>>4 & 0x03)] +
                 blt[(int)(byte>>6 & 0x03)];
    }
    
    *num_ptr = count;
    
    return TT_SUCCESS;
}


/*!
 * \brief Returns the number of bits set within a column
 * \ingroup TTABLE
 * \param[in] tt Pointer to Tag Table
 * \param[in] col Target column within Table (starts from 0)
 * \param[out] num_ptr Address to write results in
 * 
 * Sets \c num_ptr to the number of bits set within a given row.
 * 
 * On error, the \c num_ptr will remain unchanged
 * 
 * Possible return codes:
 *  - ::TT_SUCCESS
 *  - ::TT_ERR_INVALID (input in invalid, or \c tt is \c NULL or corrupted)
 */
int tt_getcount_col(tag_table *tt, int col, int *num_ptr) {

    int i, w, b;
    int count = 0;
    char *row_ptr, window;
    
    if (tt == NULL || tt->table == NULL || col >= tt->cols) {
        return TT_ERR_INVALID;
    }
    
    /* inspect columb for each row */
    for (i = 0; i < tt->rows; i++)
    {
        row_ptr = GET_ROW_FROM_TT(tt, i);
        
        /* get byte window column resides in */
        w = (int)(col / 8);
        window = *(row_ptr + w);
        
        /* calculate bit index within window */
        b = col % 8;
        
        /* check if target bit is set */
        if (MSB_IS_SET(window << b)) count++;
    }
    
    *num_ptr = count;
    
    return TT_SUCCESS;
}
