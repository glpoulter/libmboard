/* some function somewhere */
void func_itik(void) {
    
    MyMessageType *msg1, *msg2;
    MBt_Iterator  iterator;
    
    /* assuming myboard has been created and populated */
    
    /* create and iterator myBoard */
    MB_Iterator_Create(myBoard, &iterator);
    
    /* get a message */
    MB_Iterator_GetMessage(iterator, (void *)msg1); 
    
    /* rewind the iterator */
    MB_Iterator_Rewind(iterator);
    
    /* get another message */
    MB_Iterator_GetMessage(iterator, (void *)msg2);
    
    /* msg1 and msg2 will be pointers to different blocks of data, but
     * both blocks will contain the same data
     */
    
    /* ... more ... */
    
    free(msg1);
    free(msg2);
}
