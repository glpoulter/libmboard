/* Here's an example of a simple program that can be written using the
 * Message Board library to handle parallelisation.
 * 
 * This is, of course, a very artificial example of how libmboard can be
 * used as it does not play on the strengths the message board, which is
 * to disseminate numerous different types of data simulataneously, and
 * in an all-to-all fashion.
 * 
 * Note: Here, we use MB_Env_GetProcID() and assume that the MPI environment
 * is intialised and finalised for us by MB_Env_Init() and MB_Env_Finalise().
 * These have not be implemented, but may be so in the next version. Even if
 * they are not implemented, all we need is add a few additional calls to MPI 
 * routines for the following example to work.
 * 
 * Author: L.S. Chin (STFC)
 * Date: August 2008
 * 
 */
#include "mboard.h"
#include <stdlib.h>
#include <stdio.h>

#define NR 1000
#define FUNC_X(a) (4.0 / (1.0 + (a)*(a)))

/* this example was meant to be  brief, so we've omitted checks for 
 * Message Board routines' return code. 
 * In practice, you would want to check all return code, and handle any
 * erroneous conditions
 */
int main(int argc, char **argv) {
    
    int rc;
    void *msg;
    int id, pcount;
    MBt_Board mboard;
    MBt_Iterator iter;
    double mypi, pi, sum, h;
    
    /* Initialise Message Board environment */
    MB_Env_Init();

    /* get my ID and proc count */
    id = MB_Env_GetProcID();
    pcount = MB_Env_GetProcCount();
    
    /* Create a board that can store doubles */
    MB_Create(&mboard, sizeof(double));
    
    /* calculate my portion of pi */
    h = 1.0 / (double)NR;
    pi = sum = 0.0;
    for (i = id + 1; i <= NR; i += pcount)
    {
        sum += FUNC_X(((double)i - 0.5)*h);
    }
    
    /* Everyone adds their sum to the board */
    MB_AddMessage(mboard, &sum);
    
    /* Synchornise boards */
    /* If there are more things to do that does not involved the board
     * we can run them between SyncStart() and SyncComplete(). That will
     * overlap the communication time with your own computation.
     */ 
    MB_SyncStart(mboard);
    MB_SyncComplete(mboard);
    
    /* Master node reads message and prints out result */
    if (id == 0)
    {
        MB_Iterator_Create(mboard, &iter);
        MB_Iterator_GetMessage(iter, msg);
        while (msg)
        {
            pi += *((double *)msg);
            MB_Iterator_GetMessage(iter, msg);
        } 
        MB_Iterator_Delete(&iter);
        
        printf("Pi is approximately %.16f\n", pi);
    }
    
    /* Delete board */
    MB_Delete(&mboard);
    
    /* Finalise Message Board environment */
    return MB_Env_Finalise();
}
