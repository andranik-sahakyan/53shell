#include "shell_util.h"
#include "linkedList.h"
#include "helpers.h"

// Library Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

int sigchld = 0;

int main(int argc, char *argv[]) {
	int i, is_bg;
	char* args[MAX_TOKENS + 1];
	char* args_right[MAX_TOKENS + 1];
	int exec_result;
	int exit_status;
	pid_t pid;
	pid_t wait_result;
    List_t bg_list;

    //Initialize the linked list
    bg_list.head = NULL;
    bg_list.length = 0;
    bg_list.comparator = (int (*)(void*, void*)) &timeComparator;

	// SIGSEGV Handler
	if (signal(SIGSEGV, sigsegv_handler) == SIG_ERR) {
		perror("Failed to set signal handler");
		exit(-1);
	}

	// SIGCHLD Handler
	void sigchld_handler(int sig) { sigchld = 1; }
	signal(SIGCHLD, sigchld_handler);

	// SIGUSR1 Handler
	void sigusr1_handler(int sig) { printBgList(&bg_list); }	
	signal(SIGUSR1, sigusr1_handler);
	
	while (1) {
		is_bg = 0;	
		
		// DO NOT MODIFY buffer
		// The buffer is dynamically allocated, we need to free it at the end of the loop
		char* const buffer = NULL;
		size_t buf_size = 0;

		// Print the shell prompt
		display_shell_prompt();
		
		// Read line from STDIN
		ssize_t nbytes = getline((char**) &buffer, &buf_size, stdin);

		// No more input from STDIN, free buffer and terminate
		if (nbytes == -1) {
			free(buffer);
			break;
		}

		// Remove newline character from buffer, if it's there
		if (buffer[nbytes - 1] == '\n')
			buffer[nbytes- 1] = '\0';

		// Handling empty strings
		if (strcmp(buffer, "") == 0) {
			free(buffer);
			continue;
		}
	
		// Save original command	
		char* cmd = malloc(buf_size);
		strcpy(cmd, buffer);

		// Parsing input string into a sequence of tokens
		char* right_cmd = strchr(buffer, '|');		
		size_t numTokens_right; 

		if (right_cmd) {
			*right_cmd = '\0';
			right_cmd = strdup(right_cmd + 2);
			*args_right = NULL;
			numTokens_right = tokenizer(right_cmd, args_right);
		}
	
		size_t numTokens;
		*args = NULL;
		numTokens = tokenizer(buffer, args);

		// BUILT-IN COMMANDS
		if (strcmp(args[0], "exit") == 0) {
			killBgProcs(&bg_list);		
			free(buffer);
			free(cmd);
			return 0;
		}

		if (strcmp(args[0], "cd") == 0) {
			char* path = NULL;
			size_t size = 0;
			
			if (numTokens == 1)
				exit_status = chdir(getenv("HOME"));
			else
				exit_status = chdir(args[1]);
			
			if (exit_status == 0)
				printf("%s\n", getcwd(path, size));
			else 
				fprintf(stderr, "%s", DIR_ERR);

			free(path);
			continue;
		}
	
		if (strcmp(args[0], "estatus") == 0) {
			if (WIFEXITED(exit_status))
				printf("%d\n", WEXITSTATUS(exit_status));
			continue;
		}

		if (strcmp(args[numTokens - 1], "&") == 0) {	
			args[numTokens - 1] = NULL;
			is_bg = 1;
		}

		if (right_cmd && strcmp(args_right[numTokens_right - 1], "&") == 0) {
			args_right[numTokens_right - 1] = NULL;
			is_bg = 1;
		}


		pid = fork();
		
		if (pid == 0) {
			if (right_cmd) {
				executePipe(args, args_right);
				free(buffer);
				free(cmd);
				continue;
			}	
			
			if (configureIO(args, numTokens) < 0) continue;							
			exec_result = execvp(args[0], &args[0]);
			if (exec_result == -1) {
				printf(EXEC_ERR, args[0]);
				exit(EXIT_FAILURE);
			}
		    exit(EXIT_SUCCESS);
		} else {
			if (sigchld) {
				while (wait_result = waitpid(-1, &exit_status, WNOHANG)) { 
					ProcessEntry_t* child = findByPid(&bg_list, wait_result);
					printf(BG_TERM, child->pid, child->cmd);
					removeByPid(&bg_list, wait_result);
				}
				sigchld = 0;
			}

			if (is_bg) {
				ProcessEntry_t* bg_process = malloc(sizeof(ProcessEntry_t));
				bg_process->cmd = malloc(buf_size);
				strcpy(bg_process->cmd, cmd);
				bg_process->pid = pid;
				bg_process->seconds = time(NULL);
				insertInOrder(&bg_list, bg_process);
			} else {
				wait_result = waitpid(pid, &exit_status, 0);
				if (wait_result == -1) {
					printf(WAIT_ERR);
					exit(EXIT_FAILURE);
				}
			}
		}
		
		// Free resources
		free(buffer);
		free(cmd);
	}

	return 0;
}
