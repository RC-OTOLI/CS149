#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {

	pid_t pid;
	int parent_to_child_fd[2];
	int child_to_parent_fd[2];
	const int READ_END = 0;
	const int WRITE_END = 1;

	char buffer[100];
	char active_char;
	
	// Prepare the pipes
	if (pipe(parent_to_child_fd) == -1 ||
		       	pipe(child_to_parent_fd) == -1) {
		printf("failed to create pipes.\n");
		return -1;
	}
	
	pid = fork();

	if (pid < 0) {
		printf("Fork failed.\n");
		return -1;
	}

	// child process
	else if (pid == 0) {
		
		// close unused pipe ends
		close(parent_to_child_fd[WRITE_END]);
		close(child_to_parent_fd[READ_END]);

		// Keep reading and stalling until newline is processed, or error occurs
		while(read(parent_to_child_fd[READ_END], &active_char, sizeof(char)) >= 0) {
			if (active_char == '\n') {break;}

			if (isupper(active_char)) {
				active_char += 32;
			}
			else if (islower(active_char)) {
				active_char -= 32;
			}

			write(child_to_parent_fd[WRITE_END], &active_char, sizeof(char));
		}
		
		exit(0);
	}

	// parent process
	else {	

		printf("Enter sentence to invert case: \n");
		scanf("%[^\n]%*c", buffer);	// string input terminates at newline
		buffer[strlen(buffer)] = '\n';	// need to manually append newline

		// Close unused pipe ends
		close(parent_to_child_fd[READ_END]);
		close(child_to_parent_fd[WRITE_END]);
		
		for (int i = 0; i < strlen(buffer); i++) {
			write(parent_to_child_fd[WRITE_END], &buffer[i], sizeof(buffer[i]));
		}

		// Keep reading and stalling until pipe closes on child's end
		while (read(child_to_parent_fd[READ_END], &active_char, sizeof(char)) > 0) {
			printf("%c", active_char);
		}	
		printf("\n");
		
		// Clean up file i/o
		close(parent_to_child_fd[WRITE_END]);
		close(child_to_parent_fd[READ_END]);

		// Release the child from the process table
		wait(NULL);
	}
	
	return 0;
}
