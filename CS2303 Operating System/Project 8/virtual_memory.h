#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "LRU_cache.h"
#include "TLB.h"
#include "physical_memory.h"

/// @brief The virtual memory page table entry
typedef struct virtual_memory_page_table_entry
{
    size_t page_number;
    size_t frame_number;
    bool   valid;
} virtual_memory_page_table_entry_t;

/// @brief The virtual memory
typedef struct virtual_memory
{
    size_t                             page_size;
    virtual_memory_page_table_entry_t *page_table;
    size_t                             page_table_capacity;

    LRU_cache_t       page_cache;
    TLB_t             TLB;
    physical_memory_t physical_memory;
    FILE             *backing_store;

    size_t page_faults;
    size_t total_accesses;

    bool  log_enabled;
    FILE *log_file;
} virtual_memory_t;

/// @brief Creates a virtual memory
void virtual_memory_create(virtual_memory_t *virtual_memory, size_t page_size, size_t page_table_capacity, size_t TLB_capacity,
                           size_t physical_memory_capacity, void *physical_memory_ptr, const char *backing_store_filename);

/// @brief Destroys a virtual memory
void virtual_memory_destroy(virtual_memory_t *virtual_memory);

/// @brief Read a byte from the virtual memory
int8_t virtual_memory_read(virtual_memory_t *virtual_memory, ptrdiff_t address);

/// @brief Write a byte to the virtual memory, do nothing if the logical address is invalid
void virtual_memory_write(virtual_memory_t *virtual_memory, ptrdiff_t address, int8_t value);

/// @brief Get physical address from virtual address
ptrdiff_t virtual_memory_get_physical_address(virtual_memory_t *virtual_memory, ptrdiff_t address);

/// @brief Set log file
void virtual_memory_set_log(virtual_memory_t *virtual_memory, bool enabled, const char *filename);