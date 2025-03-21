#set align(center + horizon)
#set text(size: 40pt)
= Project 6
#set text(size: 24pt)
Banker's Algorithm\
#set align(start + top)
#set text(size: 14pt)
#pagebreak(weak: true)

#set page(header: context{
  if counter(page).get().first() > 1 [
    *Project 6*
    #h(1fr)
    _Banker's Algorithm_
  ]
}, numbering: "1")
#counter(page).update(1)

= Introduction
The Banker's Algorithm is a deadlock avoidance algorithm that is used to avoid deadlock in a system. It is named so because it is based on the principles of a bank. A bank never allocates available cash in such a way that it can no longer satisfy the needs of all of its customers.\

The essential part of the Banker's Algorithm is the safety algorithm, which is used to check whether a system is in a safe state or not. A system is in a safe state only if there is a safe sequence of processes that, by running in that sequence, will allow each process to acquire all the resources it needs, finish the execution, and release all the resources it has acquired. If a system is in a safe state after an allocation request, the Banker's Algorithm will grant the request; otherwise, it will deny the request and force the process to wait until the system is in a safe state.\

#figure(
  image("intro.png", width: 50%),
  caption: [Test using command-line interface],
)
#pagebreak(weak: true)

= Implementation
== Algorithm
The Banker's Algorithm is implemented by following functions:
```c
void request_resources(int customer_num, int request[])
{
  // Check if request exceeds need or available
  ...

  // Pretend to allocate resources
  for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
  {
    available[i]                -= request[i];
    allocation[customer_num][i] += request[i];
    need[customer_num][i]       -= request[i];
  }

  // Check if system is in safe state
  if (check_safe())
    printf("Successfully allocated resources.\n");
  else
  {
    // Rollback allocation
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
    {
      available[i]                += request[i];
      allocation[customer_num][i] -= request[i];
      need[customer_num][i]       += request[i];
    }
    printf("Request denied to maintain safety.\n");
  }
}

void release_resources(int customer_num, int release[])
{
  // Check if release exceeds allocation
  // Release the resources
}

int check_safe(void)
{
  // The available resources in the simulation
  int work[NUMBER_OF_RESOURCES];
  // The finish status of each customer
  int finish[NUMBER_OF_CUSTOMERS];
  int finish_count = 0;

  for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
    work[i] = available[i];
  memset(finish, 0, sizeof(finish));

  while (finish_count < NUMBER_OF_CUSTOMERS)
  {
    // Search for a customer that can finish
    int found = 0;

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
      if (!finish[i])
      {
        // Check if customer can finish
        int enough = 1;
        for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
          if (need[i][j] > work[j])
          {
            enough = 0;
            break;
          }
        // If customer can finish, release all its resources
        if (enough)
        {
          found     = 1;
          finish[i] = 1;
          ++finish_count;
          for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
            work[j] += allocation[i][j];
        }
      }

    // If no customer can finish, the system is in an unsafe state
    if (!found)
        return 0;
  }

  // Found a safe sequence, the system is in a safe state
  return 1;
}
```
== Command-line Interface
The Banker's Algorithm is tested using a command-line interface. The user can request resources and release resources using the following commands:
- `RQ <customer_num> <request1> <request2> ...`: Request resources for a customer.
- `RL <customer_num> <release1> <release2> ...`: Release resources for a customer.
- `*`: Print the current state of the system.
- `exit`: Exit the program.\

The main function parses the user input and calls the corresponding functions to request or release resources, print the current state of the system, or exit the program.\

If one of the customers is finished, the resources allocated to the customer are released and its need is set to 0, in order to indicate that the customer has finished its execution when the state of the system is printed.\
#pagebreak(weak: true)

= Correctness
The correctness test of the Banker's Algorithm is shown in the following figure:
#figure(
  image("correctness1.png", width: 40%),
  caption: [Request is granted],
)
#figure(
  image("correctness2.png", width: 40%),
  caption: [Request exceeds need],
)
#figure(
  image("correctness3.png", width: 40%),
  caption: [Request exceeds available],
)
#figure(
  image("correctness4.png", width: 40%),
  caption: [Request is denied to maintain safety],
)
#figure(
  image("correctness5.png", width: 40%),
  caption: [Release resources],
)
#figure(
  image("correctness6.png", width: 40%),
  caption: [Release resources when customer is finished],
)
#figure(
  image("correctness7.png", width: 40%),
  caption: [Release exceeds allocation],
)
#pagebreak(weak: true)