#include "virtual_memory.h"

#include <stdio.h>
#include <string.h>

#define logical_address_space (1 << 16)
#define page_table_capacity   (1 << 8)
#define page_size             (1 << 8)
#define TLB_size              (16)
#define frame_capacity        (256)
#define physical_memory_size  (frame_capacity * page_size)
#define query_file_path       "addresses.txt"
#define answer_file_path      "answers.txt"
#define correct_file_path     "correct.txt"
#define backing_store_path    "BACKING_STORE.bin"

void test(virtual_memory_t *virtual_memory);
void check();

int main()
{
    uint8_t memory[physical_memory_size];

    virtual_memory_t virtual_memory;
    virtual_memory_create(&virtual_memory, page_size, page_table_capacity, TLB_size, physical_memory_size, memory, backing_store_path);
    virtual_memory_set_log(&virtual_memory, true, answer_file_path);

    test(&virtual_memory);
    printf("Total accesses: %zu\n", virtual_memory.total_accesses);
    printf("Page faults: %zu, page fault rate: %.2f%%\n", virtual_memory.page_faults,
           (double)virtual_memory.page_faults / virtual_memory.total_accesses * 100.0);
    printf("TLB hits: %zu, TLB hit rate: %.2f%%\n", TLB_hit_count(&virtual_memory.TLB),
           (double)TLB_hit_count(&virtual_memory.TLB) / (TLB_hit_count(&virtual_memory.TLB) + TLB_miss_count(&virtual_memory.TLB)) * 100.0);

    virtual_memory_destroy(&virtual_memory);

    check();
    return 0;
}

void test(virtual_memory_t *virtual_memory)
{
    FILE *clear_answer_file = fopen(answer_file_path, "w");
    fclose(clear_answer_file);

    FILE *query_file = fopen(query_file_path, "r");
    fseek(query_file, 0, SEEK_SET);
    int logical_address;
    while (fscanf(query_file, "%d", &logical_address) != EOF)
        virtual_memory_read(virtual_memory, logical_address);
}

void check()
{
    FILE *correct_file = fopen(correct_file_path, "r");
    FILE *answer_file  = fopen(answer_file_path, "r");
    fseek(correct_file, 0, SEEK_SET);
    fseek(answer_file, 0, SEEK_SET);
    char correct_line[256];
    char answer_line[256];
    int  line = 1;
    while (fgets(correct_line, 256, correct_file) != NULL && fgets(answer_line, 256, answer_file) != NULL)
    {
        if (strcmp(correct_line, answer_line) != 0)
        {
            printf("Answer is incorrect at line %d\n", line);
            return;
        }
        ++line;
    }
    printf("Answer is correct\n");
}