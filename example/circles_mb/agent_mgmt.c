/* $Id$ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : agent_mgmt.c
 *  
 */

#include "circles.h"
#include <string.h>

int addAgent(int id, double x, double y, 
              double fx, double fy, double radius) {
    
    struct agent_node *node;
    
    /* create list node */
    node = (struct agent_node *)malloc(sizeof(struct agent_node));
    if (node == NULL) return FAIL;
    
    /* create agent */
    node->agent = (struct agent_obj *)malloc(sizeof(struct agent_obj));
    if (node->agent == NULL)
    {
        free(node);
        return FAIL;
    }
    
    /* populate with data */
    node->agent->id = id;
    node->agent->x = x;
    node->agent->y = y;
    node->agent->fx = fx;
    node->agent->fy = fy;
    node->agent->radius = radius;
    
    /* append to head of list */
    node->next = agent_list;
    agent_list = node;
    
    env_agent_count++;
    
    return OK;
}

int freeAllAgents(void) {
    
    struct agent_node *target, *temp;
    
    for (target = agent_list; target; target = temp)
    {
        temp = target->next;
        free(target->agent);
        free(target);
    }
    
    agent_list = NULL;
    env_agent_count = 0;
    return OK;
}

static void startElemCallback(void *userData, const  XML_Char*name, const XML_Char **atts) {
    
    struct controlData *data = (struct controlData *)userData;
    
    switch (data->state) {
        case TAG_OUTSIDE:
            if (0 == strcmp((const char *)name, "xagent"))
            {
                data->state = TAG_CIRCLE;
            }
            break;
        case TAG_CIRCLE:
            /* reset char data */
            data->index = 0; 
            data->content[0] = '\0';
            
            /* change to inner state */
            if (0 == strcmp((const char *)name, "radius")) data->state = TAG_RADIUS;
            else if (0 == strcmp((const char *)name, "x")) data->state = TAG_X;
            else if (0 == strcmp((const char *)name, "y")) data->state = TAG_Y;
            else if (0 == strcmp((const char *)name, "fx")) data->state = TAG_FX;
            else if (0 == strcmp((const char *)name, "fy")) data->state = TAG_FY;
            else if (0 == strcmp((const char *)name, "id")) data->state = TAG_ID;
            else if (0 == strcmp((const char *)name, "name")) data->state = TAG_NAME;
            break;
            
        default: break;
    }
}

static void endElemCallback(void *userData, const XML_Char *name)
{
    struct controlData *data = (struct controlData *)userData;
    
    switch (data->state) {
        
        case TAG_OUTSIDE:
            break;
            
        case TAG_CIRCLE:
            /* if within partition, add agent */
            if (data->x >= partData[env_rank][XMIN] &&
                data->x <  partData[env_rank][XMAX] &&
                data->y >= partData[env_rank][YMIN] &&
                data->y <  partData[env_rank][YMAX] )
            {
                addAgent(data->id, data->x, data->y,
                        data->fx, data->fy, data->radius);
            }
            
            /* change state */
            data->state = TAG_OUTSIDE;
            break;
            
        default:
            data->content[data->index] = '\0'; /* end the string */
            
            /* convert string to double and process */
            switch (data->state) {
                case TAG_X: data->x = atof(data->content); break;
                case TAG_Y: data->y = atof(data->content); break;
                case TAG_FX: data->fx = atof(data->content); break;
                case TAG_FY: data->fy = atof(data->content); break;
                case TAG_ID: data->id = atoi(data->content); break;
                case TAG_RADIUS: data->radius = atof(data->content); break;
                case TAG_NAME: 
                    if (0 != strcmp((const char *)data->content, "Circle"))
                    {
                        printf("** Found unexpected agent (%s)\n", data->content);
                    }
                    break;
                default:
                    printf("** Corrupted input data ? (%d)\n", data->state);
                    break;
            }
           
            /* change state */
            data->state = TAG_CIRCLE;            
            break;

    }
}

