#include "policy.h"

ptrdiff_t first_fit(freelist_t *list, size_t size)
{
    /* Search for the first node that can fit the allocation */
    freelist_node_t *node = list->node->next;
    while (node != list->node)
    {
        if (node->size >= size)
        {
            ptrdiff_t address = node->address;
            freelist_alloc(list, node, size);
            return address;
        }
        node = node->next;
    }
    return -1;
}

ptrdiff_t best_fit(freelist_t *list, size_t size)
{
    /* Search for the smallest node that can fit the allocation */
    freelist_node_t *node = list->node->larger;
    while (node != list->node)
    {
        if (node->size >= size)
        {
            ptrdiff_t address = node->address;
            freelist_alloc(list, node, size);
            return address;
        }
        node = node->larger;
    }
    return -1;
}

ptrdiff_t worst_fit(freelist_t *list, size_t size)
{
    /* Search for the largest node that can fit the allocation */
    freelist_node_t *node = list->node->smaller;
    if (node->size >= size)
    {
        ptrdiff_t address = node->address;
        freelist_alloc(list, node, size);
        return address;
    }
    return -1;
}
