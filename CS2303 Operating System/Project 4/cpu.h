// length of a time quantum
#define QUANTUM 10

#include <stdatomic.h>

#include "task.h"

// run the specified task for the following time slice
void run(Task *task, int slice);

int get_tid();

void init_stats(int task_count);

void print_stats();