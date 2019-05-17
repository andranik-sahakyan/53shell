int timeComparator(ProcessEntry_t* node1, ProcessEntry_t* node2);
ProcessEntry_t* findByPid(List_t* bg_list, pid_t pid); 
void printBgList(List_t* bg_list);
void killBgProcs(List_t* bg_list); 
int executePipe(char* left_cmd[], char* right_cmd[]);
int configureIO(char* args[], size_t numTokens);
