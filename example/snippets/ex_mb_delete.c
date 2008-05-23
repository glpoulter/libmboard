MBt_Board myboard;
/* .... more code .... */

rc = MB_Create(&myboard, sizeof(myMessageType));
/* .... more code .... */

if ( MB_Delete(&myboard) != MB_SUCCESS )
{
    fprintf(stderr, "Could not delete message board\n");
    /* handle error */
}
