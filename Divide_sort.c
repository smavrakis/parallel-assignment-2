#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NUM_ELEMENTS 10000
#define NUM_THREADS 4

int num_thr;
pthread_mutex_t mutex;

struct thread_data{
	double *A;
	int left;
	int right;
};

int partition(double a[], int left, int right){

	int i, j;
	double pivot,temp;

	// Initialize the pivot to the first element
	pivot = a[left];
	i = left;
	j = right+1;

	while( 1){
		/* Standard implementation of quicksort: Have 2 indexes, one at the start of list and one at the end.
		 * Move the left one to the right until you find an element bigger than the pivot. Move the right one
		 * to the left until you find an element smaller than the pivot. Swap these 2 elements. If the 2 indexes
		 * intersect, break the loop.*/
		do ++i; while(a[i] <= pivot && i <= right);
	   	do --j; while(a[j] > pivot);
	   	if( i >= j ){
	   		break;
	   	}
	   	temp = a[i];
	   	a[i] = a[j];
	   	a[j] = temp;
	}

	temp = a[left];
	a[left] = a[j];
	a[j] = temp;

	return j;
}

void quicksort(double a[], int left, int right){

	int pivot;

	if( left < right ){
		pivot = partition( a, left, right);
		quicksort( a, left, pivot-1);
		quicksort( a, pivot+1, right);
	}

}

void *pquicksortHelp(void *threadarg){

	int pivot,i;
	void *status;
	struct thread_data *mydata;
	mydata = (struct thread_data *) threadarg;

	if (num_thr <= 0 || mydata->left == mydata->right){
		// Max number of threads reached, continue with serial quicksort
		quicksort(mydata->A, mydata->left, mydata->right);
		pthread_exit(NULL);
	}

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	pivot = partition(mydata->A, mydata->left, mydata->right);

	// Prepare 2 threads, 1 for each split side of the list
	struct thread_data thread_data_array[2];
	for (i = 0;i < 2;i ++){
		thread_data_array[i].A = mydata->A;
		// Decrease the global vaiable num_thr for each new thread created
		pthread_mutex_lock (&mutex);
		num_thr--;
		pthread_mutex_unlock (&mutex);
	}
	// Give the 2 threads their boundaries
	thread_data_array[0].left = mydata->left;
	thread_data_array[0].right = pivot-1;
	thread_data_array[1].left = pivot+1;
	thread_data_array[1].right = mydata->right;

	// Create the 2 threads
	pthread_t threads[2];
	for (i = 0;i < 2;i ++){
		pthread_create(&threads[i], &attr, pquicksortHelp, (void *) &thread_data_array[i]);
	}

	pthread_attr_destroy(&attr);
	for (i = 0;i < 2;i ++){
		pthread_join(threads[i], &status);
	}

	pthread_exit(NULL);

}

void pquicksort(double a[], int size){

	void *status;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&mutex, NULL);

	struct thread_data mydata;
	mydata.A = a;
	mydata.left = 0;
	mydata.right = size - 1;

	// Create the first thread that is used to call the actual sorting function
	pthread_t thread1;
	pthread_create(&thread1, &attr, pquicksortHelp, (void *) &mydata);

	pthread_attr_destroy(&attr);
	pthread_join(thread1, &status);

}

int isSorted(double a[], int size)
{
	int i;
	for (i = 1;i < size;i ++){
		if (a[i] < a[i-1]){
			printf("at loc %d, %e < %e \n", i, a[i], a[i-1]);
			return 0;
		}
	}
	return 1;
}


int main(int argc, char *argv[]) {

	int i,num_elem;
	double *A;
	clock_t start_time,end_time;

	num_elem = NUM_ELEMENTS;
	num_thr = NUM_THREADS;

	if (argc == 3){
		// First argument is the size of the list, second is the number of threads
		num_elem = atoi(argv[1]);
		num_thr = atoi(argv[2]);
	}else{
		printf("\nNo arguments given, continuing with default values: NUM_ELEMENTS 10000 NUM_THREADS 4\n");
	}

	A = malloc(num_elem * sizeof(double));
	srand48((unsigned int)time(NULL));

	// Initialize the list with random values
	for (i=0;i<num_elem;i++){
		A[i] = drand48() * 100;
	}

	// Do the sorting and measure the time it took to completion
	start_time = clock();
	pquicksort(A,num_elem);
	end_time = clock();

	// Check to see if the list was sorted correctly
	if (!isSorted(A, num_elem)){
		printf("\nList did not get sorted dummy!\n");
	}else{
		printf("\nEverything went great, the list is sorted!\n");
	}

	// Print the processing time
	printf("Processing time: %f s\n\n", (end_time-start_time)/(double)CLOCKS_PER_SEC );

	free(A);
	pthread_mutex_destroy (&mutex);
	pthread_exit(NULL);
}
