int i, rc;
int count = 5;
int *flag_array;
MBt_Board *mb_array;
myMessageType staticMsg;

/* Allocate memory for mb and flag array */
flag_array = (int *)malloc(sizeof(int) * count);
mb_array = (MBt_Board *)malloc(sizeof(MBt_Board) * count);

/* .... more code .... */

for (i = 0; i < count; i++)
{
    rc = MB_Create(&mb_array[i], sizeof(myMessageType));
    /* ... perhaps add messages to board ... */
}
/* .... more code .... */


if ( MB_SyncStartGroup(mb_array, count) != MB_SUCCESS )
{
    fprintf(stderr, "Unable to begin synchronisation\n");
    /* Handle error */
}

/* check if synchronisation has completed */
rc = MB_SyncTestGroup(mb_array, flag_array, count);
for (i = 0; i < count; i++)
{
    if (flag_array[i] == MB_TRUE) 
    {
        fprintf(stderr, "synchronisation of board %d has completed\n", i);
    }
    else
    {
        fprintf(stderr, "synchronisation of board %d still in progress\n", i);
        /* perhaps do something else first? */
    }
}

if ( MB_SyncCompleteGroup(mb_array, count) != MB_SUCCESS ) /* wait till sync done */
{
    fprintf(stderr, "Unable to begin synchronisation\n");
    /* Handle error */
}

/* Now create Iterators to read messages from board */

/* .... rest of program .... */

free(mb_array);
free(flag_array);
