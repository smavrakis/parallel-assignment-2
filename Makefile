CC=gcc
LIBS=-lm 
FLAGS= -O3 -pthread
pi: pi.c 
	$(CC) $(FLAGS) -o pi pi.c $(LIBS)
	
SerialQuickSort: SerialQuickSort.c
	$(CC) $(FLAGS) -o SerialQuickSort SerialQuickSort.c $(LIBS)
	
Divide_sort: Divide_sort.c
	$(CC) $(FLAGS) -o Divide_sort Divide_sort.c $(LIBS)
	
Peer_sort: Peer_sort.c
	$(CC) $(FLAGS) -o Peer_sort Peer_sort.c $(LIBS)
	
clean:
	rm -f *.o *~ SerialQuickSort Divide_sort Peer_sort


