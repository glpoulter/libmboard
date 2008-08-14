/* some function somewhere */
void func_belut(void) {
    
    MBt_Board myboard;
    
    /* board created */
    rc = MB_Create(&myboard, sizeof(myMessageType));
    
    /* .... more code that uses the board .... */
    
    /* when done, delete the board */
    if ( MB_Delete(&myboard) != MB_SUCCESS )
    {
        fprintf(stderr, "Could not delete message board\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }
    
    /* ... more code ... */
}
