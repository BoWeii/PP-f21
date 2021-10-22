#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define NUM_OF_TOSS 10000

int main()
{
    int number_in_circle = 0;
    double x = 0, y = 0, distance_squared = 0;
    srand(time(NULL));
    for (int toss = 0; toss < NUM_OF_TOSS; toss++)
    {
        x = (double)rand() / RAND_MAX * 2.0 - 1.0; //float in range -1 to 1
        y = (double)rand() / RAND_MAX * 2.0 - 1.0; //float in range -1 to 1
        distance_squared = x * x + y * y;
        if (distance_squared <= 1)
            number_in_circle++;
    }
    printf("hw0 pi=%lf", 4 * number_in_circle /((double) NUM_OF_TOSS));
    return 0;
}