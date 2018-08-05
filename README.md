# Shared memory parallelization using Pthreads

## Quick-Sort
The serial Quick-sort algorithm is described numerously in the literature and briefly the algorithm
follows as:
1. Select a pivot element.
2. Divide the data into two sets according to the pivot (smaller and larger).
3. Sort each list with Quick-sort recursively.
Start by implementing a serial version of this algorithm following the notes, e.g., in Wikipedia
[http://en.wikipedia.org/wiki/Quicksort ] using the in-place version. The algorithm can then
be parallelized in many ways but we will only study and implement two (or three optionally)
inherently different ways to do this, a divide-and-conquer algorithm and a peer algorithm.

## Divide and conquer parallelization
A straightforward parallelization of Quick-Sort is to acquire a new thread for each recursion
step and sort the two lists in parallel on the two threads. One problem is then to not acquire to
many threads (or too few threads). After a number of recursion levels one can proceed without
requiring new threads using the serial Quick-sort algorithm on the respective threads. How does
the number of threads affect the performance and in what way? What are the performance
obstacles?

## Peer parallelization
Create p threads as peers, divide the data equally assigning n=p elements to each thread, and
sort the elements internally within each thread using the serial Quick-Sort algorithm. Then, in p
phases alternating with your left and your right neighbor, merge data and keep either the right or
the left part of the data. Compare parallel odd-even-sort algorithm 9.4 in the textbook or in the
lecture notes. How does the number of threads affect the performance and in what way? What
are the performance obstacles?
