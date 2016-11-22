/* our message datatype */
typedef struct {
    int id;
    double price;
    double value;
} myMessageType;

/* to be used for filtering myMessageType  */
int myFilter(const void *msg, int pid) {
    myMessageType *m;

    /* cast data to proper type */
    m = (myMessageType*)msg;

    if (m->price < 0) return 0; /* reject */
    else return 1; /* accept */

}
    
/* some function somewhere */
void func_beruang(void) {

    int rc;
    MBt_Filter f_handle;
    
    /* register the function */
    rc = MB_Fitler_Create(&f_handle, &myFilter);
    if ( rc != MB_SUCCESS )
    {
        fprintf(stderr, "Error while creating filter\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }
    
    /* assign function to board, assuming myboard has been created */
    rc = MB_Filter_Assign(mboard, f_handle);
    if ( rc != MB_SUCCESS )
    {
        fprintf(stderr, "Error while assigning filter to board\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }
    
    /* ... more code that adds messages to myboard ... */
    
    /* assign params for filtering messages during sync */
    MB_SyncStart(myboard); /* you should check the return code */
    do_something_else();
    MB_SyncComplete(myboard); /* you should check the return code */
    
    /* we should now have messages from other processing nodes, but
     * only those that passes the filter function myFilter()
     */
    
    /* ... do stuff ... */
    
    rc = MB_Filter_Delete(&f_handle);
    if ( rc != MB_SUCCESS )
    {
        fprintf(stderr, "Error while deleting filter\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }
    
}
