#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"

#define COLOR_BLUE   "\x1b[34m"
#define COLOR_RESET  "\x1b[0m"
#define FONT_BOLD    "\x1b[1m"
#define FONT_NORMAL  "\x1b[22m"
#define STYLE_PROMPT COLOR_BLUE FONT_BOLD
#define STYLE_RESET  COLOR_RESET FONT_NORMAL

#define MEMORY_SIZE (1 << 20)
#define PAGE_SIZE   (4096)
#define BUFFER_SIZE (128)

#define PRINT_RQ_USAGE() printf("Usage: RQ <block_name> <block_size> <allocate_policy: F/B/W>\n")
#define CHECK_RQ_TOKEN()                                                                                                                   \
    if (token == NULL)                                                                                                                     \
    {                                                                                                                                      \
        PRINT_RQ_USAGE();                                                                                                                  \
        continue;                                                                                                                          \
    }
#define PRINT_RL_USAGE() printf("Usage: RL <block_name>\n")
#define CHECK_RL_TOKEN()                                                                                                                   \
    if (token == NULL)                                                                                                                     \
    {                                                                                                                                      \
        PRINT_RL_USAGE();                                                                                                                  \
        continue;                                                                                                                          \
    }
#define PRINT_STAT_USAGE() printf("Usage: STAT\n")
#define CHECK_STAT_TOKEN()                                                                                                                 \
    if (token != NULL)                                                                                                                     \
    {                                                                                                                                      \
        PRINT_STAT_USAGE();                                                                                                                \
        continue;                                                                                                                          \
    }
#define PRINT_UNKNOWN_CMD() printf("Unknown command: type HELP for more information\n")
#define PRINT_HELP()                                                                                                                       \
    printf("HELP:\n");                                                                                                                     \
    printf("Request a memory block:\n");                                                                                                   \
    PRINT_RQ_USAGE();                                                                                                                      \
    printf("Release a memory block:\n");                                                                                                   \
    PRINT_RL_USAGE();                                                                                                                      \
    printf("Print the status of the memory:\n");                                                                                           \
    PRINT_STAT_USAGE();                                                                                                                    \
    printf("Exit the program:\n");                                                                                                         \
    printf("EXIT\n");

void command_line_interface(allocator_t *allocator);

int main()
{
    allocator_t allocator;
    char        memory[MEMORY_SIZE];
    allocator_create(&allocator, memory, sizeof(memory), PAGE_SIZE);

    command_line_interface(&allocator);

    allocator_destroy(&allocator);
    return 0;
}

void command_line_interface(allocator_t *allocator)
{
    char buffer[BUFFER_SIZE];

    while (1)
    {
        printf(STYLE_PROMPT "allocator > " STYLE_RESET);
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);

        char *token = strtok(buffer, " \n");
        if (token == NULL)
            continue;

        /* Help */
        if (strcmp(token, "HELP") == 0)
        {
            PRINT_HELP();
            continue;
        }
        /* Request */
        else if (strcmp(token, "RQ") == 0)
        {
            token = strtok(NULL, " \n");
            CHECK_RQ_TOKEN();
            char *block_name = token;
            if (allocator_address(allocator, block_name) != NULL)
            {
                printf("Memory block \"%s\" already exists\n", block_name);
                continue;
            }

            token = strtok(NULL, " \n");
            CHECK_RQ_TOKEN();
            int block_size = atoi(token);
            if (block_size <= 0)
            {
                printf("Block size must be a positive integer\n");
                continue;
            }

            token = strtok(NULL, " \n");
            CHECK_RQ_TOKEN();
            char     policy_name = token[0];
            policy_t policy;
            switch (policy_name)
            {
            case 'F': policy = first_fit; break;
            case 'B': policy = best_fit; break;
            case 'W': policy = worst_fit; break;
            default:  printf("Unknown allocation policy: %c\n", policy_name); continue;
            }

            token = strtok(NULL, " \n");
            if (token != NULL)
            {
                PRINT_RQ_USAGE();
                continue;
            }

            if (allocator_alloc(allocator, block_name, block_size, policy) == NULL)
                printf("Memory allocation failed\n");
        }
        /* Release */
        else if (strcmp(token, "RL") == 0)
        {
            token = strtok(NULL, " \n");
            CHECK_RL_TOKEN();
            char *block_name = token;

            token = strtok(NULL, " \n");
            if (token != NULL)
            {
                PRINT_RL_USAGE();
                continue;
            }

            if (allocator_address(allocator, block_name) == NULL)
                printf("Memory block \"%s\" does not exist\n", block_name);
            else
                allocator_free(allocator, block_name);
        }
        /* Status */
        else if (strcmp(token, "STAT") == 0)
        {
            token = strtok(NULL, " \n");
            if (token != NULL)
            {
                PRINT_STAT_USAGE();
                continue;
            }

            allocator_print(allocator);
        }
        /* Exit */
        else if (strcmp(token, "EXIT") == 0)
            break;
        else
            PRINT_UNKNOWN_CMD();
    }
}