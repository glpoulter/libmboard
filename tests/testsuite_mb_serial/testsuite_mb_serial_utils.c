/* $Id: testsuite_mb_serial_utils.c 1871 2009-06-12 13:58:18Z lsc $ */
/* 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008
 * 
 */

#include "header_mb_serial.h"

static int _cmp_func(const void* a, const void* b);
static void _randomise_array(int *array, int size);

/* map message sequence ID to value */
int get_message_value_from_id(int id) {
    /* return a value for message such that it is predictable,
     * yet capable for exercising the sorting/filtering
     * functionalities of Iterators
     */
    
    int index, ub;
    
    if ((id % 2) == 0) /* even nums */
    {
        index = (int)(id / 2);
        ub = SERIAL_TEST_MSG_COUNT;
    }
    else /* odd nums */
    {
        index = (int)((id - 1) / 2);
        ub = (int)(SERIAL_TEST_MSG_COUNT / 2);
    }
    
    return ub - index;
    
    
}

/* initialise message board with content */
int init_mb_with_content(MBt_Board *mb_ptr) {
    
    int i, rc;
    dummy_msg msg;
    
    /* Create Message Board */
    rc = MB_Create(mb_ptr, sizeof(dummy_msg));
    if (rc != MB_SUCCESS) return rc; /* abort on failure */
    
    /* populate board with messages */
    for (i = 0; i < SERIAL_TEST_MSG_COUNT; i++)
    {
        msg.ernet = get_message_value_from_id(i);
        msg.jeopardy = (double)get_message_value_from_id(i);
        rc = MB_AddMessage(*mb_ptr, (void *)&msg);
        
        /* stop on failure */
        if (rc != MB_SUCCESS) break;
    }
    
    return rc;
    
}

void generate_random_unique_ints(int *array, int size) {
    
    int i, prev;
    int dups;
    
    /* give initial values to array */
    for (i = 0; i < size; i++) array[i] = rand();
    
    dups = 999; /* any non-zero value will do */
    while (dups != 0)
    {
        /* reset value */
        dups = 0;
        
        /* sort the values so duplicate values are adjacent to each other */
        qsort((void*)array, (size_t)size, sizeof(int), &_cmp_func);
        
        /* replace duplicates with random values */
        prev = array[0];
        for (i = 1; i < size; i++)
        {
            if (array[i] == prev)
            {
                array[i] = rand();
                dups++;
            }
            else prev = array[i];
        }
    }
    
    /* randomise the array */
    _randomise_array(array, size);
}

int is_in_array(int *array, int size, int value) {
    
    int i;
    
    for (i = 0; i < size; i++)
    {
        if (array[i] == value) return (1==1);
    }
    
    return (1 == 0);
}

static void _randomise_array(int *array, int size) {
    
    int i, temp, rnd;
    double rnd_ratio;
    
    rnd_ratio = 1.0 / (RAND_MAX + 1.0); /* ratio to scale random numbers */
    for (i = size - 1; i > 0; i--)
    {
        /* get a random number from 0 to i */
        rnd = (int)(rnd_ratio * (i) * rand());
        
        if (rnd == i) continue; /* this value stays in place */
        
        /* perform swap */
        temp = array[i];
        array[i] = array[rnd];
        array[rnd] = temp;
    }
}

static int _cmp_func(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}
