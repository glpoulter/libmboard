/* Datatype for message */
typedef struct {
    double x;
    double y;
    int    value;
} MyMessageType;

int rc;
MBt_Board myboard;

/* create the message board */
rc = MB_Create(&myboard, sizeof(MyMessageType));
if ( rc != MB_SUCCESS )
{
    fprintf(stderr, "Message board creation failed!\n");
    /* handle error */
}
