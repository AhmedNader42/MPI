#include <stdlib.h>
#include <stdio.h> 
#include "mpi.h"





int main(int argc, char* argv[])
{
	int my_rank;
	int p;
	int source;
	int dest = 0;
	int tag = 0;
	int i;
	MPI_Status status;
	int n;

	double start, finish;



	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	MPI_Barrier(MPI_COMM_WORLD);

	if (my_rank == 0)
	{
		printf("Enter the number.\n");
		scanf("%i", &n);

		start = MPI_Wtime();
		int rem = n%(p-1);
		int portion;
		if (rem == 0)
		{
			portion = n/(p-1);
		}
		else
		{
			portion = (n-rem)/(p-1);
		}
		
		for (source=1; source<p; source++)
		{
			MPI_Send(&portion, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
		}
	}
	else 
	{
		int portion;
		MPI_Recv(&portion, 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);
		int myPorition = my_rank*portion;
		printf("from process (%i) portion(%i)\n", my_rank, myPorition);

		unsigned long long factorial=1;
		for (i=myPorition; i >(myPorition-portion); i--)
		{
			factorial = factorial * i;
		}
		MPI_Send(&factorial, 1, MPI_UNSIGNED_LONG_LONG, dest, tag, MPI_COMM_WORLD);
	}


	MPI_Barrier(MPI_COMM_WORLD);
	if (my_rank == 0)
	{
		unsigned long long factorial=1,val;
		for (source=1; source<p; source++)
		{
			MPI_Recv(&val, 1, MPI_UNSIGNED_LONG_LONG, source, tag, MPI_COMM_WORLD, &status);
			factorial*= val;
		}

		int rem = n%(p-1);
		if (rem != 0)
		{
			printf("Rem ain't zero\n");
			for (i=n; i>(n-rem); i--)
			{
				printf("Multiplying %llu * %i\n", factorial, i);
				factorial *= i;
			}
		}

		finish = MPI_Wtime();
		printf("Factorial is %llu in Time %4f\n", factorial, finish-start);
	}



	MPI_Finalize();
	return 0;
}