static void charDataCallback(void *userData, const XML_Char* s, int len) {
    int i;
    struct controlData *data = (struct controlData *)userData;
    
    if (data->state == TAG_OUTSIDE || data->state == TAG_CIRCLE) return;
    
    for (i = 0; i < len; i++)
    {
        if (data->index >= BUFSIZE - 1) /* don't for get space for '\0' */
        {
            printf("WARNING: data will truncated. Please increase BUFSIZE\n");
            printf("{ %s }\n", s);
            return;
        }
        
        if (s[i] == '<') break;
        
        data->content[data->index++] = (char)s[i];
    }
}

int loadAgents(void) {
    
    struct controlData data;
    
    /* initialise data */
    data.index = 0;
    data.state = TAG_OUTSIDE;
        
    return parseXML(INPUT_FILE, startElemCallback, 
                    endElemCallback, charDataCallback, &data);
}

struct agent_obj* getNextAgent(void) {

    static int start = 1;
    static struct agent_node *node = NULL;
    struct agent_obj *agent;
    
    if (1 == start) /* begin new traversal */
    {
        if (agent_list == NULL) return NULL; /* no agents */
        
        node = agent_list;
        start = 0;
    }
    else if (node == NULL) /* reached end */
    {
        start = 1; /* rewind */
        return NULL;
    }
    
    agent = node->agent;
    node  = node->next;

    return agent;
}

