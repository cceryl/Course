#include "threadpool.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define QUEUE_SIZE        10
#define NUMBER_OF_THREADS 3

typedef struct
{
    void (*function)(void *p);
    void *data;
} task;

typedef struct
{
    task tasks[QUEUE_SIZE + 1];
    int  front;
    int  rear;
} queue;

queue           taskQueue;
pthread_t       workerBee[NUMBER_OF_THREADS];
pthread_mutex_t queueLock;
sem_t           occupied;
sem_t           vacancy;

void enqueue(task t)
{
    sem_wait(&vacancy);

    pthread_mutex_lock(&queueLock);
    taskQueue.tasks[taskQueue.rear] = t;
    taskQueue.rear                  = (taskQueue.rear + 1) % (QUEUE_SIZE + 1);
    pthread_mutex_unlock(&queueLock);

    sem_post(&occupied);
}

task dequeue()
{
    sem_wait(&occupied);

    pthread_mutex_lock(&queueLock);
    task t          = taskQueue.tasks[taskQueue.front];
    taskQueue.front = (taskQueue.front + 1) % (QUEUE_SIZE + 1);
    pthread_mutex_unlock(&queueLock);

    sem_post(&vacancy);

    return t;
}

void *worker(void *param)
{
    while (1)
    {
        task t = dequeue();
        execute(t.function, t.data);
    } 
}

void execute(void (*f)(void *), void *p) { f(p); }

int pool_submit(void (*f)(void *), void *p)
{
    pthread_mutex_lock(&queueLock);
    if ((taskQueue.rear + 1) % (QUEUE_SIZE + 1) == taskQueue.front)
    {
        pthread_mutex_unlock(&queueLock);
        return 0;
    }
    pthread_mutex_unlock(&queueLock);

    task t = {.function = f, .data = p};
    enqueue(t);

    return 1;
}

void pool_init(void)
{
    sem_init(&occupied, 0, 0);
    sem_init(&vacancy, 0, QUEUE_SIZE);

    pthread_mutex_init(&queueLock, NULL);
    taskQueue.front = 0;
    taskQueue.rear  = 0;

    for (int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_create(&workerBee[i], NULL, worker, NULL);
}

void pool_shutdown(void)
{
    sem_destroy(&occupied);
    sem_destroy(&vacancy);

    pthread_mutex_destroy(&queueLock);
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_cancel(workerBee[i]);
}