MBt_IndexMap map_id;
/* our message datatype */
typedef struct {
    int recipient_id;
    double price;
    double value;
} myMessageType;

/* some function somewhere */
void func_itik(void) {

    int i, rc;
    
    /* create a map of agent IDs available on this processor */
    rc = MB_IndexMap_Create(&map_id, "Agent ID");
    assert(rc == MB_SUCCESS);
    
    /* loop thru local agents and add their IDs to map */
    for (i = 0; i < agents_in_this_proc; i++)
    {
        rc = MB_IndexMap_AddEntry(map_id, agent[i].id);
        assert(rc == MB_SUCCESS);
    }
    /* map_id should now know which agents are in out local proc */
    
    /* sync so it also knows which agents are in all remote procs */
    rc = MB_IndexMap_Sync(map_id);
    assert(rc == MB_SUCCESS);
    
    /* map is not ready to be used in agent filter functions */
    /* see filter_by_target_agents() */
    
    /* .... other stuff ... */
    
    
    /* Delete map when done */
    rc = MB_IndexMap_Delete(&map_id);
    assert(rc == MB_SUCCESS);
}

/* example filter function that uses the IndexMap. This fucntion can
 * be registered using MB_Filter_Create() and assigned to boards
 * using MB_Filter_Assign()
 */
int filter_by_target_agents(const void *msg, int pid)
{
    /* value of pid will be populated by the board sync mechanism 
     * that runs the filter, and will contain the pid of the
     * target processor
     */
    myMessageType *m = (myMessageType *)msg;
    
    if (MB_IndexMap_MemberOf(map_id, pid, m->recipient_id)) return 1; /*ok*/
    else return 0; /* reject */
}
