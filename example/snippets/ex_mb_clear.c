MBt_Board myboard;
/* .... more code .... */

rc = MB_Create(&myboard, sizeof(myMessageType));
/* .... more code .... */

if ( MB_Clear(myboard) != MB_SUCCESS )
{
    fprintf(stderr, "Could not clear message board\n");
    /* handle error */
}
