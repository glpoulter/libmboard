MBt_Board myboard;
MBt_Iterator iterator;
/* .... more code .... */

rc = MB_Iterator_Create(myboard, &iterator);
/* .... more code .... */

rc = MB_Iterator_Delete(&iterator);
if ( rc != MB_SUCCESS )
{
    fprintf(stderr, "Unable to delete Iterator\n");
    /* handle error */
}
