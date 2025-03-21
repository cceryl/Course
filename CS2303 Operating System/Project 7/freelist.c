#include "freelist.h"

#include <stdio.h>
#include <stdlib.h>

/// @brief Internal function: insert a new node after the given node by address order
static void freelist_internal_insert(freelist_node_t *node, freelist_node_t *new_node);
/// @brief Internal function: remove the given node from the list
static void freelist_internal_remove(freelist_node_t *node);
/// @brief Internal function: try to merge the node with the address adjacent nodes
static void freelist_internal_merge(freelist_t *list, freelist_node_t *node);
/// @brief Internal function: sort the given node by size order
static void freelist_internal_sort(freelist_t *list, freelist_node_t *node);
#ifdef DEBUG
/// @brief Internal function: check the validity of the list
static int freelist_internal_check(freelist_t *list);
#endif

void freelist_create(freelist_t *list, ptrdiff_t address, size_t size)
{
    /* Record memory information */
    list->address = address;
    list->size    = size;

    /* Initialize the base node */
    list->node          = malloc(sizeof(freelist_node_t));
    list->node->address = 0;
    list->node->size    = 0;

    /* Initialize the memory node */
    freelist_node_t *memory_node = malloc(sizeof(freelist_node_t));
    memory_node->address         = address;
    memory_node->size            = size;

    /* Link the nodes */
    list->node->prev     = memory_node;
    list->node->next     = memory_node;
    list->node->smaller  = memory_node;
    list->node->larger   = memory_node;
    memory_node->prev    = list->node;
    memory_node->next    = list->node;
    memory_node->smaller = list->node;
    memory_node->larger  = list->node;
}

void freelist_destroy(freelist_t *list)
{
    /* Free all the nodes */
    freelist_node_t *node = list->node->next;
    while (node != list->node)
    {
        freelist_node_t *next = node->next;
        free(node);
        node = next;
    }
    free(list->node);

    /* Reset the list */
    list->node    = NULL;
    list->address = 0;
    list->size    = 0;
}

void freelist_alloc(freelist_t *list, freelist_node_t *node, size_t size)
{
#ifdef DEBUG
    if (size == 0)
    {
        printf("Error: size is zero in freelist_alloc\n");
        return;
    }

    if (node->size < size)
    {
        printf("Error: size of allocation too large in freelist_alloc\n");
        return;
    }
#endif

    /* Allocate memory */
    if (node->size > size)
    {
        node->address += size;
        node->size    -= size;
        freelist_internal_sort(list, node);
    }
    else
        freelist_internal_remove(node);
}

void freelist_free(freelist_t *list, ptrdiff_t address, size_t size)
{
#ifdef DEBUG
    if (address < list->address || address + size > list->address + list->size)
    {
        printf("Error: address out of range in freelist_free\n");
        return;
    }

    if (size == 0)
    {
        printf("Error: size is zero in freelist_free\n");
        return;
    }
#endif

    /* Create a new node */
    freelist_node_t *new_node = malloc(sizeof(freelist_node_t));
    new_node->address         = address;
    new_node->size            = size;
    new_node->smaller         = NULL;
    new_node->larger          = NULL;

    /* Find the right position */
    freelist_node_t *node = list->node;
    while (node->next != list->node && node->next->address < address)
        node = node->next;

    /* Insert the new node */
    freelist_internal_insert(node, new_node);
    freelist_internal_merge(list, new_node);
}

#ifdef DEBUG
void freelist_print(freelist_t *list)
{
    /* Check errors */
    if (freelist_internal_check(list))
        printf("Validity check failed\n");
    else
        printf("Validity check passed\n");

    /* List information */
    printf("List information: address range = [%td, %td], size = %zu\n", list->address, list->address + list->size - 1, list->size);

    /* Print the list by address */
    freelist_node_t *node = list->node->next;
    int              i    = 0;
    printf("By address:\n");
    while (node != list->node)
    {
        printf("    Node %d: address range = [%td, %td], size = %zu\n", i, node->address, node->address + node->size - 1, node->size);
        node = node->next;
        ++i;
    }

    /* Print the list by size */
    node = list->node->smaller;
    i    = 0;
    printf("By size:\n");
    while (node != list->node)
    {
        printf("    Node %d: size = %zu, address range = [%td, %td]\n", i, node->size, node->address, node->address + node->size - 1);
        node = node->smaller;
        ++i;
    }
}
#endif

