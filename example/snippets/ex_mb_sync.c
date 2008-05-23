int rc;
int flag;
MBt_Board myboard;
myMessageType staticMsg;
/* .... more code .... */

rc = MB_Create(&myboard, sizeof(myMessageType));
/* .... more code .... */

/* create messages to add to board */
staticMsg.value = 200;


if ( MB_AddMessage(myboard, (void *)&staticMsg) != MB_SUCCESS )
{
    fprintf(stderr, "Error adding message to board\n");
}

if ( MB_SyncStart(myboard) != MB_SUCCESS )
{
    fprintf(stderr, "Unable to begin synchronisation\n");
    /* Handle error */
}

/* check if synchronisation has completed */
MB_SyncTest(myboard, &flag);
if (flag == MB_TRUE) 
{
    fprintf(stderr, "synchronisation has completed\n");
    process_message_board();
}
else
{
    fprintf(stderr, "synchronisation still in progress\n");
    
    do_something_else_first();
    
    if ( MB_SyncComplete(myboard) != MB_SUCCESS ) /* wait till sync done */
    {
        fprintf(stderr, "Unable to begin synchronisation\n");
        /* Handle error */
    }
    
    process_message_board();
}

/* .... rest of program .... */
