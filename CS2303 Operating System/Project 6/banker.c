#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 32

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

#define INPUT_FILE "input.txt"

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

void request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);
void print_status(void);

int  check_safe(void);
void check_finish(int customer_num);

int main(int argc, char *argv[])
{
    /* Init */
    if (argc != NUMBER_OF_RESOURCES + 1)
    {
        printf("Invalid number of arguments.\n");
        printf("Usage: ./banker <available resource 1> <...> <available resource %d>\n", NUMBER_OF_RESOURCES);
        return -1;
    }
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
    {
        available[i] = atoi(argv[i + 1]);
        if (available[i] < 0)
        {
            printf("Available resources must be non-negative.\n");
            return -1;
        }
    }

    FILE *file = fopen(INPUT_FILE, "r");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
        for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
            fscanf(file, "%d", &maximum[i][j]);
    fclose(file);

    memset(allocation, 0, sizeof(allocation));

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
        for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
            need[i][j] = maximum[i][j];

    /* Main */
    while (1)
    {
        printf("Enter command > ");

        char command[BUFFER_SIZE];
        fgets(command, BUFFER_SIZE, stdin);
        if (strcmp(command, "exit\n") == 0)
            break;

        char *token = strtok(command, " ");

        /* Request */
        if (strcmp(token, "RQ") == 0)
        {
            int customer_num = atoi(strtok(NULL, " "));
            int request[NUMBER_OF_RESOURCES];
            int valid_request = 1;

            if (customer_num < 0 || customer_num >= NUMBER_OF_CUSTOMERS)
            {
                printf("Invalid customer number.\n");
                continue;
            }
            for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
            {
                request[i] = atoi(strtok(NULL, " "));
                if (request[i] < 0)
                {
                    printf("Request must be non-negative.\n");
                    valid_request = 0;
                    break;
                }
                else if (request[i] > need[customer_num][i])
                {
                    printf("Request exceeds need.\n");
                    valid_request = 0;
                    break;
                }
            }
            if (valid_request)
            {
                request_resources(customer_num, request);
                check_finish(customer_num);
            }
        }
        /* Release */
        else if (strcmp(token, "RL") == 0)
        {
            int customer_num = atoi(strtok(NULL, " "));
            int release[NUMBER_OF_RESOURCES];
            int valid_release = 1;

            if (customer_num < 0 || customer_num >= NUMBER_OF_CUSTOMERS)
            {
                printf("Invalid customer number\n");
                continue;
            }
            for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
            {
                release[i] = atoi(strtok(NULL, " "));
                if (release[i] < 0)
                {
                    printf("Release must be non-negative.\n");
                    valid_release = 0;
                    break;
                }
                else if (release[i] > allocation[customer_num][i])
                {
                    printf("Release exceeds allocation.\n");
                    valid_release = 0;
                    break;
                
                }
            }

            if (valid_release)
                release_resources(customer_num, release);
        }
        /* Print Status */
        else if (strcmp(token, "*\n") == 0)
            print_status();
        else
            printf("Unknown command.\n");
    }

    return 0;
}

void request_resources(int customer_num, int request[])
{
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
        if (request[i] > need[customer_num][i])
        {
            printf("Request exceeds need.\n");
            return;
        }

    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
        if (request[i] > available[i])
        {
            printf("Request exceeds available.\n");
            return;
        }

    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
    {
        available[i]                -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i]       -= request[i];
    }

    if (check_safe())
        printf("Successfully allocated resources.\n");
    else
    {
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
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
        if (release[i] > allocation[customer_num][i])
        {
            printf("Release exceeds allocation.\n");
            return;
        }

    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
    {
        available[i]                += release[i];
        allocation[customer_num][i] -= release[i];
        need[customer_num][i]       += release[i];
    }
    printf("Successfully released resources.\n");
}

void print_status()
{
    printf("Available:\n");
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
        printf("%d ", available[i]);
    printf("\n");

    printf("Maximum:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
    {
        for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
            printf("%d ", maximum[i][j]);
        printf("\n");
    }

    printf("Allocation:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
    {
        for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
            printf("%d ", allocation[i][j]);
        printf("\n");
    }

    printf("Need:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
    {
        for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
            printf("%d ", need[i][j]);
        printf("\n");
    }
}

int check_safe(void)
{
    int work[NUMBER_OF_RESOURCES];
    int finish[NUMBER_OF_CUSTOMERS];
    int finish_count = 0;

    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
        work[i] = available[i];
    memset(finish, 0, sizeof(finish));

    while (finish_count < NUMBER_OF_CUSTOMERS)
    {
        int found = 0;

        for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
            if (!finish[i])
            {
                int enough = 1;
                for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
                    if (need[i][j] > work[j])
                    {
                        enough = 0;
                        break;
                    }
                if (enough)
                {
                    found     = 1;
                    finish[i] = 1;
                    ++finish_count;
                    for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
                        work[j] += allocation[i][j];
                }
            }

        if (!found)
            return 0;
    }

    return 1;
}

void check_finish(int customer_num)
{
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
        if (need[customer_num][i] != 0)
            return;

    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
    {
        available[i]                += allocation[customer_num][i];
        allocation[customer_num][i]  = 0;
        need[customer_num][i]        = 0;
        maximum[customer_num][i]     = 0;
    }

    printf("Customer %d has finished, all resources released.\n", customer_num);
}