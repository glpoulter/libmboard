/* $Id: partition.c 738 2008-03-12 16:14:41Z lsc $ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : partition.c
 *  
 * Assumtions we make in this code:
 * a) input file defined by INPUT_FILE is valid XML for Circles Model
 * b) Only one type of agent in input (Circles)
 * c) x and y tags exist
 * 
 * Note: partitioning algorithm adapted from FLAME framework code written
 * by C.Greenough (STFC).
 */

#include "circles.h"
#include "low_primes.h"
#include <float.h> /* for DBL_MAX and DBL_MIN */
#include <string.h>




double maxX, maxY, minX, minY;
static int factors[30];
static int powers[30];
static int nfactor=0;


static void startElemCallback(void *userData, const  XML_Char*name, const XML_Char **atts) {
    
    struct controlData *data = (struct controlData *)userData;
    
    switch (data->state) {
        case TAG_OUTSIDE:
            if (0 == strcmp((const char *)name, "x"))
            {
                data->state = TAG_X;
                data->index = 0; /* reset index */
                data->content[0] = '\0';
            }
            else if (0 == strcmp((const char *)name, "y"))
            {
                data->state = TAG_Y;
                data->index = 0; /* reset index */
                data->content[0] = '\0';
            }
            break;
            
        default:
            /* if we are within X or Y, there should not be any subtags */
            printf("** Corrupted input data?");
            break;
    }
}

static void endElemCallback(void *userData, const XML_Char *name)
{
    struct controlData *data = (struct controlData *)userData;
    double value;
    
    switch (data->state) {
        
        case TAG_OUTSIDE:
            break;
            
        case TAG_X:
            data->content[data->index] = '\0'; /* end the string */
            
            /* convert string to double and process */
            value = atof(data->content);
            if (value > maxX) maxX = value;
            if (value < minX) minX = value;
            
            /* revert state */
            data->state = TAG_OUTSIDE;
            
            break;
            
        case TAG_Y:
            data->content[data->index] = '\0'; /* end the string */
            
            /* convert string to double and process */
            value = atof(data->content);
            if (value > maxY) maxY = value;
            if (value < minY) minY = value;
            
            /* revert state */
            data->state = TAG_OUTSIDE;
            
            break;
        
        default: break;
    }
}

static void charDataCallback(void *userData, const XML_Char* s, int len) {
    int i;
    struct controlData *data = (struct controlData *)userData;
    
    for (i = 0; i < len; i++)
    {
        if (data->index >= BUFSIZE - 1) /* don't for get space for '\0' */
        {
            printf("WARNING: data will truncated. Please increase BUFSIZE\n");
            return;
        }
        
        if (s[i] == '<') break;
        
        data->content[data->index++] = (char)s[i];
    }
}

static int getMinMax() {
    
    struct controlData data;
    
    /* initialise data */
    data.index = 0;
    data.state = TAG_OUTSIDE;
        
    return parseXML(INPUT_FILE, startElemCallback, 
                    endElemCallback, charDataCallback, &data);
    
}

static void collect_factors( long fact, long power )
{
    factors[nfactor] = fact;
    powers[nfactor]  = power;
    nfactor += 1;
}

static int test_fact( long* numP, long fact )
{
    long power, t;

    power = 0;
    while ( ((t = *numP / fact) * fact) == *numP )
    {
        power++;
        *numP = t;
    }

    if ( power != 0 ) collect_factors(fact, power );

    if ( t > fact ) return 1;

    return 0;
}

static void factor( long num )
{
    int p;
    long lnum, fact;

    lnum = num;

    if ( lnum == 0 || lnum == 1 ) {
        /* If it's a simple factorisation */
        collect_factors( lnum, 1 );
    }
    else 
    {
        /* Loop over the lower prime numbers till we have the factors.*/
        for ( p = 0; p < (int)sizeof(low_primes)/(int)sizeof(*low_primes); ++p )
            if ( ! test_fact( &lnum, low_primes[p] ) ) goto done;
        /* If we didn't find any factors generate some more on the fly. */
        fact = ( low_primes[p - 1] + 5 ) / 6 * 6 - 1;
        for ( ; ; )
            {
            if ( ! test_fact( &lnum, fact ) )
            break;
            fact += 2;
            if ( ! test_fact( &lnum, fact ) )
            break;
            fact += 4;
            }
        done:
            if ( lnum != 1 )
                collect_factors( lnum, 1 );
    }
}

int partition(void) {
    
    int xdiv = 1, ydiv = 1;
    int i, j, id;

    double dx,dy,px,py;
    double xlo, xhi, ylo, yhi;
    double margin=0.25; /* small extension of region */
    
    if (0 == env_rank) 
    {
        /* get domain size */
        if (getMinMax() != OK)
        {
            #ifdef _PARALLEL
                fprintf(stderr, "Fatal Error: could not read input file\n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            #endif
            
            return FAIL;
        }
        
        /* calculate partitions for each node */
        if (env_size > 1)
        {
            /* factor number of partitions */
            factor((long) env_size);
            
            /* build divisions for x and y axes */
            for (i = nfactor-1; i >= 0; i--) {
                for(j = 1; j <= powers[i] ; j++)
                {
                    if (xdiv <= ydiv) xdiv = xdiv * factors[i];
                    else ydiv = ydiv * factors[i];
                }
            }
            
            /* defined agent cloud with small margin */ 
            maxX += maxX * margin;
            maxY += maxY * margin;
            minX -= minX * margin;
            minY -= minY * margin;
            
            /* calculate increments */
            dx = (maxX - minX) / xdiv;
            dy = (maxY - minY) / ydiv;
            
            /* generate partitions */
            id = 0;
            px = minX;
            for (i = 0; i < xdiv; i++)
            {
                py = minY;
                for (j = 0; j < ydiv; j++)
                {
                    xlo = px;
                    xhi = px + dx;
                    ylo = py;
                    yhi = py + dy;
                    
                    if (xlo <= minX) xlo = -DBL_MAX;
                    if (ylo <= minY) ylo = -DBL_MAX;
                    if (xhi >= maxX) xhi = DBL_MAX;
                    if (yhi >= maxY) yhi = DBL_MAX;
                    
                    partData[id][XMIN] = xlo;
                    partData[id][XMAX] = xhi;
                    partData[id][YMIN] = ylo;
                    partData[id][YMAX] = yhi;
                    
                    id++;
                    py += dy;
                }
                px += dx;
            }
        }
        else
        {
            partData[0][XMIN] = -DBL_MAX;
            partData[0][XMAX] = DBL_MAX;
            partData[0][YMIN] = -DBL_MAX;
            partData[0][YMAX] = DBL_MAX; 
        }
        
        
        
    }
    
    #ifdef _PARALLEL
        /* distribute partition data to all nodes */
        MPI_Bcast(partData, env_size * 4, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    #endif /* _PARALLEL */
    
    return OK;
}

