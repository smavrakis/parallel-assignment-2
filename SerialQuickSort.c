#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define NUM_ELEMENTS 10

void xorSwap (int *x, int *y) {
     if (x != y) {
         *x ^= *y;
         *y ^= *x;
         *x ^= *y;
     }
 }

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

	//Mporeis na dokimaseis na xrhsimopoihseis
	//thn xorSwap poy evala :P malakizomai
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

	int i,num_elem;
	double *A;
	clock_t start_time,end_time;

	num_elem = NUM_ELEMENTS;

	if (argc == 2){
		// Argument is the size of the list
		num_elem = atoi(argv[1]);
	}

	A = malloc(num_elem * sizeof(double));
	srand48((unsigned int)time(NULL));

	//printf("\n\nUnsorted: ");
	for (i=0;i<num_elem;i++){
		A[i] = drand48() * 100;
		//printf(" %f ",A[i]);
	}

	start_time = clock();
	quicksort(A,0,num_elem-1);
	end_time = clock();

	/*printf("\n\nSorted: ");
	for (i=0;i<NUM_ELEMENTS;i++){
			printf(" %f ",A[i]);
	}*/

	if (!isSorted(A, num_elem)){
		printf("\nList did not get sorted dummy!\n");
	}else{
		printf("\nEverything went great, the list is sorted, you're a genius!\n");
	}

	printf("Processing time: %f s\n\n", (end_time-start_time)/(double)CLOCKS_PER_SEC );

	return 0;
}
