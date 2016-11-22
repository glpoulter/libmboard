/* $Id: stresstest.h 732 2008-03-10 15:55:53Z lsc $ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : stresstest.h
 *  
 */

#ifndef STRESSTEST_H_
#define STRESSTEST_H_

#include "mboard.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _PARALLEL
    #include <mpi.h>
    #define PRINT if(0==env_rank)printf 
#else
    #define PRINT printf
#endif

/* test params */
#define TP_MANYBOARD_COUNT 10000
#define TP_MANYMSG_COUNT   100000

#define OK   1
#define FAIL 0

extern int env_rank;
extern int env_size;


/* -- from utils.c -- */
void complain_then_quit(char * msg);
int setup_env(int * argc, char *** argv);
int shutdown_env(void);

/* tests */
int t_manyboards(void);
int t_manymsgs(void);

#endif /*STRESSTEST_H_*/
