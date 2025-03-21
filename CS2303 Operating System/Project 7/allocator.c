#include "allocator.h"

#include <stdio.h>

/// @brief Round up a size to the multiple of allocator page size
static size_t allocator_internal_roundup(allocator_t *allocator, size_t size);

void allocator_create(allocator_t *allocator, void *memory, size_t size, size_t page_size)
{
#ifdef DEBUG
    if ((page_size & (page_size - 1)) != 0)
    {
        printf("allocator_create: page_size is not a power of 2\n");
        return;
    }
#endif

    allocator->memory    = memory;
    allocator->size      = size;
    allocator->page_size = page_size;
    alloclist_create(&allocator->alloclist);
    freelist_create(&allocator->freelist, 0, size);
}

void allocator_destroy(allocator_t *allocator)
{
    allocator->memory = NULL;
    allocator->size   = 0;
    alloclist_destroy(&allocator->alloclist);
    freelist_destroy(&allocator->freelist);
}

void *allocator_alloc(allocator_t *allocator, const char *name, size_t size, policy_t alloc_policy)
{
    size_t    rounded_size = allocator_internal_roundup(allocator, size);
    ptrdiff_t address      = alloc_policy(&allocator->freelist, rounded_size);
    if (address == -1)
        return NULL;
    alloclist_alloc(&allocator->alloclist, name, address, rounded_size);
    return (char *)allocator->memory + address;
}

void allocator_free(allocator_t *allocator, const char *name)
{
    alloc_info_t info = alloclist_free(&allocator->alloclist, name);
    if (info.address != -1)
        freelist_free(&allocator->freelist, info.address, info.size);
}

void *allocator_address(allocator_t *allocator, const char *name)
{
    ptrdiff_t address = alloclist_address(&allocator->alloclist, name);
    if (address == -1)
        return NULL;
    return (char *)allocator->memory + address;
}

size_t allocator_size(allocator_t *allocator, const char *name) { return alloclist_size(&allocator->alloclist, name); }

void allocator_print(allocator_t *allocator)
{
    printf("allocator memory pointer: %p\n", allocator->memory);
    printf("allocator size: %zu bytes\n", allocator->size);
    printf("allocator page size: %zu bytes\n", allocator->page_size);

    printf("allocator allocation list:\n");
    alloclist_node_t *alloc_node = allocator->alloclist.head;
    freelist_node_t  *free_node  = allocator->freelist.node->next;
    while (alloc_node != NULL && free_node != allocator->freelist.node)
    {
        if (alloc_node->info.address < free_node->address)
        {
            printf("  Address [%7zu:%7zu]   %8s     Size = %zu\n", alloc_node->info.address,
                   alloc_node->info.address + alloc_node->info.size - 1, alloc_node->info.name, alloc_node->info.size);
            alloc_node = alloc_node->next;
        }
        else
        {
            printf("  Address [%7zu:%7zu]   %8s     Size = %zu\n", free_node->address, free_node->address + free_node->size - 1, "Unused",
                   free_node->size);
            free_node = free_node->next;
        }
    }
    while (alloc_node != NULL)
    {
        printf("  Address [%7zu:%7zu]   %8s     Size = %zu\n", alloc_node->info.address,
               alloc_node->info.address + alloc_node->info.size - 1, alloc_node->info.name, alloc_node->info.size);
        alloc_node = alloc_node->next;
    }
    while (free_node != allocator->freelist.node)
    {
        printf("  Address [%7zu:%7zu]   %8s     Size = %zu\n", free_node->address, free_node->address + free_node->size - 1, "Unused",
               free_node->size);
        free_node = free_node->next;
    }
}

static size_t allocator_internal_roundup(allocator_t *allocator, size_t size)
{
    /* This works iff allocator->page_size is a power of 2 */
    return (size + allocator->page_size - 1) & ~(allocator->page_size - 1);
}