/* our message datatype */
typedef struct {
    int id;
    double price;
    double value;
} myMessageType;

/* parameter datatype for myFilter */
typedef struct {
    double minPrice;
    double maxPrice;
} myFilterParams;

/* to be used for filtering myMessageType  */
int myFilter(const void *msg, const void *params) {
    myMessageType *m;
    myFilterParams *p;

    /* cast data to proper type */
    m = (myMessageType*)msg;
    p = (myFilterParams*)params;

    if (m1->price > p->maxPrice)
    {
        return 0; /* reject */
    }
    else if (m1->price < p->minPrice) {
    {
        return 0; /* reject */
    }
    else
    {
        return 1; /* accept */
    }
}
    

/* some function somewhere */
void func_beruang(void) {

    int rc;
    myFilterParam myparam;
    MBt_Function f_handle;
    
    /* register the function */
    rc = MB_Function_Register(&f_handle, &myFilter);
    if ( rc != MB_SUCCESS )
    {
        fprintf(stderr, "Error while registering function\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }
    
    /* assign function to board, assuming myboard has been created */
    rc = MB_Function_Assign(mboard, f_handle, &myparam, sizeof(myFilterParam));
    if ( rc != MB_SUCCESS )
    {
        fprintf(stderr, "Error while assigning function to board\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }
    
    /* ... more code that adds messages to myboard ... */
    
    /* assign params for filtering messages during sync */
    myparam.minPrice = 0.8;
    myparam.maxPrice = 2.3;
    MB_SyncStart(myboard); /* you should check the return code */
    do_something_else();
    MB_SyncComplete(myboard); /* you should check the return code */
    
    /* we should now have messages from other processing nodes, but
     * only those that passes the filter function myFilter()
     */
    
    /* ... do stuff ... */
    
    rc = MB_Function_Free(&f_handle);
    if ( rc != MB_SUCCESS )
    {
        fprintf(stderr, "Error while freeing function\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }
    
}
