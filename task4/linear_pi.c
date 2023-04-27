
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define SEED     921
#define NUM_ITER 1000000000

int main(int argc, char* argv[])
{
    int local_count = 0, total_count, flip;
    double x, y, z, pi;

    int rank, size, provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);

    double start_time, stop_time, elapsed_time;
    start_time = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    
    srand(SEED * rank); // Important: Multiply SEED by "rank" when you introduce MPI!

    flip = NUM_ITER / size;
    
    // Calculate PI following a Monte Carlo method
    for (int iter = 0; iter < flip; iter++)
    {
        // Generate random (X,Y) points
        x = (double)random() / (double)RAND_MAX;
        y = (double)random() / (double)RAND_MAX;
        z = sqrt((x*x) + (y*y));
        
        // Check if point is in unit circle
        if (z <= 1.0)
        {
            local_count++;
        }
    }

    if (rank==0) {
        int counts[size-1];

        for (int i = 1; i < size; i++) {
            MPI_Recv(&counts[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        total_count = local_count;
        for (int i = 1; i < size; i++) {
            total_count += counts[i];
        }

        pi = ((double)total_count / (double)NUM_ITER) * 4.0;

        stop_time = MPI_Wtime();
        elapsed_time = stop_time - start_time;
    
        printf("The result is %f\n", pi);
        printf("Execution time %f\n", elapsed_time);
    
    } else {
        MPI_Send(&local_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } 
    
    MPI_Finalize();

    return 0;
}

