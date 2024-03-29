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

    // TODO: MPI init
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Status status;
    long long int count = tosses / world_size;

    // TODO: binary tree redunction
    long long int data = pi(world_rank, count);
    int offset = 1;

    for (int run = world_size; run > 1; run /= 2)
    {
        for (int i = 0; i < world_size; i += offset * 2)
        {
            if (world_rank == i)
            {
                long long int message = 0;
                MPI_Recv(&message, 1, MPI_LONG_LONG, i + offset, 0, MPI_COMM_WORLD, &status);
                data += message;
            }
            else if (world_rank == i + offset)
            {
                MPI_Send(&data, 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD);
            }
        }
        offset = offset * 2;
    }

    if (world_rank == 0)
    {
        // TODO: PI result
        pi_result = 4.0 * data / (double)tosses;
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
