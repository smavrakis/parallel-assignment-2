#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NUM_ELEMENTS 1000
#define NUM_THREADS 4

struct thread_data{
	double *A;
	int left;
	int right;
	int num_thr;
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

void *pquicksortHelp(void *threadarg){

	int pivot,i;
	void *status;
	struct thread_data *mydata;
	mydata = (struct thread_data *) threadarg;

	if (mydata->num_thr <= 0 || mydata->left == mydata->right){
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
		thread_data_array[i].num_thr = mydata->num_thr - 1;
	}
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

void pquicksort(double a[], int size, int num_thr){

	void *status;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	struct thread_data mydata;
	mydata.A = a;
	mydata.left = 0;
	mydata.right = size - 1;
	mydata.num_thr = num_thr;

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
			return 0;
		}
	}
	return 1;
}


int main(int argc, char *argv[]) {

	int i,num_elem,num_thr;
	double *A;
	clock_t start_time,end_time;

	num_elem = NUM_ELEMENTS;
	num_thr = NUM_THREADS;

	if (argc == 3){
		// First argument is the size of the list, second is the number of threads
		num_elem = atoi(argv[1]);
		num_thr = atoi(argv[2]);
	}

	A = malloc(num_elem * sizeof(double));
	srand48((unsigned int)time(NULL));

	//printf("\n\nUnsorted: ");
	for (i=0;i<num_elem;i++){
		A[i] = drand48() * 100;
		//printf(" %f ",A[i]);
	}

	start_time = clock();
	pquicksort(A,num_elem,num_thr);
	end_time = clock();

	/*printf("\n\nSorted: ");
	for (i=0;i<num_elem;i++){
			printf(" %f ",A[i]);
		}*/

	if (!isSorted(A, num_elem)){
		printf("\nList did not get sorted dummy!\n");
	}else{
		printf("\nEverything went great, the list is sorted, you're a genius!\n");
	}

	printf("Processing time: %f s\n\n", (end_time-start_time)/(double)CLOCKS_PER_SEC );

	free(A);
	pthread_exit(NULL);
}