int propagate_agents(void) {
    
    int status = OK;
    
    /* agent propagation only needed for parallel runs */
    #ifndef _PARALLEL
        return status;
    #else
       
    /* ------------------------------------------------ */
    
    /* more variable definitions */
    MPI_Request *in_req, *out_req;
    double xmax, xmin, ymax, ymin, x, y;
    struct agent_obj *a_obj;
    struct agent_node **outbox;
    struct agent_node *prev, *current, *toMove;
    void **outbuf, **inbuf;
    int *outbox_count;
    int *inbox_count;
    int i, j, bufsize, from, recv_count;
    
    /* nothing to propagate if only one proc */
    if (env_size < 2) return status;
    
    /* allocate required memory */
    outbox = (struct agent_node **)malloc(sizeof(struct agent_node *) * env_size);
    outbox_count = (int *)malloc(sizeof(int) * env_size);
    inbox_count  = (int *)malloc(sizeof(int) * env_size);
    outbuf = (void *)malloc(sizeof(void *) * env_size);
    inbuf  = (void *)malloc(sizeof(void *) * env_size);
    out_req = (MPI_Request *)malloc(sizeof(MPI_Request) * env_size);
    in_req  = (MPI_Request *)malloc(sizeof(MPI_Request) * env_size);
    if (!outbox || !outbox_count || !inbox_count || 
        !outbuf || !inbuf || !in_req || !out_req )
    {
        if (outbox) free(outbox);
        if (outbox_count) free(outbox_count);
        if (inbox_count) free(inbox_count);
        if (outbuf) free(outbuf);
        if (inbuf) free(inbuf);
        if (in_req) free(in_req);
        if (out_req) free(out_req);
        return FAIL;
    }
    
    /* some data initialisation */
    for (i = 0; i < env_size; i++)
    {
        outbox_count[i] = 0;
        inbox_count[i] = 0;
        outbox[i] = NULL;
        outbuf[i] = NULL;
        inbuf[i] = NULL;
    }
    xmax = partData[env_rank][XMAX];
    xmin = partData[env_rank][XMIN];
    ymax = partData[env_rank][YMAX];
    ymin = partData[env_rank][YMIN];
    
    /* determine agents that need to be migrated */
    prev = NULL;
    current = agent_list;
    while(current)
    {
        x = current->agent->x;
        y = current->agent->y;
        
        /* is agent still in partition? */
        if (x >= xmin && x < xmax && y >= ymin && y < ymax) 
        {
            current = current->next;
            continue;
        }
        
        /* this agent has to be moved */
        toMove = current;
        
        /* remove agent from main linked list */
        if (prev == NULL) agent_list = current->next;
        else prev->next = current->next;
        
        /* select next node for following iteration */
        prev = current;
        current = current->next;
        
        /* determine which partition agent has to be sent to */
        for (i = 0; i < env_size; i++)
        {
            if (i == env_rank) continue;
            
            if (x >= partData[i][XMIN] &&
                x <  partData[i][XMAX] &&
                y >= partData[i][YMIN] &&
                y <  partData[i][YMAX] )
            {
                /* add to proper outbox */
                toMove->next = outbox[i];
                outbox[i] = toMove;
                outbox_count[i]++;
                break;
            }
                        
        }
        
    }
    
    /* broadcast send counts */
    MPI_Alltoall(outbox_count, 1, MPI_INT, inbox_count,  1, MPI_INT, MPI_COMM_WORLD);
    
    /* prep input buffers and post non-blocking receives */
    recv_count = 0;
    for (i = 0; i < env_size; i++)
    {
        if (inbox_count[i] == 0)
        {
            in_req[i] = MPI_REQUEST_NULL;
            inbuf[i]  = NULL;
            continue;
        }
        
        /* allocate memory for incoming data */
        bufsize = sizeof(struct agent_obj) * inbox_count[i];
        inbuf[i] = (void *)malloc(bufsize);
        
        /* post non-blocking receive */
        MPI_Irecv(inbuf[i], bufsize, MPI_BYTE, i, PROP_AGENT_TAG, 
                  MPI_COMM_WORLD, &in_req[i]);
        
        recv_count++;
    }
    
    /* prep and populate output buffer. post non-blocking sends */
    for (i = 0; i < env_size; i++)
    {
        if (outbox_count[i] == 0)
        {
            out_req[i] = MPI_REQUEST_NULL;
            outbuf[i]  = NULL;
            continue;
        }
        
        /* allocate memory for outgoing data */
        bufsize = sizeof(struct agent_obj) * outbox_count[i];
        outbuf[i] = (void *)malloc(bufsize);
        
        /* populate outbuf */
        current = outbox[i];
        outbox[i] = NULL;
        j = 0;
        while(current)
        {
            /* copy agent data */
            memcpy((char *)outbuf[i] + (j * sizeof(struct agent_obj)),
                   current->agent, sizeof(struct agent_obj));
            j++;
            
            /* free node and proceed */
            prev = current;
            current = current->next;
            free(prev);
        }
            
        /* post non-blocking synchronous send */
        MPI_Issend(outbuf[i], bufsize, MPI_BYTE, i, PROP_AGENT_TAG, 
                   MPI_COMM_WORLD, &out_req[i]);
    }
    
    /* complete and process receives */
    for (i = 0; i < recv_count; i++)
    {
        /* wait for any receive to complete */
        MPI_Waitany(env_size, in_req, &from, MPI_STATUS_IGNORE);
        
        for (j = 0; j < inbox_count[from]; j++)
        {
            a_obj = (struct agent_obj*)((char *)inbuf[from] + (j * sizeof(struct agent_obj)));
            addAgent(a_obj->id, 
                    a_obj->x, 
                    a_obj->y, 
                    a_obj->fx, 
                    a_obj->fy, 
                    a_obj->radius); 
                    
        }
        
        free(inbuf[from]);
    }
    
    /* wait for all sends to complete */
    MPI_Waitall(env_size, out_req, MPI_STATUSES_IGNORE);
    /* free output buffers */
    for (i = 0; i < env_size; i++) if (outbuf[i] != NULL) free(outbuf[i]);
    
    /* deallocate memory */
    free(outbox);
    free(outbox_count);
    free(inbox_count);
    free(outbuf);
    free(inbuf);
    free(in_req);
    free(out_req);
    
    /* ------------------------------------------------ */
    
    #endif
    return status;
}
