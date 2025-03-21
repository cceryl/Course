// Policy: Priority Round Robin

#include <stdlib.h>

#include "cpu.h"
#include "list.h"
#include "schedulers.h"
#include "task.h"

#define TIME_QUANTUM (5)
struct node *head       = NULL;
static int   task_count = 0;

void add(char *name, int priority, int burst)
{
    Task *task     = (Task *)malloc(sizeof(Task));
    task->name     = name;
    task->tid      = get_tid();
    task->priority = priority;
    task->burst    = burst;
    insert(&head, task);
    ++task_count;
}

void schedule()
{
    init_stats(task_count);
    // reverse the list as new tasks are added to the head
    struct node *prev    = NULL;
    struct node *current = head;
    struct node *next    = NULL;
    while (current != NULL)
    {
        next          = current->next;
        current->next = prev;
        prev          = current;
        current       = next;
    }
    head = prev;

    struct node *task = head;
    int          priority;
    for (priority = MIN_PRIORITY; priority <= MAX_PRIORITY; ++priority)
    {
        int exist = 0;
        while (head != NULL)
        {
            if (task->task->priority == priority)
            {
                exist = 1;
                if (task->task->burst <= TIME_QUANTUM)
                {
                    run(task->task, task->task->burst);
                    struct node *temp = task;
                    task              = task->next;
                    delete (&head, temp->task);
                }
                else
                {
                    run(task->task, TIME_QUANTUM);
                    task->task->burst -= TIME_QUANTUM;
                    task               = task->next;
                }
            }
            else
                task = task->next;

            if (task == NULL)
            {
                task = head;
                if (!exist)
                    break;
                exist = 0;
            }
        }
    }
    print_stats();
}