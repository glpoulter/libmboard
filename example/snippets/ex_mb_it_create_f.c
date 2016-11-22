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
void func_monyet(void) {
    
    MBt_Iterator iterator;
    myFilterParam params;
    
    /* assuming myboard has been created and populated */
    
    params.minPrice = 10.5;
    params.maxPrice = 58.3;
    rc = MB_Iterator_CreateFiltered(myboard, &iterator, &myFilter, &params);
    if ( rc != MB_SUCCESS )
    {
        fprintf(stderr, "Error while creating Filtered Iterator\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }
    
    /* ... more code ... */
}
