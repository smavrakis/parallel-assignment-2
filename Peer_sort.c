#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NUM_ELEMENTS 20
#define NUM_THREADS 4

pthread_mutex_t mutex;
struct thread_data{
	double *A;
	int left;
	int right;
	//int num_thr;
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
void *helpQuicksort(void *arg);
void quicksort(double a[], int left, int right);
void pquicksort(double a[], int size, int num_thr){

	void *status;
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&mutex, NULL);
	int t;
	struct thread_data mydata[NUM_THREADS];
	
	//Size of elements for each thread
	int chunk = size/num_thr;	

	//Set left and right for all threads
	for (t=0; t<NUM_THREADS; t++){
		mydata[t].A = a;
		mydata[t].left = t*chunk;
		mydata[t].right = (t+1)*chunk - 1;
      	pthread_create(&threads[t], &attr, helpQuicksort, (void *)&mydata[t]);
   	}

   	for (t=0; t<NUM_THREADS; t++){
      pthread_join(threads[t], &status);
   	}

	pthread_attr_destroy(&attr);	
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

	int pivot;
	struct thread_data *mydata;
	mydata = (struct thread_data *) arg;
	// printf("%d\n", mydata-);
	quicksort(mydata->A,mydata->left,mydata->right);
}

int main(int argc, char *argv[]) {

	int i;
	double *A;

	A = malloc(NUM_ELEMENTS * sizeof(double));
	srand((unsigned int)time(NULL));
	// Variemai na psaksw na kanw tin drand48/srand48 na doulepsei sta windows
	//srand48((unsigned int)time(NULL));

	double scaleLimit = 100.0;
	double divisor = (double)RAND_MAX/scaleLimit;

	printf("\n\nUnsorted: \n");
	for (i=0;i<NUM_ELEMENTS;i++){
		A[i] = rand()/divisor;
		// Variemai na psaksw na kanw tin drand48/srand48 na doulepsei sta windows
		//A[i] = drand48() * 100;
		printf(" %f ",A[i]);
	}

	pquicksort(A,NUM_ELEMENTS,NUM_THREADS);

	printf("\n\nSorted: \n");
	for (i=0;i<NUM_ELEMENTS;i++){
			printf(" %f ",A[i]);
		}
	printf("\n");
	return 0;
}
