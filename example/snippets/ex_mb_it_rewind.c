MyMessageType *msg1, *msg2;
MBt_Iterator  iterator;

/* create and iterator myBoard */
MB_Iterator_Create(myBoard, &iterator);

/* get a message */
MB_Iterator_GetMessage(iterator, (void *)msg1); 

/* rewind the iterator */
MB_Iterator_Rewind(iterator);

/* get another message, which should be the same value as msg1!! */
MB_Iterator_GetMessage(iterator, (void *)msg2);

/* ... more ... */

free(msg1);
free(msg2);
