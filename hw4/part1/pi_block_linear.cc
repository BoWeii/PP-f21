#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

long long int pi(int rank, long long int count)
{
    unsigned int seed = time(NULL) * rank;
    long long int temp = 0;
    for (long long int i = 0; i < count; i++)
    {
        double X = ((double)rand_r(&seed) / RAND_MAX) * 2 - 1;
        double Y = ((double)rand_r(&seed) / RAND_MAX) * 2 - 1;
        double sum = X * X + Y * Y;
        if (sum <= 1)
        {
            temp++;
        }
    }
    return temp;
}

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    // TODO: init MPI
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Status status;
    long long int count = tosses / world_size;

    if (world_rank > 0)
    {
        // TODO: handle workers
        long long int message = pi(world_rank, count);
        MPI_Send(&message, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }
    else if (world_rank == 0)
    {
        // TODO: master
        pi_result = pi(0, count);
    }

    if (world_rank == 0)
    {
        // TODO: process PI result
        for (auto source = 1; source < world_size; source++)
        {
            long long int message = 0;
            MPI_Recv(&message, 1, MPI_LONG_LONG, source, 0, MPI_COMM_WORLD, &status);
            pi_result += message;
        }
        pi_result = pi_result * 4 / (double)tosses;
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}