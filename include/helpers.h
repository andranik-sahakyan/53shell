int timeComparator(ProcessEntry_t* node1, ProcessEntry_t* node2);
ProcessEntry_t* findByPid(List_t* bg_list, int pid); 
int configureIO(char* args[], int numTokens);
