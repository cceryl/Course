#pragma once

#include "alloclist.h"
#include "freelist.h"
#include "policy.h"

/// @brief A memory allocator that manages a block of allocated memory
typedef struct allocator
{
    void       *memory;
    size_t      size;
    size_t      page_size;
    alloclist_t alloclist;
    freelist_t  freelist;
} allocator_t;

/// @brief Create a new allocator
/// @param allocator The allocator to create
/// @param memory The memory to manage
/// @param size The size of the memory
/// @param page_size The size of the memory page, must be a power of 2
void allocator_create(allocator_t *allocator, void *memory, size_t size, size_t page_size);

/// @brief Destroy an allocator
/// @param allocator The allocator to destroy
void allocator_destroy(allocator_t *allocator);

/// @brief Allocate a new memory block
/// @param allocator The allocator to allocate from
/// @param name Name of the memory block
/// @param size Size of the memory block
/// @param alloc_policy Policy function to use for allocation
/// @return Address of the memory block, or NULL if the allocation failed
void *allocator_alloc(allocator_t *allocator, const char *name, size_t size, policy_t alloc_policy);

/// @brief Free a memory block, if multiple blocks have the same name, free the first one
/// @param allocator The allocator to free from
/// @param name Name of the memory block
void allocator_free(allocator_t *allocator, const char *name);

/// @brief Get the address of the memory block
/// @param allocator The allocator to query
/// @param name Name of the memory block
/// @return Address of the memory block, or NULL if the block does not exist
void *allocator_address(allocator_t *allocator, const char *name);

/// @brief Get the size of the memory block
/// @param allocator The allocator to query
/// @param name Name of the memory block
/// @return Size of the memory block, or 0 if the block does not exist
size_t allocator_size(allocator_t *allocator, const char *name);

/// @brief Print memory allocation information
/// @param allocator The allocator to print
void allocator_print(allocator_t *allocator);