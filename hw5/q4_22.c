#include <iostream>
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

int main(int argc, int *argv[]) {
	pthread_t workers[NUMBER_OF_THREADS];
	int size = argc-1;
	int input_arr[size];

	// Transfer trimmed inputs from command line to array
	for(int i = 0; i <= size; i++) {
		input_arr[i] = atoi(argv[i+1]);
	}
	
	for(int i = 0; i <= size; i++) {
		printf("%d ", input_arr[i]);
	}

	// Create a new thread for each statistic function
	pthread_create(workers[0], 0, find_average, &input_arr);
	pthread_create(workers[1], 0, find_minimum, &input_arr);
	pthread_create(workers[2], 0, find_maximum, &input_arr);

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
	int *arr = (int *)int_array;
	int sum = 0;

	for(int i = 0; i < size; i++) {
		sum += arr[i];
	}

	average = sum/size;
	pthread_exit(0);
}

void *find_minimum(void *int_array) {
	int *arr = (int *)int_array;
	int curr_max = arr[0];

	for(int i = 1; i < size; i++) {
		if(arr[i] > curr_max) {
			curr_max = arr[i];
		}
	}

	maximum = curr_max;
}

void *find_minimum(void *int_array) {
        int *arr = (int *)int_array;
        int curr_min = arr[0];

	for(int i = 1; i < size; i++) {
                if(arr[i] < curr_min) {
                        curr_min = arr[i];
                }
	}

	minimum = curr_min;
}
