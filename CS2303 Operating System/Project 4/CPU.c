/**
 * "Virtual" CPU that also maintains track of system time.
 */

#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "task.h"

static int *burst_time      = NULL;
static int *turnaround_time = NULL;
static int *waiting_time    = NULL;
static int *response_time   = NULL;
static int *response_flag   = NULL;
static int  time            = 0;
static int  task_count      = 0;

// run this task for the specified time slice
void run(Task *task, int slice)
{
    printf("Running task = [%s] [%d] [%d] for %d units.\n", task->name, task->priority, task->burst, slice);

    if (response_flag[task->tid] == 0)
    {
        burst_time[task->tid]    = task->burst;
        response_time[task->tid] = time;
        response_flag[task->tid] = 1;
    }
    time += slice;

    if (task->burst - slice == 0)
    {
        turnaround_time[task->tid] = time;
        waiting_time[task->tid]    = time - task->burst;
    }
}

int get_tid()
{
    static atomic_int tid = 0;
    return atomic_fetch_add(&tid, 1);
}

void init_stats(int tsk)
{
    task_count      = tsk;
    burst_time      = (int *)malloc(task_count * sizeof(int));
    turnaround_time = (int *)malloc(task_count * sizeof(int));
    waiting_time    = (int *)malloc(task_count * sizeof(int));
    response_time   = (int *)malloc(task_count * sizeof(int));
    response_flag   = (int *)malloc(task_count * sizeof(int));
    for (int i = 0; i < task_count; i++)
    {
        burst_time[i]      = 0;
        turnaround_time[i] = 0;
        waiting_time[i]    = 0;
        response_time[i]   = 0;
        response_flag[i]   = 0;
    }
}

void print_stats()
{
    double sum_turnaround_time = 0;
    double sum_waiting_time    = 0;
    double sum_response_time   = 0;
    for (int i = 0; i < task_count; ++i)
    {
        sum_turnaround_time += turnaround_time[i];
        sum_waiting_time    += waiting_time[i];
        sum_response_time   += response_time[i];
    }
    printf("Average Turnaround Time = %.3f\n", sum_turnaround_time / task_count);
    printf("Average Waiting Time    = %.3f\n", sum_waiting_time / task_count);
    printf("Average Response Time   = %.3f\n", sum_response_time / task_count);
}