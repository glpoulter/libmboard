/* $Id: testsuite_avl_utils.c 1871 2009-06-12 13:58:18Z lsc $ */
/* 
 * Copyright (c) 2009 STFC Rutherford Appleton Laboratory 
 * Author: Lee-Shawn Chin 
 * Date  : May 2009
 * 
 */
#include "header_avltree.h"
#include <stdio.h>

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

struct tree_stats {
    int count;
    int depth;
};

static int _cmp_func(const void* a, const void* b);
static void _randomise_array(int *array, int size);
static void _walk_nodes(MBIt_AVLnode *node, struct tree_stats *stat);
static int expected_max_depth(int nodes); 

void check_tree_integrity(MBIt_AVLtree *tree) {
    
    struct tree_stats stat;
    
    CU_ASSERT_PTR_NOT_NULL(tree);
    if (tree == NULL) return;
    
    /* _walk_nodes routine depth calculation starts at 1 (root) */
    _walk_nodes(tree->root, &stat);
    stat.depth -= 1; /* adjust value to get "actual" depth */
    
    CU_ASSERT_EQUAL(tree->count, stat.count);
    CU_ASSERT(stat.depth <= expected_max_depth(tree->count));
}

static int expected_max_depth(int nodes) {
    /* Not all math.h provide log2() */
    /* return (int)ceil(log2((double)nodes)) + 1; */
    return (int)ceil(log((double)nodes)/log(2.0)) + 1;
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

static void _walk_nodes(MBIt_AVLnode *node, struct tree_stats *stat) {
    
    struct tree_stats lstat, rstat;
    
    lstat.count = rstat.count = 0;
    lstat.depth = rstat.depth = 0;
    
    if (node != NULL)
    {        
        
        _walk_nodes(node->child[AVL_LEFT], &lstat);
        _walk_nodes(node->child[AVL_RIGHT], &rstat);
        
        stat->count = lstat.count + rstat.count + 1;
        stat->depth = 1 + (MAX(lstat.depth, rstat.depth));
    }
    else
    {
        stat->depth = -1;
    }
}

void print_node(MBIt_AVLnode *node) {
    
    if (node == NULL) return;
    
    printf("\n * %d (%d)\n", node->key, node->balance);
    if (node->child[AVL_LEFT] != NULL)
    {
        printf(" --> L %d\n", node->child[AVL_LEFT]->key);
    }
    if (node->child[AVL_RIGHT] != NULL)
    {
        printf(" --> R %d\n", node->child[AVL_RIGHT]->key);
    }
    
    print_node(node->child[AVL_LEFT]);
    print_node(node->child[AVL_RIGHT]);
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
