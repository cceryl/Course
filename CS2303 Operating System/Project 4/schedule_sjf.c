// Policy: Shortest Job First

#include <stdlib.h>

#include "cpu.h"
#include "list.h"
#include "schedulers.h"
#include "task.h"

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

    while (head != NULL)
    {
        struct node *task     = head;
        struct node *shortest = head;
        while (task != NULL)
        {
            if (task->task->burst < shortest->task->burst)
                shortest = task;
            task = task->next;
        }
        run(shortest->task, shortest->task->burst);
        delete (&head, shortest->task);
    }
    print_stats();
}