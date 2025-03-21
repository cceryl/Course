#pragma once

#include <stddef.h>

/// @brief A node in a freelist
typedef struct freelist_node
{
    ptrdiff_t             address;
    size_t                size;
    struct freelist_node *prev;
    struct freelist_node *next;
    struct freelist_node *smaller;
    struct freelist_node *larger;
} freelist_node_t;

/// @brief An ordered cross linked list for managing free memory
typedef struct freelist
{
    freelist_node_t *node;
    ptrdiff_t        address;
    size_t           size;
} freelist_t;

/// @brief Create a freelist
/// @param list The freelist to initialize
/// @param address Address of the initial free block
/// @param size Size of the initial free block
void freelist_create(freelist_t *list, ptrdiff_t address, size_t size);

/// @brief Destroy a freelist
/// @param list The freelist to destroy
void freelist_destroy(freelist_t *list);

/// @brief Allocate memory from the node
/// @param list The freelist to allocate from
/// @param node The node to allocate from
/// @param size Size of the allocation
void freelist_alloc(freelist_t *list, freelist_node_t *node, size_t size);

/// @brief Free memory to the freelist
/// @param list The freelist to add to
/// @param address Address of the free block
/// @param size Size of the free block
void freelist_free(freelist_t *list, ptrdiff_t address, size_t size);

#ifdef DEBUG
/// @brief Print the freelist
/// @param list The freelist to print
void freelist_print(freelist_t *list);
#endif