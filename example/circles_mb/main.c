/* $Id: main.c 1141 2008-08-08 09:17:23Z lsc $ */
/*
 * Author: Lee-Shawn Chin 
 * Date  : Feb 2008 
 * Copyright (c) 2008 STFC Rutherford Appleton Laboratory
 * 
 * File : main.c
 *  
 */

#include "circles.h"

/* define and initialise global vars */
int iter     = 0;
int env_rank = 0;
int env_size = 1;
int env_agent_count = 0;
double (*partData)[4];
struct agent_node *agent_list = NULL;
struct agent_obj *current_agent = NULL;

MBt_Board b_Location = MB_NULL_MBOARD;
MBt_Board b_Force    = MB_NULL_MBOARD;
MBt_Iterator i_Location = MB_NULL_ITERATOR;
MBt_Iterator i_Force    = MB_NULL_ITERATOR;
MBt_Function f_range = MB_NULL_FUNCTION;
struct rangeFuncParams fp_range;

int main(int argc, char ** argv) {
    
    /* timer vars */
    double time_total_start, time_total_end;
    double time_load_start, time_load_end;
    double time_write_start, time_write_end;
    double time_iter_start, time_iter_end;
    double time_cleanup_start, time_cleanup_end;
    
    /* parameter data for filters */
    struct filterWithinRangeParams af_calculateForcesPrams;
    
    time_total_start = getTime();
    
    /* Set up the message board environment */
    if (setup_env(&argc, &argv) != OK)
    {
        complain_then_quit("Could not set up libmoard environment\n");
    }
    
    time_load_start = getTime();
    /* set up partitions */
    if (partition() != OK) 
    {
        complain_then_quit("Error setting up partitions\n");
    }
    
    if (loadAgents() != OK)
    {
        complain_then_quit("Error loading agents from file\n");
    }
    time_load_end = getTime();
    
    print_env_data();
    
    /* ======================== SIM LOOP ========================= */
    time_iter_start = getTime();
    for (iter = 0; iter < ITERATIONS; iter++)
    {
        PRINT("== Starting iteration %d ==\n", iter);
        
        /* post location */
        LOOP_THRU_AGENTS { af_postLocation(); }
        
        generate_filterfunc_params(); /* populate params used by reg. func */
        MB_SyncStart(b_Location); /* "Location" message was sent */
        
        /* ------------ calculate forces --------------------- */

        
        /* "Location" message required soon */
        MB_SyncComplete(b_Location);         

        /* run function for each agent */
        LOOP_THRU_AGENTS 
        {
            /* prep parameters for filter function */
            af_calculateForcesPrams.x = agent_get_x();
            af_calculateForcesPrams.x = agent_get_x();
            af_calculateForcesPrams.range = 2 * agent_get_radius();
            
            /* create filtered iterator */
            MB_Iterator_CreateFiltered(b_Location, &i_Location, 
                        &filterWithinRange, &af_calculateForcesPrams);

            /* run function */
            af_calculateForces(); 
            
            /* delete iterator */
            MB_Iterator_Delete(&i_Location);
        }
        MB_SyncStart(b_Force); /* "Force" message was sent */
        
        MB_Clear(b_Location);
        
        
        /* Update position */
        LOOP_THRU_AGENTS { af_updatePosition(); }

        
        /* gather stats */
        MB_SyncComplete(b_Force); /* "Force" message required soon */
        
        /* propagate agents */
        propagate_agents();
        
        MB_Iterator_Create(b_Force, &i_Force);
        calculate_stats();
        MB_Iterator_Delete(&i_Force);
        MB_Clear(b_Force);
    }
    time_iter_end = getTime();
    /* ============================================================ */
    
    time_write_start = getTime();
    if (writeOutputXML() != OK)
    {
        fprintf(stderr, "ERROR: Could not open output file for writing\n");
    }
    time_write_end = getTime();
    
    time_cleanup_start = getTime();
    freeAllAgents();
    
    /* clean up and quit */
    if (shutdown_env() != OK) exit(1);
    
    time_cleanup_end = getTime();
    time_total_end = getTime();
    
    /* Print timing data */
    PRINT("\n=============== TIMERS ============== \n");
    PRINT("partition() + loadAgents()       = %.3f seconds\n", time_load_end - time_load_start);
    PRINT("Iterations                       = %.3f seconds\n", time_iter_end - time_iter_start);
    PRINT("writeOutputXML()                 = %.3f seconds\n", time_write_end - time_write_start);
    PRINT("freeAllAgents() + shutdown_env() = %.3f seconds\n", time_write_end - time_write_start);
    PRINT("Total (include other overheads)  = %.3f seconds\n", time_total_end - time_total_start);
    
    exit(0);
}
