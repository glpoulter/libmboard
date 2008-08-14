/* some function somewhere */
void func_angsa(void) {

    MyMessageType *msg1, *msg2, *msg3, *msg4;
    MBt_Iterator  iterator;
    
    /* assuming myboard has been created and populated */
    
    /* create and iterator myBoard */
    MB_Iterator_Create(myBoard, &iterator);
    
    /* ... fill up board ... */
    
    /* get messages */
    MB_Iterator_GetMessage(iterator, (void *)msg1); 
    MB_Iterator_GetMessage(iterator, (void *)msg2); 
    MB_Iterator_GetMessage(iterator, (void *)msg3); 
    MB_Iterator_GetMessage(iterator, (void *)msg4); 
    /* ... more ... */
    
    /* .... process messages .... */
    
    free(msg1);
    free(msg2);
    free(msg3);
    free(msg4);
    
    /* randomise the iterator (rewind was done automatically) */
    MB_Iterator_Randomise(iterator);
    
    /* messages should now be returned in a randomised order */
    MB_Iterator_GetMessage(iterator, (void *)msg1); 
    MB_Iterator_GetMessage(iterator, (void *)msg2); 
    MB_Iterator_GetMessage(iterator, (void *)msg3); 
    MB_Iterator_GetMessage(iterator, (void *)msg4); 
    /* ... more ... */
    
    free(msg1);
    free(msg2);
    free(msg3);
    free(msg4);

}