static void freelist_internal_insert(freelist_node_t *node, freelist_node_t *new_node)
{
    /* Link the new node */
    new_node->prev   = node;
    new_node->next   = node->next;
    node->next->prev = new_node;
    node->next       = new_node;
}

static void freelist_internal_remove(freelist_node_t *node)
{
    /* Unlink the node */
    node->prev->next = node->next;
    node->next->prev = node->prev;
    if (node->smaller != NULL && node->larger != NULL)
    {
        node->smaller->larger = node->larger;
        node->larger->smaller = node->smaller;
    }

    /* Free the node */
    free(node);
}

static void freelist_internal_merge(freelist_t *list, freelist_node_t *node)
{
    /* Try to merge with previous nodes */
    freelist_node_t *prev = node->prev;
    while (prev != list->node && prev->address + (ptrdiff_t)prev->size == node->address)
    {
        prev->size += node->size;
        freelist_internal_remove(node);
        node = prev;
        prev = prev->prev;
    }

    /* Try to merge with next nodes */
    freelist_node_t *next = node->next;
    while (next != list->node && node->address + (ptrdiff_t)node->size == next->address)
    {
        node->size += next->size;
        freelist_internal_remove(next);
        next = next->next;
    }

    freelist_internal_sort(list, node);
}

static void freelist_internal_sort(freelist_t *list, freelist_node_t *node)
{
    /* Detach the node from the list by size order*/
    if (node->smaller != NULL && node->larger != NULL)
    {
        node->smaller->larger = node->larger;
        node->larger->smaller = node->smaller;
    }

    /* Find the right position */
    freelist_node_t *current = list->node;
    while (current->larger != list->node && current->larger->size < node->size)
        current = current->larger;

    /* Insert the new node */
    node->larger             = current->larger;
    node->smaller            = current;
    current->larger->smaller = node;
    current->larger          = node;
}

#ifdef DEBUG
static int freelist_internal_check(freelist_t *list)
{
    /* Check pointers */
    freelist_node_t *node = list->node;
    if (node->next == NULL || node->prev == NULL || node->smaller == NULL || node->larger == NULL)
    {
        printf("Error: NULL pointers in the base node\n");
        return 1;
    }
    if (node->next->prev != node || node->prev->next != node || node->smaller->larger != node || node->larger->smaller != node)
    {
        printf("Error: wrong pointers in the base node\n");
        return 1;
    }
    node = node->next;
    while (node != list->node)
    {
        if (node->next == NULL || node->prev == NULL || node->smaller == NULL || node->larger == NULL)
        {
            printf("Error: NULL pointers in a data node\n");
            return 1;
        }
        if (node->next->prev != node || node->prev->next != node || node->smaller->larger != node || node->larger->smaller != node)
        {
            printf("Error: wrong pointers in a data node\n");
            return 1;
        }
        node = node->next;
    }

    /* Check address range */
    node = list->node->next;
    while (node != list->node)
    {
        if (node->address < list->address || node->address + node->size > list->address + list->size)
        {
            printf("Error: invalid address range in a data node\n");
            return 1;
        }
        node = node->next;
    }

    /* Check size range */
    node = list->node->next;
    while (node != list->node)
    {
        if (node->size == 0)
        {
            printf("Error: zero size in a data node\n");
            return 1;
        }
        else if (node->size > list->size)
        {
            printf("Error: size too large in a data node\n");
            return 1;
        }
        node = node->next;
    }

    /* Check address order */
    node = list->node->next;
    while (node->next != list->node)
    {
        if (node->address >= node->next->address)
        {
            printf("Error: wrong address order in the list\n");
            return 1;
        }
        else if (node->address + (ptrdiff_t)node->size > node->next->address)
        {
            printf("Error: overlapping address in the list\n");
            return 1;
        }
        node = node->next;
    }

    /* Check size order */
    node = list->node->smaller;
    while (node->smaller != list->node)
    {
        if (node->size < node->smaller->size)
        {
            printf("Error: wrong size order in the list\n");
            return 1;
        }
        node = node->smaller;
    }

    /* Check possible merge */
    node = list->node->next;
    while (node != list->node)
    {
        if (node->address + (ptrdiff_t)node->size == node->next->address)
        {
            printf("Error: possible merge not done in the list\n");
            return 1;
        }
        node = node->next;
    }

    return 0;
}
#endif