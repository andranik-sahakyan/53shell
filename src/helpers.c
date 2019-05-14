#include <fcntl.h>
#include "shell_util.h"

int timeComparator(void* node1, void* node2) {
	return ((ProcessEntry_t*) node1)->seconds > ((ProcessEntry_t*) node2)->seconds;
}

ProcessEntry_t* findByPid(List_t* bg_list, pid_t pid) {
	node_t* cur_node = bg_list->head;	
	while (cur_node) {
		ProcessEntry_t* cur_proc = cur_node->value;
		if (cur_proc->pid == pid) return cur_proc;
		cur_node = cur_node->next;
	}
	return NULL;
}

int configureIO(char* args[], size_t numTokens) {
	int i;
	for (i = 0; i < numTokens && args[i]; ++i) {
		if (strcmp(args[i], ">") == 0) {
			args[i] = NULL;
			if (i + 1 == numTokens) { fprintf(stderr, RD_ERR); return -1; }		
			int fd = open(args[i + 1], O_WRONLY | O_CREAT, 0666);
			if (fd < 0) { fprintf(stderr, RD_ERR); return -1; }	
			dup2(fd, 1);
		} else if (strcmp(args[i], ">>") == 0) {
			args[i] = NULL;
			if (i + 1 == numTokens) { fprintf(stderr, RD_ERR); return -1; }		
			int fd = open(args[i + 1], O_WRONLY | O_APPEND | O_CREAT, 0666);
			if (fd < 0) { fprintf(stderr, RD_ERR); return -1; }		
			dup2(fd, 1);
		} else if (strcmp(args[i], "2>") == 0) {
			args[i] = NULL;
			if (i + 1 == numTokens) { fprintf(stderr, RD_ERR); return -1; }		
			int fd = open(args[i + 1], O_WRONLY | O_CREAT, 0666);
			if (fd < 0) { fprintf(stderr, RD_ERR); return -1; }		
			dup2(fd, 2);
		} else if (strcmp(args[i], "<") == 0) {
			args[i] = NULL;
			if (i + 1 == numTokens) { fprintf(stderr, RD_ERR); return -1; }		
			int fd = open(args[i + 1], O_RDONLY);
			if (fd < 0) { fprintf(stderr, RD_ERR); return -1; }		
			dup2(fd, 0);
		}
	}
}
