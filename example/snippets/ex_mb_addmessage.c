/* some function somewhere */
void func_kucing(void) {

    int rc;
    MBt_Board myboard;
    myMessageType staticMsg;
    myMessageType dynamicMsg;

    /* create board to store myMessageType messages */
    rc = MB_Create(&myboard, sizeof(myMessageType));
    
    /* create messages to add to board */
    staticMsg.value = 200;
    dynamicMsg = (myMessageType *)malloc(sizeof(myMessageType));
    dynamicMsg->value = 100;
    
    if ( MB_AddMessage(myboard, (void *)&staticMsg) != MB_SUCCESS )
    {
        fprintf(stderr, "Error adding message to board\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }
    
    if ( MB_AddMessage(myboard, (void *)dynamicMsg) != MB_SUCCESS )
    {
        fprintf(stderr, "Error adding message to board\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }
    
    /* it is safe to modify message memory once it has been added to the board.
     * Value in the board will not be modified.
     */
    staticMsg.value = 42;
    
    /* it is safe to deallocate message memory once it has been added to message board */
    free(dynamicMsg);

    /* ... more code ... */
}
