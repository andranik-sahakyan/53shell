#include "shell_util.h"

int timeComparator(void* node1, void* node2) {
	return ((ProcessEntry_t*) node1)->seconds > ((ProcessEntry_t*) node2)->seconds;
}

ProcessEntry_t* findByPid(List_t* bg_list, int pid) {
	node_t* cur_node = bg_list->head;	
	while (cur_node) {
		ProcessEntry_t* cur_proc = cur_node->value;
		if (cur_proc->pid == pid) return cur_proc;
		cur_node = cur_node->next;
	}
	return NULL;
}
