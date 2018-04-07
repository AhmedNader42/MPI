#include <stdio.h>
#include "mpi.h"
#include <math.h>

double f(double x, int order)
{
    int i=1;
    double val = x;
    for (; i<order; i++)
    {
        val *= x;
    }
    return val;
}


double calculate(double local_a, double local_b, int local_n, double h, int order)
{
    double integral=0, s=0;
    int i;

    // printf("%lf %lf %i %lf\n", local_a, local_b, local_n, h);
    s = f(local_a, order) + f(local_b, order);

    for (i=1; i<=local_n; i+=2)
    {
        s += 4 * f(local_a + (i*h), order);
    }

    for (i=2; i<=local_n-1; i+=2)
    {
        s += 2 * f(local_a+(i*h), order);
    }

    integral = s * h / 3;
    return integral;
}


int main(int argc, char* argv[])
{
    freopen("Simpsons_input.txt","r",stdin);
    freopen("Simpsons_output.txt", "w", stdout);

    int my_rank, p, source, dest = 0,tag=0;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    int testcases;
    scanf("%i", &testcases);

    while (testcases--)
    {
        int n, i, order, local_n;
        double a, b, h, local_a, local_b, start, finish;
        double s, integral, total;
        

        if (my_rank == 0)
        {
            scanf("%i %lf %lf %i", &n, &a, &b, &order);    
        }

        // Broadcast the start and end of the interval and the number of trapezoids to the SLAVES.
        MPI_Bcast(&a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&order, 1, MPI_INT, 0, MPI_COMM_WORLD);
        

        // Get the start time.
        MPI_Barrier(MPI_COMM_WORLD);
        start = MPI_Wtime();

        h = (b-a)/n;


        local_n = n/p;

        local_a = a + my_rank*local_n*h;
        local_b = local_a + local_n*h;


        integral = calculate(local_a, local_b, local_n, h, order);



        // Collect the calculations done by all the processes.
        if(my_rank == 0)
        {
            total = integral;
            for(source = 1; source < p; source++)
            {
                MPI_Recv(&integral, 1, MPI_DOUBLE, source, tag, MPI_COMM_WORLD, &status);
                total = total + integral;
             }
        }
        else
        {
            MPI_Send(&integral, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
        }


        // Print result and calculate time.
        MPI_Barrier(MPI_COMM_WORLD);
        finish = MPI_Wtime();
        if (my_rank == 0)
        {
            // printf("Estimate of the integral is (%lf) Time : (%8f)\n", total, finish - start);
            printf("%8f\n", finish - start);
        }

    }
    MPI_Finalize();
    return 0;
}
