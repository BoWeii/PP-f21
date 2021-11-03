#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
int thread_count = 0;
long long int toss = 0;
pthread_mutex_t mutex;

typedef struct
{
    long long int total;
    int id;
} cus;

void *child(void *body)
{
    cus *my_body = (cus *)body;
    // printf("%d \n",my_body->id);
    unsigned int seed =  time(NULL) * my_body->id;
    long long int times = toss / thread_count, temp = 0;
    for (long long int i = 0; i < times; i++)
    {
        double X = ((double)rand_r(&seed) / RAND_MAX) * 2 - 1;
        double Y = ((double)rand_r(&seed) / RAND_MAX) * 2 - 1;
        double sum = X * X + Y * Y;
        if (sum <= 1)
        {
            temp++;
        }
    }
    my_body->total += temp;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    thread_count = atoi(argv[1]);
    toss = atoi(argv[2]);
    pthread_t t[thread_count];
    cus x[thread_count];
    for (int i = 0; i < thread_count; i++)
    {
        x[i].total = 0;
        x[i].id=i;
        pthread_create(&t[i], NULL, child, (void *)&x[i]);
    }
    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(t[i], NULL); // 等待子執行緒執行完成
    }
    long long int total2 = 0;
    for (int i = 0; i < thread_count; i++)
    {
        total2 += x[i].total;
    }
    printf("%lf\n", 4.0 * total2 / (double)toss);
    return 0;
}