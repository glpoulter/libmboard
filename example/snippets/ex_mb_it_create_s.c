MBt_Board myboard;

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

/* ------- within some routine ----- */
MBt_Iterator iterator;

rc = MB_Iterator_CreateSorted(myboard, &iterator, &mycmp);

if ( rc != MB_SUCCESS )
{
    fprintf(stderr, "Error while creating Sorted Iterator\n");
    /* handle error */
}
