int rc;
MyMessageType *msg = NULL;
MBt_Iterator  iterator;

/* create and iterator myBoard */
MB_Iterator_Create(myBoard, &iterator);

rc = MB_Iterator_GetMessage(iterator, (void *)msg);
while (msg) /* loop till end of Iterator */
{
    do_something_with_message(msg);
    free(msg); /* free allocated message */
    
    /* get next message from iterator */
    rc = MB_Iterator_GetMessage(iterator, (void *)msg);
    
    if (rc != MB_SUCCESS)
    {
        fprintf(stderr, "Oh no! Error while traversing iterator.\n");
        /* handle error */
    }
}
