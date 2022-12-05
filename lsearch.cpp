/*
Linear Search with MPI
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>


int main(int argc, char **argv)
{
	/*Local Lib*/
	int rank, np; //rank of processes and number of processes
	int n, n_elems_recv, n_low, mpi, size_elems;
	double times;
	int count;
	
	int arr[] = { 7,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
		17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,
		36,37,38,39,40,41,42,43,44,45,17,19,20,19,22,56,3,2,1,
		1,1,2,3,4,99,100,45,46,47,48,49,50,51,52,53,54,55,98,97,96,98
		,67,68,69,65,55,78,79,71,72,73,77,76,47,61,63,90,95,96,11
		,12,13,13,13,13,14,14,2,2,2,3,4,4,4,4,4,4,4,6,6,6,6,
		6,6,6,6,6,32,33,31,30,20,10,40,50,60,70,80,90,
		122,33,32,44,32,44,4,5,7,7,7,79,9,9,9,1,2,3,4,5,1,2,3,4,5,7,8,9,
		1,2,3,4,5,6,4,4,4,4,4,1,1,1,1,11,11,11,12,12,12,12,13,13,4,4,4,
		5,5,5,5,5,5,5,5,55,5,5,5,5,5,5,5,5,5,5,20,20,20,20,20,15,14,14,
		13,13,13,14,15,16,17,18,21,21,21};

	int arr2[10000];
	

	MPI_Status status;
	int size = sizeof(arr) / sizeof(int);
	/*Algorithm*/
	count = 0;

	// Initialize MPI
	mpi = MPI_Init(&argc, &argv);
	times = -MPI_Wtime();

	if (mpi != 0) {
		printf("\n");
		printf("Fatal error!\n");
	}

	//get number and rank of processes
	mpi = MPI_Comm_size(MPI_COMM_WORLD, &np);
	mpi = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	

	// do this if processes id or rank is 0 or master rank
	if (rank == 0)
	{
		int index, i, odd , j;
		printf("Count Odd Number Program with MPI is Started\n");
		printf("The number of processes: %d and data is : %d\n", np, size);
		
		size_elems = size / np;

		if (np > 1) {

			for (i = 1; i < np - 1; i++) {
				index = i * size_elems;

				//Send information about size of array
				MPI_Send(&size_elems, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				//Send Array with the size
				MPI_Send(&arr[index], size_elems, MPI_INT, i, 0, MPI_COMM_WORLD);

			}

			// last process adds remaining elements
			index = i * size_elems;
			int sisa_elemts = size - index;

			MPI_Send(&sisa_elemts, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&arr[index], sisa_elemts, MPI_INT, i, 0, MPI_COMM_WORLD);

			// Find odd number
			for (i = 0; i < size_elems; i++ ) {
				if (arr[i] % 2 != 0 ) {
					count++;
				}
			}

			j = 0;
			for (i = 1; i < np; i++) {
				MPI_Recv(&odd,1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
				j = j + odd;
			}
			
			j = j + count;
			printf("total of odd number is : %d\n", j);
		}
	}
	else {
		// All Slave Process will do this code

		//Recv information about array
		MPI_Recv(&n_elems_recv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		//Recv Array with portion or indexing each rank so another rank wil be have another indexing
		MPI_Recv(&arr2, n_elems_recv, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		// Find odd number in each rank processess except rank 0
		for (int i = 0; i < n_elems_recv; i++) {
			if (arr2[i] % 2 != 0) {
				count++;
			}
		}

		MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	times += MPI_Wtime();

	printf("time of proc %d: %f: \n",rank, times);

	printf("amount odd number in procs: %d is: %d\n",rank,count);

	mpi = MPI_Finalize();

	return 0;
}

