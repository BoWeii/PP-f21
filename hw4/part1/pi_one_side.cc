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

    MPI_Win win;

    // TODO: MPI init
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    long long int count = tosses / world_size;
    long long int temp=pi(world_rank,count);
    
    if (world_rank == 0)
    {
        // Master
        MPI_Win_create(&temp, sizeof(long long int), sizeof(long long int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    }
    else
    {
        // Workers
        MPI_Win_create(NULL, 0, sizeof(long long int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
        MPI_Accumulate(&temp, 1, MPI_LONG_LONG, 0, 0, 1, MPI_LONG_LONG, MPI_SUM, win);
        MPI_Win_unlock(0, win);
    }

    MPI_Win_free(&win);

    if (world_rank == 0)
    {
        // TODO: handle PI result
        pi_result= temp * 4 / (double)tosses;
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}