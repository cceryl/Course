#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data *)param;

    printf("I add two values %d and %d result = %d\n", temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    // create some work to do
    struct data works[30];

    // initialize the thread pool
    pool_init();

    // submit the work to the queue
    for (int i = 0; i < 30; i++)
    {
        works[i].a = rand() % 20;
        works[i].b = rand() % 20;
        while (!pool_submit(add, &works[i]))
            usleep(10);
    }

    // shutdown the thread pool
    pool_shutdown();

    return 0;
}
