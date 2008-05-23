MBt_Board myboard;
MBt_Iterator iterator;

/* .... more code that creates and populate myboard .... */

rc = MB_Iterator_Create(myboard, &iterator);

if ( rc != MB_SUCCESS )
{
    fprintf(stderr, "Error while creating Iterator\n");
    /* handle error */
}

/* iterator ready to be used */
