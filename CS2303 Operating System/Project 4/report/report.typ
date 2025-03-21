#set align(center + horizon)
#set text(size: 40pt)
= Project 4
#set text(size: 24pt)
Scheduling Algorithms\
#set align(start + top)
#set text(size: 14pt)
#pagebreak(weak: true)

#set page(header: context{
  if counter(page).get().first() > 1 [
  *Project 4*
  #h(1fr)
  _Scheduling Algorithms_
  ]
}, numbering: "1")
#counter(page).update(1)

= Introduction
The project implements five scheduling algorithms: First-Come-First-Serve (FCFS), Shortest-Job-First (SJF), Priority, Round-Robin (RR), and Priority with Round-Robin. The project simulates the scheduling of processes and calculates the average waiting time, turnaround time, and response time for each algorithm. The project also use atomic variables to ensure thread safety when allocating tid to processes.

= Implementation
== FCFS
The First-Come-First-Serve algorithm schedules processes based on the order they arrive. The algorithm is implemented using a queue to store the processes. The algorithm traverses the queue and runs each process in order.

== SJF
The Shortest-Job-First algorithm schedules processes based on the burst time of the processes. The algorithm traverses the queue and finds the process with the shortest burst time to run next, and removes the process from the queue.

== Priority
The Priority algorithm schedules processes based on the priority of the processes. The algorithm traverses the queue and finds the process with the highest priority to run next, and removes the process from the queue.

== RR
The Round-Robin algorithm schedules processes in a circular queue. The algorithm runs each process for a fixed time quantum and removes the process from the queue if the process is completed. When it reaches the end of the queue, the algorithm starts from the beginning of the queue again, until all processes are completed.

== Priority with RR
The Priority with Round-Robin algorithm schedules processes based on the priority of the processes. The processes have a maximum and a minimum priority. The algorithm traverses from the highest priority to the lowest priority and checks if there are any processes in the queue with the current priority. If there are processes with the current priority, the algorithm runs the processes in a round-robin fashion, as is introduced in the RR algorithm.

== Atomic Variables
The project provides a `get_tid()` function to allocate a unique tid to each process. The Implementation is:
```c
int get_tid()
{
  static atomic_int tid = 0;
  return atomic_fetch_add(&tid, 1);
}
```
All threads call the `get_tid()` function to get a unique tid. The `atomic_fetch_add()` function ensures that the tid is incremented atomically, so each process gets a unique tid.

== Statistics
The project calculates the average waiting time, turnaround time, and response time for each scheduling algorithm. The statistics are updated in the `run()` function for each algorithm. The statistics are stored in arrays for each process, and the average is calculated at the end of the simulation.
```c
static int *burst_time      = NULL;
static int *turnaround_time = NULL;
static int *waiting_time    = NULL;
static int *response_time   = NULL;
static int *response_flag   = NULL;
static int  time            = 0;
static int  task_count      = 0;

void init_stats(int tsk)
{
  // Initialization code
}

void print_stats()
{
  // Print statistics
}

void run(Task *task, int slice)
{
  printf("Running task = [%s] [%d] [%d] for %d units.\n", 
         task->name, task->priority, task->burst, slice);

  if (response_flag[task->tid] == 0)
  {
    burst_time[task->tid]  = task->burst;
    response_time[task->tid] = time;
    response_flag[task->tid] = 1;
  }
  time += slice;

  if (task->burst - slice == 0)
  {
    turnaround_time[task->tid] = time;
    waiting_time[task->tid]  = time - task->burst;
  }
}
```

= Correctness
The following figures show the results of the input `schedule.txt` file for each scheduling algorithm.
#figure(
  image("fcfs.png", width: 70%),
  caption: [FCFS],
)
#figure(
  image("sjf.png", width: 70%),
  caption: [SJF],
)
#figure(
  image("priority.png", width: 70%),
  caption: [Priority],
)
#figure(
  image("rr.png", width: 70%),
  caption: [RR],
)
#figure(
  image("priority_rr.png", width: 70%),
  caption: [Priority with RR],
)
The average time is shown below:
#table(
  columns: (auto, auto, auto, auto),
  inset: 10pt,
  align: horizon,
  table.header(
    [*Algorithm*], [*Average Waiting Time*], [*Average Turnaround Time*], [*Average Response Time*]
  ),
  "FCFS", "94.375", "73.125", "73.125",
  "SJF", "82.500", "61.250", "61.250",
  "Priority", "93.125", "71.875", "71.875",
  "RR", "133.125", "128.125", "17.500",
  "Priority with RR", "101.875", "96.875", "63.125" 
)

= Conclusion
The project implements five scheduling algorithms and calculates the average waiting time, turnaround time, and response time for each algorithm. 
- SJF has the lowest average waiting time and turnaround time. RR has the lowest average response time. 
- RR has the highest average waiting time and turnaround time. FCFS has the highest average response time.