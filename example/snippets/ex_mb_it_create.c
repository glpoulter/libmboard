/* some function somewhere */
void func_siput(void) {
    
    MBt_Board myboard;
    MBt_Iterator iterator;
    
    /* .... more code that creates and populate myboard .... */
    
    rc = MB_Iterator_Create(myboard, &iterator);
    if ( rc != MB_SUCCESS )
    {
        fprintf(stderr, "Error while creating Iterator\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }
    
    /* iterator ready to be used */
    /* ... more code ... */
}
