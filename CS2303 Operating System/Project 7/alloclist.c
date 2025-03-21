#include "alloclist.h"

#include <stdlib.h>
#include <string.h>

void alloclist_create(alloclist_t *list) { list->head = NULL; }

void alloclist_destroy(alloclist_t *list)
{
    alloclist_node_t *node = list->head;
    while (node != NULL)
    {
        alloclist_node_t *next = node->next;
        free(node->info.name);
        free(node);
        node = next;
    }
}

void alloclist_alloc(alloclist_t *list, const char *name, ptrdiff_t address, size_t size)
{
    alloclist_node_t *node = malloc(sizeof(alloclist_node_t));
    node->info.name        = malloc(strlen(name) + 1);
    strcpy(node->info.name, name);
    node->info.address = address;
    node->info.size    = size;

    if (list->head == NULL || list->head->info.address > address)
    {
        node->next = list->head;
        list->head = node;
        return;
    }

    alloclist_node_t *prev = list->head;
    alloclist_node_t *curr = list->head->next;
    while (curr != NULL && curr->info.address < address)
    {
        prev = curr;
        curr = curr->next;
    }
    prev->next = node;
    node->next = curr;
}

alloc_info_t alloclist_free(alloclist_t *list, const char *name)
{
    alloclist_node_t *node = list->head;
    alloclist_node_t *prev = NULL;
    while (node != NULL)
    {
        if (strcmp(node->info.name, name) == 0)
        {
            if (prev == NULL)
                list->head = node->next;
            else
                prev->next = node->next;
            alloc_info_t info = node->info;
            free(node->info.name);
            free(node);
            return info;
        }
        prev = node;
        node = node->next;
    }
    alloc_info_t info = {NULL, -1, 0};
    return info;
}

ptrdiff_t alloclist_address(alloclist_t *list, const char *name)
{
    alloclist_node_t *node = list->head;
    while (node != NULL)
    {
        if (strcmp(node->info.name, name) == 0)
            return node->info.address;
        node = node->next;
    }
    return -1;
}

size_t alloclist_size(alloclist_t *list, const char *name)
{
    alloclist_node_t *node = list->head;
    while (node != NULL)
    {
        if (strcmp(node->info.name, name) == 0)
            return node->info.size;
        node = node->next;
    }
    return 0;
}