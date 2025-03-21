#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 5

typedef int buffer_item;

struct buffer
{
    buffer_item data[BUFFER_SIZE];
    int         front;
    int         rear;
};

struct buffer   sharedBuffer;
pthread_mutex_t mutex;
sem_t           empty;
sem_t           full;

void *producer(void *param);
void *consumer(void *param);

int main(int argc, char *argv[])
{
    int sleepTime;
    int numProducers;
    int numConsumers;

    if (argc != 4)
    {
        printf("Invalid number of arguments\n");
        printf("Usage: %s sleepTime numProducers numConsumers\n", argv[0]);
        exit(1);
    }
    sleepTime    = atoi(argv[1]);
    numProducers = atoi(argv[2]);
    numConsumers = atoi(argv[3]);
    printf("sleepTime = %d, numProducers = %d, numConsumers = %d\n", sleepTime, numProducers, numConsumers);

    sharedBuffer.front = 0;
    sharedBuffer.rear  = 0;
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    pthread_t *producerThreads = (pthread_t *)malloc(numProducers * sizeof(pthread_t));
    pthread_t *consumerThreads = (pthread_t *)malloc(numConsumers * sizeof(pthread_t));
    int        producerID[numProducers];
    int        consumerID[numConsumers];
    for (int i = 0; i < numProducers; ++i)
        producerID[i] = i;
    for (int i = 0; i < numConsumers; ++i)
        consumerID[i] = i;
    for (int i = 0; i < numProducers; ++i)
        pthread_create(&producerThreads[i], NULL, producer, &producerID[i]);
    for (int i = 0; i < numConsumers; ++i)
        pthread_create(&consumerThreads[i], NULL, consumer, &consumerID[i]);

    sleep(sleepTime);

    for (int i = 0; i < numProducers; ++i)
        pthread_cancel(producerThreads[i]);
    for (int i = 0; i < numConsumers; ++i)
        pthread_cancel(consumerThreads[i]);

    free(producerThreads);
    free(consumerThreads);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}

void *producer(void *param)
{
    int producerID = *(int *)param;

    buffer_item next_produced;
    while (1)
    {
        sleep(rand() % 5 + 1);
        next_produced = rand() % 100;

        sem_wait(&empty);

        pthread_mutex_lock(&mutex);
        sharedBuffer.data[sharedBuffer.rear] = next_produced;
        sharedBuffer.rear                    = (sharedBuffer.rear + 1) % BUFFER_SIZE;

        int item_count;
        sem_getvalue(&full, &item_count);

        printf("Producer %d: produced %d. Item: %d\n", producerID, next_produced, item_count + 1);
        pthread_mutex_unlock(&mutex);

        sem_post(&full);
    }
}

void *consumer(void *param)
{
    int consumerID = *(int *)param;

    buffer_item next_consumed;
    while (1)
    {
        sleep(rand() % 5 + 1);

        sem_wait(&full);

        pthread_mutex_lock(&mutex);
        next_consumed      = sharedBuffer.data[sharedBuffer.front];
        sharedBuffer.front = (sharedBuffer.front + 1) % BUFFER_SIZE;

        int item_count;
        sem_getvalue(&full, &item_count);

        printf("Consumer %d: consumed %d. Item: %d\n", consumerID, next_consumed, item_count);
        pthread_mutex_unlock(&mutex);

        sem_post(&empty);
    }
}