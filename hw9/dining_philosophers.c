#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_PHILOSOPHERS 5
#define LEFT (philosopher_number+NUM_PHILOSOPHERS-1)%NUM_PHILOSOPHERS
#define RIGHT (philosopher_number+1)%NUM_PHILOSOPHERS
#define UPPER_ACTIVITY_TIME 3000000	// Maximum time spent thinking/eating in microseconds
#define LOWER_ACTIVITY_TIME 1000000	// Minimum time spent thinking/eating in microseconds

void *philosopher(void *num);
void phil_loop(void *num);
void *pickup_fork(int philosopher_number);
void *return_fork(int philosopher_number);
void check(int philosopher_number);
int crand();

enum {THINKING, HUNGRY, EATING} state[NUM_PHILOSOPHERS];
int philosophers[NUM_PHILOSOPHERS];
int RUN_MODE = 1;	// !0 = each philosopher must eat MAX_LOOP times
			// 0 = all philosophers will run until program is forcibly terminated
int MAX_LOOP = 10;

sem_t mutex;			// protect functions
sem_t S[NUM_PHILOSOPHERS];	// protect resources

void *philosopher (void *num) {
	if(RUN_MODE)
		for(int iter = 0; iter < MAX_LOOP; iter++)
			phil_loop(num);
	else
		while(1) {
			phil_loop(num);
		}
}

void phil_loop(void *num) {
	int* i = num;		// philosopher number
	usleep(crand());	// thinking time
	pickup_fork(*i);	// hungry wait
	usleep(crand());	// eating time
	return_fork(*i);
}

void *pickup_fork(int philosopher_number) {
	sem_wait(&mutex);

	state[philosopher_number] = HUNGRY;
	printf("Philosopher %d is hungry.\n", philosopher_number);

	// Check if resources are free
	check(philosopher_number);
	sem_post(&mutex);

	// Wait until resources are free
	sem_wait(&S[philosopher_number]);
}

void *return_fork(int philosopher_number) {
	sem_wait(&mutex);

	// Return philosopher to thinking state when done eating
	state[philosopher_number] = THINKING;
	printf("Philosopher %d returns %d and %d. Back to thinking\n", 
			philosopher_number, LEFT, philosopher_number);

	check(LEFT);
	check(RIGHT);

	sem_post(&mutex);
}

// Checks if resources are available
// Notifies hungry philosophers
void check(int philosopher_number) {
	//
	if(state[philosopher_number] == HUNGRY
			&& state[LEFT] != EATING
			&& state[RIGHT] != EATING) {	
		state[philosopher_number] = EATING;

//		usleep(500);	// time between interactions
		printf("Philosopher %d takes %d and %d to eat.\n",
				philosopher_number, LEFT, philosopher_number);
		
		// Notify hungry philosophers
		sem_post(&S[philosopher_number]);
	}
}

// used to simulate random eating and thinking times
int crand() {
	return (rand()%(UPPER_ACTIVITY_TIME-LOWER_ACTIVITY_TIME+1)+LOWER_ACTIVITY_TIME);
}

int main() {
	int i;
	srand(time(0));
	pthread_t tid[NUM_PHILOSOPHERS];

	sem_init(&mutex, 0, 1);
	for(int i = 0; i < NUM_PHILOSOPHERS; i++)
		sem_init(&S[i], 0, 0);

	for(int i = 0; i < NUM_PHILOSOPHERS; i++) 
		philosophers[i] = i;

	printf("Running in %s runtime mode. You can switch by changing the \"RUN_MODE\" variable in the source code\n",
			RUN_MODE ? "finite" : "infinite");
	printf("Press \"Ctrl + c\" to terminate at any time.\n\n");
	usleep(1500);	// time to read
	
	
	// Create philosophers
	for(int i = 0; i < NUM_PHILOSOPHERS; i++)
		pthread_create(&tid[i], NULL, philosopher, &philosophers[i]);
	
	for(int i = 0; i < NUM_PHILOSOPHERS; i++)
		pthread_join(tid[i], NULL);
}
