/*

Program Name: cobasintaks.cpp
Description: Bubble Sort Program with MPI implementation
*/

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define N 10000

/*Defines all header for procedure and function*/
void showElapsed(int id, char* m);
void showVector(int *v, int n, int id);
int* merge(int *v1, int n1, int *v2, int n2);
void swap(int *v, int i, int j);
void sort(int *v, int n);

/*Defines global variable*/
double startT, stopT, startTime;

/*PROCEDURE for print execution time*/
void showElapsed(int id, char* m)
{
	printf("%d: %s %f secs\n", id, m, (clock() - startTime) / CLOCKS_PER_SEC);
}

void showVector(int *v, int n, int id)
{
	int i;
	printf("%d: ", id);
	for (i = 0; i < n; i++) {
		printf("%d ", v[i]);
	}
	putchar('\n');
}

int *merge(int *v1, int n1, int *v2, int n2)
{
	int i, j, k;
	int *result;

	result = (int*)malloc((n1 + n2) * sizeof(int));

	/*
		i: pointer of v1
		j: pointer of v2
		k: pointer of k
	*/
	i = 0; j = 0; k = 0;
	while (i < n1 && j < n2) {
		if (v1[i] < v2[j]) {
			result[k] = v1[i];
			i++; k++;
		}
		else {
			result[k] = v2[j];
			j++; k++;
		}
	}
	if (i == n1) {
		while (j < n2) {
			result[k] = v2[j];
			j++; k++;
		}
	}
	else {
		while (i < n1) {
			result[k] = v1[i];
			i++; k++;
		}
	}
	return result;
}

void swap(int *v, int i, int j)
{
	int t;
	t = v[i];
	v[i] = v[j];
	v[j] = t;
}

void sort(int *v, int n)
{
	int i, j;
	for (i = n - 2; i >= 0; i--) {
		for (j = 0; j <= i; j++) {
			if (v[j] > v[j+1]) {
				swap(v, i, j+1);
			}
		}
	}
}

int main(int argc, char **argv)
{
	int * data = NULL;
	int * chunk;
	int * other;
	int m, n = N;
	int id, p;
	int s;
	int i;
	int step;
	double exec_time;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	exec_time = -MPI_Wtime();
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (id == 0) {
		int r;
		srand(clock());
		s = n / p;
		r = n % p;
		data = (int*)malloc((n + p - r) * sizeof(int));
		for (i = 0; i < n; i++) {
			data[i] = rand() * 0.0001;
		}
		if (r != 0) {
			for (i = n; i < n + p - r; i++) {
				data[i] = 0;
			}
			s = s + 1;
		}

		startT = clock();
		
		MPI_Bcast(&s, 1, MPI_INT, 0, MPI_COMM_WORLD);
		chunk = (int*)malloc(s *sizeof(int));
		MPI_Scatter(data, s, MPI_INT, chunk, s, MPI_INT, 0, MPI_COMM_WORLD);

		sort(chunk, s);
	}
	else {
		MPI_Bcast(&s, 1, MPI_INT, 0, MPI_COMM_WORLD);
		chunk = (int *)malloc(s * sizeof(int));
		MPI_Scatter(data, s, MPI_INT, chunk, s, MPI_INT, 0, MPI_COMM_WORLD);
		
		sort(chunk, s);
	}

	step = 1;
	while (step < p) {
		if (id % (2 * step) == 0) {
			if (id + step < p) {
				MPI_Recv(&m, 1, MPI_INT, id + step, 0, MPI_COMM_WORLD, &status);
				other = (int*)malloc(m * sizeof(int));
				MPI_Recv(other, m, MPI_INT, id + step, 0, MPI_COMM_WORLD, &status);
				chunk = merge(chunk, s, other, m);
				s = s + m;
			}
		}
		else {
			int near = id - step;
			MPI_Send(&s, 1, MPI_INT, near, 0, MPI_COMM_WORLD);
			MPI_Send(chunk, s, MPI_INT, near, 0, MPI_COMM_WORLD);
			break;
		}
		step = step * 2;
	}

	exec_time += MPI_Wtime();
	printf("time of proc %d : %f: \n", id, exec_time);

	if (id == 0) {
		FILE* fout;

		stopT = clock();
		showVector(data, n, id);
		showVector(chunk, n, id);

		fout = fopen("result", "w");
		for (i = 0; i < s; i++) {
			if (chunk[i] != 0) {
				fprintf(fout, "%d\n", chunk[i]);
			}
		}
		fclose(fout);
	}
	MPI_Finalize();

	return 0;
}
