/* $Id: circles.h 937 2008-06-06 16:49:54Z lsc $ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : circles.h
 *  
 */

/* splint directive to allow m_* names */
/*@-macrovarprefixexclude*/

#ifndef CIRCLES_H_
#define CIRCLES_H_

#include "mboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* we use EXPAT (http://expat.sourceforge.net) for parsing XML */
#include "expat.h"

#ifdef _PARALLEL
    #include <mpi.h>
    #define PRINT if(0==env_rank)printf 
#else
    #define PRINT printf
#endif /* _PARALLEL */


#define getTime() ((double)clock() / CLOCKS_PER_SEC)

/* ------ Constants ------------------- */

#define OK   1
#define FAIL 0
#define BUFSIZE 256

#define XMIN 0
#define XMAX 1
#define YMIN 2
#define YMAX 3

/* for simplicity, hardcode input data */
#define INPUT_FILE "0.xml"
#define OUTFILE_SUFFIX "-out.xml"
#define ITERATIONS 100
#define PROP_AGENT_TAG 0

enum states {
    TAG_OUTSIDE, /* outside agent related tags */
    TAG_CIRCLE,  /* within Circle tag */
    TAG_NAME,    /* within name tag */
    TAG_ID,      /* within id tag */
    TAG_X,       /* within x tag */
    TAG_Y,       /* within y tag */
    TAG_FX,      /* within fx tag */
    TAG_FY,      /* within fy tag */
    TAG_RADIUS   /* within radius tag */
};


/* ---- data structures ----- */

/* Circle agent object */
struct agent_obj {
    int id;
    double x;
    double y;
    double fx;
    double fy;
    double radius;
};

/* linked-list node for referencing Circle agent */
struct agent_node {
    struct agent_obj *agent;
    struct agent_node *next;
};

/* data strucy used for parsing input XML file */
struct controlData {
    int  index;
    char content[BUFSIZE];
    enum states state;
    int id;
    double x, y, fx, fy, radius;
};

/* location message */
struct m_Location {
    int id;
    double x;
    double y;
};

/* force message */
struct m_Force {
    int id;
    double fx;
    double fy;
};



/* -------- Global variables (defined in main.c) ----- */
extern int iter;
extern int env_rank;
extern int env_size;
extern int env_agent_count;
extern double (*partData)[4];
extern struct agent_node *agent_list;
extern struct agent_obj *current_agent;

extern MBt_Board b_Location, b_Force;
extern MBt_Iterator i_Location, i_Force;
extern MBt_Function f_range;
extern struct rangeFuncParams fp_range;



/* --------------- function prototypes ---------------- */

/* -- from utils.c -- */
int setup_env(int * argc, char *** argv);
int shutdown_env(void);
void complain_then_quit(char * msg);
void print_env_data(void);

/* -- from partition.c -- */
int partition(void);

/* -- from agent_mgmt.c -- */
int addAgent(int id, double x, double y, double fx, double fy, double radius);
int freeAllAgents(void);
int loadAgents(void);
struct agent_obj* getNextAgent(void);
int propagate_agents(void);

#define agent_get_id()     current_agent->id
#define agent_get_x()      current_agent->x
#define agent_get_y()      current_agent->y
#define agent_get_fx()     current_agent->fx
#define agent_get_fy()     current_agent->fy
#define agent_get_radius() current_agent->radius

#define agent_set_id(X)     current_agent->id = X
#define agent_set_x(X)      current_agent->x = X
#define agent_set_y(X)      current_agent->y = X
#define agent_set_fx(X)     current_agent->fx = X
#define agent_set_fy(X)     current_agent->fy = X
#define agent_set_radius(X) current_agent->radius = X

#define LOOP_THRU_AGENTS for (current_agent = getNextAgent(); \
                              current_agent != NULL; \
                              current_agent = getNextAgent())

/* -- from io.c -- */
int parseXML(const char *filename,
             XML_StartElementHandler startElemCallBackFunc,
             XML_EndElementHandler endElemCallBackFunc,
             XML_CharacterDataHandler charDataCallbackFunc,
             void *userData);
int writeOutputXML(void);

/* -- from messages.c -- */
int createBoards(void);
int deleteBoards(void);
int registerFunctions(void);
int assignFunctionsToBoards(void);
int addMessage_Location(int id, double x, double y);
int addMessage_Force(int id, double fx, double fy);
struct m_Location* getMessage_Location(void);
struct m_Force* getMessage_Force(void);
void randomiseMessages_Location(void);
void randomiseMessages_Force(void);


/* -- from agent_functions.c -- */
void af_postLocation(void);
void af_calculateForces(void);
void af_updatePosition(void);

/* -- from stats.c -- */
void calculate_stats(void);

/* -- from filters.c -- */
int filterWithinRange(const void *msg, const void *params);
void generate_filterfunc_params(void);
struct filterWithinRangeParams {
    double x,y,range;
};
struct rangeFuncParams {
    double xmin, xmax, ymin, ymax;
};

/* -- from debug.c -- */
void PRINT_DEBUG_INFO(void);

#endif /*CIRCLES_H_*/
