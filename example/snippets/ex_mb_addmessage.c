int rc;
MBt_Board myboard;
myMessageType staticMsg;
myMessageType dynamicMsg;
/* .... more code .... */

rc = MB_Create(&myboard, sizeof(myMessageType));
/* .... more code .... */

/* create messages to add to board */
staticMsg.value = 200;
dynamicMsg = (myMessageType *)malloc(sizeof(myMessageType));
dynamicMsg->value = 100;

if ( MB_AddMessage(myboard, (void *)&staticMsg) != MB_SUCCESS )
{
    fprintf(stderr, "Error adding message to board\n");
}

if ( MB_AddMessage(myboard, (void *)dynamicMsg) != MB_SUCCESS )
{
    fprintf(stderr, "Error adding message to board\n");
}

/* it is safe to deallocate message memory once it has been added to message board */
free(dynamicMsg);
