#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main() {
	pid_t pid;
	pid = fork();

	// Fork failed
	if (pid < 0) {
		return(1);
	}

	// Child process
	else if (pid == 0) {
 		// child process finishes immediately;
		// becomes zombie
		exit(0);
	}

	// Parent process
	else if (pid > 0) {
		// keep parent alive for at least 10s
		sleep(10);
		// kill parent process manually in terminal using >kill -9 <pid>
	}
	
	return(0);
}
