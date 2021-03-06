/* data structure of message */
struct my_message {
    double x;
    double y;
    double z;
}

/* function to extract "x" value from message */
double get_x(void* msg) {
    return ((struct my_message*)msg)->x;
}

/* some function somewhere */
void func_search_range(void) {
    int i; 
    MBt_Board myboard;
    MBt_SearchTree tree;
    MBt_Iterator iterator;
    struct my_message *msg;
    
    /* .... more code that creates and populate myboard .... */

    /* create a search tree which prepartitions board on "x" */
    MB_SearchTree_Create1D(myboard, &tree, &get_x);
    
    /* we can now do multiple searches of the same search tree */
    for (i = 0; i < 100; i++) {
      /* search for messages where i < x < i+10 */
      MB_SearchTree_Search1D(tree, &iterator, (double)i, (double)i + 10.0);

      /* loop through results */
      MB_Iterator_GetMessage(iterator, &msg);
      while(msg) {
        /* ... do something with msg ... */
        free(msg);
        MB_Iterator_GetMessage(iterator, &msg);
      }

      /* delete iterator */
      MB_Iterator_Delete(&iterator);
    }

    /* we're done with the search tree */
    MB_SearchTree_Delete(&tree);
}
