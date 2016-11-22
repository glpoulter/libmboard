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

/* to be used for sorting myMessageType based on 'price' */
int mycmp(const void *msg1, const void *msg2) {
    myMessageType *m1, *m2;

    /* cast messages to proper type */
    m1 = (myMessageType*)msg1;
    m2 = (myMessageType*)msg2;

    if (m1->price == m2->price)
    {
        return 0;
    }
    else if (m1->price > m2->price) {
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

/* some function somewhere */
void func_beruk(void) {
        
    MBt_Iterator iterator;
    myFilterParam params;
    
    /* assuming myboard has been created and populated */
    
    params.minPrice = 10.5;
    params.maxPrice = 58.3;
    rc = MB_Iterator_CreateFilteredSorted(myboard, &iterator, &myFilter, &params, &mycmp);
    if ( rc != MB_SUCCESS )
    {
        fprintf(stderr, "Error while creating Filtered+Sorted Iterator\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }
    
    /* ... more code ... */
}
