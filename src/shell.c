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

int main(int argc, char *argv[]) {
	int i;
	char *args[MAX_TOKENS + 1];
	int exec_result;
	int exit_status;
	pid_t pid;
	pid_t wait_result;
    List_t bg_list;

    //Initialize the linked list
    bg_list.head = NULL;
    bg_list.length = 0;
    bg_list.comparator = NULL;  // Don't forget to initialize this to your comparator!!!

	// Setup segmentation fault handler
	if(signal(SIGSEGV, sigsegv_handler) == SIG_ERR) {
		perror("Failed to set signal handler");
		exit(-1);
	}

	while (1) {
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
		
		// Parsing input string into a sequence of tokens
		size_t numTokens;
		*args = NULL;
		numTokens = tokenizer(buffer, args);

		// BUILT-IN COMMANDS
		if (strcmp(args[0], "exit") == 0) {
			free(buffer);
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
			printf("%d\n", exit_status);
			continue;
		}
	
		// EXTERNAL COMMANDS		
		pid = fork();

		if (pid == 0) {
			exec_result = execvp(args[0], &args[0]);
			if (exec_result == -1) {
				printf(EXEC_ERR, args[0]);
				exit(EXIT_FAILURE);
			}
		    exit(EXIT_SUCCESS);
		}
		 else {
			if (strcmp(args[numTokens - 1], "&") == 0) printf("BG PROCESS STARTED\n");
			wait_result = waitpid(pid, &exit_status, 0);
			if (wait_result == -1) {
				printf(WAIT_ERR);
				exit(EXIT_FAILURE);
			}
		}

		// Free the buffer allocated from getline
		free(buffer);
	}
	return 0;
}
