//#include <iostream.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int NUMBER_OF_THREADS = 3;
int average = 0;
int minimum = 0;
int maximum = 0;
int size = 0;

void *find_average(void *int_arr);
void *find_minimum(void *int_arr);
void *find_maximum(void *int_arr);

int main(int argc, char *argv[]) {
	pthread_t workers[NUMBER_OF_THREADS];
	int *input_arr = (int *)malloc((size)*sizeof(int));

	// Transfer trimmed inputs from command line to array
	for(int i = 1; i < argc; i++) {
		printf("inputting %d\n", atoi(argv[i]));
		input_arr[i-1] = atoi(argv[i]);
		size++;
	}
	
	for(int i = 0; i < size; i++) {
		printf("%d ", input_arr[i]);
	}
	printf("\n");

	// Create a new thread for each statistic function
	pthread_create(&workers[0], 0, find_average, (void *)input_arr);
	pthread_create(&workers[1], 0, find_minimum, (void *)input_arr);
	pthread_create(&workers[2], 0, find_maximum, (void *)input_arr);

	// Wait for all threads to finish
	for(int i = 0; i < NUMBER_OF_THREADS; i++) {
		pthread_join(workers[i], NULL);
	}
	
	// Print results
	printf("The average value is %d\n", average);
	printf("The minimum value is %d\n", minimum);
	printf("The maximum value is %d\n", maximum);

	return 0;
}

void *find_average(void *int_array) {
	printf("avg thread executing\n");
	int *arr = (int *)int_array;
	int sum = 0;

	for(int i = 0; i < size; i++) {
		sum += arr[i];
	}

	average = sum/size;
	printf("avg thread exiting\n");
	pthread_exit(0);
}

void *find_minimum(void *int_array) {
	int *arr = (int *)int_array;
	int curr_min = arr[0];

	printf("minimum thread executing\n");
	for(int i = 1; i < size; i++) {
		if(arr[i] < curr_min) {
			curr_min = arr[i];
		}
	}

	minimum = curr_min;
	printf("minimum thread exiting\n");
	pthread_exit(0);
}

void *find_maximum(void *int_array) {
        int *arr = (int *)int_array;
        int curr_max = arr[0];

	printf("maximum thread executing\n");
	for(int i = 1; i < size; i++) {
                if(arr[i] > curr_max) {
                        curr_max = arr[i];
                }
	}

	maximum = curr_max;
	printf("maximum thread exiting\n");
	pthread_exit(0);
}
