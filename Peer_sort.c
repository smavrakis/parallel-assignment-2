#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define NUM_ELEMENTS 10

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

int main(int argc, char *argv[]) {

	int i;
	double *A;

	A = malloc(NUM_ELEMENTS * sizeof(double));
	srand((unsigned int)time(NULL));
	// Variemai na psaksw na kanw tin drand48/srand48 na doulepsei sta windows
	//srand48((unsigned int)time(NULL));

	double scaleLimit = 100.0;
	double divisor = (double)RAND_MAX/scaleLimit;

	printf("\n\nUnsorted: ");
	for (i=0;i<NUM_ELEMENTS;i++){
		A[i] = rand()/divisor;
		// Variemai na psaksw na kanw tin drand48/srand48 na doulepsei sta windows
		//A[i] = drand48() * 100;
		printf(" %f ",A[i]);
	}

	quicksort(A,0,NUM_ELEMENTS-1);

	printf("\n\nSorted: ");
	for (i=0;i<NUM_ELEMENTS;i++){
			printf(" %f ",A[i]);
		}

	return 0;
}
