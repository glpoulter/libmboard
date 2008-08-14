/* our message datatype */
typedef struct {
    int id;
    double price;
    double value;
} myMessageType;

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
void func_siamang(void) {

    MBt_Iterator iterator;
    
    /* assuming myboard has been created and populated */
    
    rc = MB_Iterator_CreateSorted(myboard, &iterator, &mycmp);
    if ( rc != MB_SUCCESS )
    {
        fprintf(stderr, "Error while creating Sorted Iterator\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }

    /* ... more code ... */
}
