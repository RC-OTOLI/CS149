#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <sys/types.h>
//#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

	const int READ_END = 0;
	const int WRITE_END = 1;

	pid_t pid;
	int file_pipe[2];
	int process_pipe[2];
	char buffer[2];

	// prepare pipes
	file_pipe[READ_END] = open(argv[1], O_RDONLY);
	file_pipe[WRITE_END] = open(argv[2], O_CREAT | O_RDWR, 0666);
	pipe(process_pipe);

	pid = fork();

	if (pid < 0) {printf("Fork failed\n"); return -1;}

	// child process
	else if (pid == 0) {
		
		// close unused pipe ends
		printf("child closing process pipe write end");
		//close(process_pipe[WRITE_END]);

		// copy data from parent process to target file
		while(read(process_pipe[READ_END], buffer, sizeof(buffer))) {
			write(file_pipe[WRITE_END], buffer, sizeof(buffer));
		}

		// Close remaining i/o
		fclose((FILE*)file_pipe[WRITE_END]);
		close(process_pipe[READ_END]);

		exit(0);
	}

	// parent process
	if (pid > 0) { 

		// close unused pipe ends
		printf("parent closing process pipe read end");
		//close(process_pipe[READ_END]);

		// pass data from source file to child process
		while (read(file_pipe[READ_END], buffer, sizeof(buffer))) {
			write(process_pipe[WRITE_END], buffer, sizeof(buffer));
		}

		// Close remaining i/o
		fclose((FILE*)file_pipe[READ_END]);
		close(process_pipe[WRITE_END]);

		wait(NULL);
	}
	
	return(0);
}
