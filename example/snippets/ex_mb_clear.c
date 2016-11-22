/* some function somewhere */
void func_semut(void) {
    
    MBt_Board myboard;
    
    /* board created */
    rc = MB_Create(&myboard, sizeof(myMessageType));
    
    /* .... more code that uses the board .... */
    
    /* clear the board */
    if ( MB_Clear(myboard) != MB_SUCCESS )
    {
        fprintf(stderr, "Could not clear message board\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }

    /* ... board can be reused here ... */
    /* Don't forget to delete the board when done */
    
}
