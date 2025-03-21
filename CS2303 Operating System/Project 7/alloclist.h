#pragma once

#include <stddef.h>

/// @brief Information about an allocated memory block
typedef struct alloc_info
{
    char     *name;
    ptrdiff_t address;
    size_t    size;
} alloc_info_t;

/// @brief Node in the alloclist
typedef struct alloclist_node
{
    alloc_info_t           info;
    struct alloclist_node *next;
} alloclist_node_t;

/// @brief A linked list of allocated memory blocks
typedef struct alloclist
{
    alloclist_node_t *head;
} alloclist_t;

/// @brief Create a new alloclist
/// @param list The alloclist to create
void alloclist_create(alloclist_t *list);

/// @brief Destroy an alloclist
/// @param list  The alloclist to destroy
void alloclist_destroy(alloclist_t *list);

/// @brief Allocate a new memory block
/// @param list The alloclist to allocate from
/// @param name Name of the memory block
/// @param address Address of the memory block
/// @param size Size of the memory block
void alloclist_alloc(alloclist_t *list, const char *name, ptrdiff_t address, size_t size);

/// @brief Free a memory block
/// @param list The alloclist to free from
/// @param name Name of the memory block
/// @return Information about the freed memory block
alloc_info_t alloclist_free(alloclist_t *list, const char *name);

/// @brief Get the address of the memory block
/// @param list The alloclist to query
/// @param name Name of the memory block
/// @return Address of the memory block, or -1 if not found
ptrdiff_t alloclist_address(alloclist_t *list, const char *name);

/// @brief Get the size of the memory block
/// @param list The alloclist to query
/// @param name Name of the memory block
/// @return Size of the memory block, or 0 if not found
size_t alloclist_size(alloclist_t *list, const char *name);