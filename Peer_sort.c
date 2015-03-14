#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
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

//Function to measure the real execution time (Wall time)
double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

void merge(double a[], double b[], int left1, int right1, int left2, int right2){

	int i, j, k;
	j = left1;
	k = left2;

	/* We have 2 virtual sorted arrays (in essence, only 1 array divided into two parts).
	 * Compare each pair of elements in the arrays and put the smallest one in the temp array b,
	 * whose size is the sum of the other 2 arrays' size. The goal is to merge the 2 arrays
	 * into 1 but keep them sorted.*/
	for (i = 0; i < 2*chunk;){
		if (j <= right1 && k <= right2){
			if (a[j] < a[k]){
				b[i] = a[j];
				j++;
			}else{
				b[i] = a[k];
				k++;
			}
			i++;
		}else if (j > right1){
			for (; i < 2*chunk;){
				b[i] = a[k];
				k++;
				i++;
			}
		}else{
			for (; i < 2*chunk;){
				b[i] = a[j];
				j++;
				i++;
			}
		}
	}

	j = 0;

	/* Finally, replace the input array with the temp array's sorted elements.*/
	for (i = left1; i <= right2; i++){
		a[i] = b[j];
		j++;
	}
}

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

void *helpQuicksort(void *arg){

	int pivot,i,j;
	struct thread_data *mydata;
	mydata = (struct thread_data *) arg;
	double *B;

	B = malloc(2*chunk * sizeof(double));


	quicksort(mydata->A,mydata->left,mydata->right);

	// Wait for every thread to sort its chunk
	pthread_barrier_wait (&barrier1);

	// num_thr steps, introducing odd and even phases
	for (i=0; i<num_thr; i++){
		// Even phase
		if (i % 2 == 0){
			/* Get every thread with odd id to merge their chunk with their left neighbor's chunk and keep them sorted.
			 * Since this is a pthreads implementation, we don't need to follow the exact algorithm - the memory
			 * and the array are shared between processes so we don't actually need to send any blocks to neighboring
			 * processes, we just sort each merged block.*/
			if (mydata->id % 2 != 0){
				merge(mydata->A,B,mydata->left - chunk,mydata->left - 1,mydata->left,mydata->right);
			}
		}else{ // Odd phase
			/* Get every thread with odd id to merge their chunk with their right neighbor's chunk and keep them sorted.
			 * The last thread with odd id does nothing because it doesn't have a neighbor on its right*/
			if ((mydata->id % 2 != 0) && (num_thr - mydata->id != 1)){
				merge(mydata->A,B,mydata->left,mydata->right,mydata->right + 1,mydata->right + chunk);
			}
		}
		// Wait for every pair to finish their merging/sorting.
		pthread_barrier_wait (&barrier1);
	}

	free(B);
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

	// Set left and right boundaries for all threads and create them
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
	double start_time,end_time;

	num_elem = NUM_ELEMENTS;
	num_thr = NUM_THREADS;

	if (argc == 3){
		// First argument is the size of the list, second is the number of threads
		num_elem = atoi(argv[1]);
		num_thr = atoi(argv[2]);
	}else{
		printf("\nNo arguments given, continuing with default values: NUM_ELEMENTS 10000 NUM_THREADS 4\n");
	}

	// Size of elements for each thread
	chunk = num_elem/num_thr;

	A = malloc(num_elem * sizeof(double));
	srand48((unsigned int)time(NULL));

	// Initialize the list with random values
	for (i=0;i<num_elem;i++){
		A[i] = drand48() * 100;
	}

	// Do the sorting and measure the time it took to completion
	start_time = get_wall_time();
	pquicksort(A);
	end_time = get_wall_time();

	// Check to see if the list was sorted correctly
	if (!isSorted(A, num_elem)){
		printf("\nList did not get sorted dummy!\n");
	}else{
		printf("\nEverything went great, the list is sorted!\n");
	}

	// Print the processing time
	printf("Processing (Wall) time: %f s\n\n", (end_time-start_time) );

	free(A);
	pthread_barrier_destroy(&barrier1);
	pthread_exit(NULL);
}
