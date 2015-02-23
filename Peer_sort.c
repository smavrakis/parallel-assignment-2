#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NUM_ELEMENTS 10000
#define NUM_THREADS 4

pthread_barrier_t barrier1;
int num_thr,chunk;

struct thread_data{
	double *A;
	int left;
	int right;
	int id;
};

int partition(double a[], int left, int right){

	int i, j;
	double pivot,temp;

	pivot = a[left];
	i = left;
	j = right+1;

	while( 1){
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

void *helpQuicksort(void *arg){

	int pivot,i,j;
	struct thread_data *mydata;
	mydata = (struct thread_data *) arg;


	quicksort(mydata->A,mydata->left,mydata->right);

	// Wait for every thread to sort its chunk
	pthread_barrier_wait (&barrier1);

	// num_thr steps, introducing odd and even phases
	for (i=0; i<num_thr; i++){
		// Even phase
		if (i % 2 == 0){
			// Get every thread with odd id to merge their chunk with their left neighbor's chunk and sort them
			if (mydata->id % 2 != 0){
				quicksort(mydata->A,mydata->left - chunk,mydata->right);
			}
		}else{ // Odd phase
			/* Get every thread with odd id to merge their chunk with their right neighbor's chunk and sort them.
			 * The last thread with odd id does nothing because it doesn't have a right neighbor*/
			if ((mydata->id % 2 != 0) && (num_thr - mydata->id != 1)){
				quicksort(mydata->A,mydata->left,mydata->right + chunk);
			}
		}
		pthread_barrier_wait (&barrier1);
	}
}

void pquicksort(double a[]){

	void *status;
	pthread_t threads[num_thr];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_barrier_init (&barrier1, NULL, num_thr);
	int t;
	struct thread_data mydata[num_thr];

	//Set left and right for all threads
	for (t=0; t<num_thr; t++){
		mydata[t].A = a;
		mydata[t].left = t*chunk;
		mydata[t].right = (t+1)*chunk - 1;
		mydata[t].id = t;
      	pthread_create(&threads[t], &attr, helpQuicksort, (void *)&mydata[t]);
   	}

   	for (t=0; t<num_thr; t++){
      pthread_join(threads[t], &status);
   	}

	pthread_attr_destroy(&attr);
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

	//Size of elements for each thread
	chunk = num_elem/num_thr;

	A = malloc(num_elem * sizeof(double));
	srand48((unsigned int)time(NULL));

	for (i=0;i<num_elem;i++){
		A[i] = drand48() * 100;
	}

	start_time = clock();
	pquicksort(A);
	end_time = clock();

	if (!isSorted(A, num_elem)){
		printf("\nList did not get sorted dummy!\n");
	}else{
		printf("\nEverything went great, the list is sorted!\n");
	}

	printf("Processing time: %f s\n\n", (end_time-start_time)/(double)CLOCKS_PER_SEC );

	free(A);
	pthread_exit(NULL);
}
