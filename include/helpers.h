int timeComparator(ProcessEntry_t* node1, ProcessEntry_t* node2);
ProcessEntry_t* findByPid(List_t* bg_list, pid_t pid); 
int configureIO(char* args[], size_t numTokens);
