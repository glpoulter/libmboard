/* Datatype for message */
typedef struct {
    double x;
    double y;
    int    value;
} MyMessageType;

/* some function somewhere */
void func_harimau(void) {
    
    int rc;
    MBt_Board myboard;
    
    /* create the message board */
    rc = MB_Create(&myboard, sizeof(MyMessageType));
    if ( rc != MB_SUCCESS )
    {
        fprintf(stderr, "Message board creation failed!\n");
        
        /* check valur of rc to determine reason of failure. Handle error */
        /* don't continue if error can't be handled */
        exit(1);
    }
    
    /* .... more code .... */

}
