#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 100000
void construct_matrices(int *n_ptr, int *m_ptr, int *l_ptr, int **a_mat_ptr, int **b_mat_ptr)
{
    int world_rank, world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int dim[3];
    char input[MAX_SIZE];
    char *pEnd;
    if (world_rank == 0)
    {
        fgets(input, MAX_SIZE, stdin);
        dim[0] = strtoull(input, &pEnd, 10);
        dim[1] = strtoull(pEnd, &pEnd, 10);
        dim[2] = strtoull(pEnd, &pEnd, 10);
    }
    MPI_Bcast(dim, 3, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);

    int main_size = dim[0] % world_size == 0 ? dim[0] / world_size : dim[0] / world_size + dim[0] % world_size;
    int others_size = dim[0] / world_size;
    int n = *n_ptr = dim[0];
    int m = *m_ptr = dim[1];
    int l = *l_ptr = dim[2];
    (*b_mat_ptr) = (int *)malloc(sizeof(int *) * m * l);
    if (world_rank == 0)
    {
        (*a_mat_ptr) = (int *)malloc(sizeof(int *) * n * m);
        for (int i = 0; i < n; i++)
        {
            fgets(input, MAX_SIZE, stdin);
            for (int j = 0; j < m; j++)
            {
                if (j == 0)
                    (*a_mat_ptr)[i * m + j] = strtoull(input, &pEnd, 10);
                else
                    (*a_mat_ptr)[i * m + j] = strtoull(pEnd, &pEnd, 10);
            }
        }
        for (int i = 0; i < m; i++)
        {
            fgets(input, MAX_SIZE, stdin);
            for (int j = 0; j < l; j++)
            {
                if (j == 0)
                    (*b_mat_ptr)[i * l + j] = strtoull(input, &pEnd, 10);
                else
                    (*b_mat_ptr)[i * l + j] = strtoull(pEnd, &pEnd, 10);
            }
        }
    }
    else
    {
        (*a_mat_ptr) = (int *)malloc(sizeof(int *) * others_size * m);
    }

    if (world_rank == 0)
    {
        int set_size = others_size * m;
        int start = main_size * m;
        for (int i = 1; i < world_size; i++)
        {
            int offset = (i - 1) * set_size;
            MPI_Send(&(*a_mat_ptr)[start + offset], set_size, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv((*a_mat_ptr), others_size * m, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    MPI_Bcast((*b_mat_ptr), m * l, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
}

void matrix_multiply(const int n, const int m, const int l, const int * __restrict a_mat, const int * __restrict b_mat)
{
    int world_rank, world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int main_size = n % world_size == 0 ? n / world_size : n / world_size + n % world_size;
    int others_size = n / world_size;
    // if (world_rank == 0)
    // {
        // for (int i = 0; i < main_size * m; i++)
        // {
        //     printf("%d ", a_mat[i]);
        // }
        // printf("\n main_size=%d, other_size=%d\n", main_size, others_size);
        // printf("rank=%d\n", world_rank);
        // printf("n=%d, m=%d, l=%d\n", n, m, l);
        // printf("\n");
        // for (int i = 0; i < l * m; i++)
        // {
        //     printf("%d ", b_mat[i]);
        // }
    //     printf("\n");
    // }
    int *c_mat, n_row = world_rank == 0 ? n : others_size;

    c_mat = (int *)malloc(sizeof(int *) * n_row * l);
    //init result as zero
    for (int i = 0; i < n_row; i++)
    {
        for (int j = 0; j < l; j++)
        {
            c_mat[i * l + j] = 0;
        }
    }
    if (world_rank == 0)
    {
        n_row = main_size;
    }
    for (int i = 0; i < n_row; i++)
    {
        for (int j = 0; j < m; j++)
        {
            for (int k = 0; k < l; k++)
            {
                // c_mat[i,k]=a_mat[i,j]*b_mat[j,k];
                c_mat[i * l + k] += a_mat[i * m + j] * b_mat[j * l + k];
            }
        }
    }
    if (world_rank == 0)
    {
        int set_size = others_size * l;
        int start = main_size * l;
        for (int i = 1; i < world_size; i++)
        {
            int offset = (i - 1) * set_size;
            MPI_Recv(&c_mat[start + offset], set_size, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    else
    {
        MPI_Send(c_mat, others_size * l, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }
    // print the result
    if (world_rank == 0)
    {
        // printf("n=%d, m=%d, l=%d\n", n, m, l);
        // printf("rank=%d,main_size=%d, m=%d, l=%d\n", world_rank, main_size, m, l);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < l; j++)
            {
                printf("%d ", c_mat[i * l + j]);
            }
            printf("\n");
        }
    }
    free(c_mat);
}
void destruct_matrices(int *a_mat, int *b_mat)
{
    free(a_mat);
    free(b_mat);
}