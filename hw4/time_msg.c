#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

	struct timeval start;
	struct timeval end;
	long sec;
	long usec;

	pid_t pid;
	int fd[2];
	const int READ_END = 0;
	const int WRITE_END = 1;
	
	if (argc == 1) {return 0;} // no command was passed
	
	// Prepare the pipe
	if (pipe(fd) == -1) {
		printf("failed to create pipe.\n");
		return -1;
	}
	
	pid = fork();

	if (pid < 0) {
		printf("Fork failed.\n");
		return -1;
	}

	// child process
	else if (pid == 0) {
		
		close(fd[READ_END]);

		// child exits if fetching fails
		if (gettimeofday(&start, NULL) != 0) return 0;
		
		// write to pipe
		printf("child writing: \t%ld s %ld us to shared memory.\n\n",
			       	start.tv_sec, start.tv_usec);
		write(fd[WRITE_END], &start.tv_sec, sizeof(start.tv_sec));
		write(fd[WRITE_END], &start.tv_usec, sizeof(start.tv_usec));

		// close write end when done
		close(fd[WRITE_END]);

		// have child execute given command
		execvp(argv[1], &argv[1]);

		fprintf(stderr, "Error running execvp");
	}

	// parent process
	else {	
		// immediately wait for the child to finish executing
		wait(NULL);

		close(fd[WRITE_END]);
		
		// parent exits if fetching fails
		if (gettimeofday(&end, NULL) !=0) return 0;

		// read the start time from shared memory
		read(fd[READ_END], &sec, sizeof(long));
		read(fd[READ_END], &usec, sizeof(long));

		// close read end when done
		close(fd[READ_END]);
		printf("\nparent read: \t%ld s %ld us from shared memory\n", sec, usec);
		printf("parent's time: \t%ld s %ld us\n", end.tv_sec, end.tv_usec);
		printf("execution time: %ld s, %ld us\n", end.tv_sec - sec, end.tv_usec - usec);
	}
	
	return 0;
}
