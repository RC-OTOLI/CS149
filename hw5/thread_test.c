nclude <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#incldue <sys/wait.h>
#include <unistd.h>

int value = 0;
void *runner(void *param); /* the thread */

int main(int argc, int *argv[]) {
	pid_t pid;
	pthread_t tid;
	pthread_attr_t attr;

	pid = fork();

	if(pid == 0) { /* child process */
		pthread_attr_init(&attr);
		pthread_create(&tid, &attr, runner, NULL); /* LINE C */
		pthread_join(tid, NULL);

		printf("CHILD: value = %d", value);
	}

	else if(pid > 0) { /* parent process */
		wait(NULL);
		printf("PARENT: value = %d", value); /* LINE P */
	}
}

void *runner(void *param) {
	value = 5;
	pthread_exit(0);
}