#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	
	const char *memname = "v1";
	const int SIZE = 24;

	struct timeval start;
	struct timeval end;
	long sec;
	long usec;

	pid_t pid;
	int shm_fd;	// Shared memory file descriptor
	
	char *memptr;
	
	if (argc == 1) {return 0;} // no command was passed
	
	// Prepare the shared memory space
	shm_fd = shm_open(memname, O_CREAT | O_RDWR, 0666);
	if (shm_fd == -1) {
		printf("failed to create shared memory.\n");
		return -1;
	}
	
	// limit file size to save memory space
	ftruncate(shm_fd, SIZE); 

	// assign shared space to memory
	memptr = (char *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (memptr == MAP_FAILED) {
		printf("failed to map virtual memory.\n");
		return -1;
	}
	
	pid = fork();

	if (pid < 0) {
		printf("Fork failed.\n");
		return -1;
	}

	// child process
	else if (pid == 0) {
		// child exits if fetching fails
		if (gettimeofday(&start, NULL) != 0) return 0;
		
		// write to shared memory and move cursor along
		printf("child writing: \t%ld s %ld us to shared memory.\n\n",
			       	start.tv_sec, start.tv_usec);
		memptr += sprintf(memptr, "%ld %ld\n", start.tv_sec, start.tv_usec);

		// have child execute given command
		execvp(argv[1], &argv[1]);

		fprintf(stderr, "Error running execvp");
	}

	// parent process
	else {

		// immediately wait for the child to finish executing
		wait(NULL);

		// parent exits if fetching fails
		if (gettimeofday(&end, NULL) !=0) return 0;

		// read the start time from shared memory
		sscanf(memptr, "%ld %ld", &sec, &usec);
		printf("\nparent read: \t%ld s %ld us from shared memory\n", sec, usec);
		printf("parent's time: \t%ld s %ld us\n", end.tv_sec, end.tv_usec);
		printf("execution time: %ld s, %ld us\n", end.tv_sec - sec, end.tv_usec - usec);
	}
	
	return 0;
}
