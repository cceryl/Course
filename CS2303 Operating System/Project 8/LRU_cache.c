#include "LRU_cache.h"

#include <stdlib.h>

void LRU_cache_create(LRU_cache_t *LRU, size_t capacity, equal_t data_equal)
{
    LRU->head       = (LRU_cache_node_t *)malloc(sizeof(LRU_cache_node_t));
    LRU->head->prev = LRU->head;
    LRU->head->next = LRU->head;
    LRU->head->data = NULL;

    LRU->size       = 0;
    LRU->capacity   = capacity;
    LRU->data_equal = data_equal;
}

void LRU_cache_destroy(LRU_cache_t *LRU)
{
    LRU_cache_clear(LRU);
    free(LRU->head);
    LRU->head = NULL;
}

void LRU_cache_clear(LRU_cache_t *LRU)
{
    LRU_cache_node_t *node = LRU->head->next;
    while (node != LRU->head)
    {
        LRU_cache_node_t *next = node->next;
        free(node);
        node = next;
    }
    LRU->head->next = LRU->head;
    LRU->head->prev = LRU->head;
    LRU->size       = 0;
}

size_t LRU_cache_size(LRU_cache_t *LRU) { return LRU->size; }

size_t LRU_cache_capacity(LRU_cache_t *LRU) { return LRU->capacity; }

bool LRU_cache_empty(LRU_cache_t *LRU) { return LRU->size == 0; }

bool LRU_cache_full(LRU_cache_t *LRU) { return LRU->size == LRU->capacity; }

LRU_data *LRU_cache_query(LRU_cache_t *LRU, LRU_data *key)
{
    LRU_cache_node_t *node = LRU->head->next;
    while (node != LRU->head)
    {
        if (LRU->data_equal(node->data, key))
        {
            node->prev->next = node->next;
            node->next->prev = node->prev;

            node->prev            = LRU->head->prev;
            node->next            = LRU->head;
            LRU->head->prev->next = node;
            LRU->head->prev       = node;

            return node->data;
        }
        node = node->next;
    }
    return NULL;
}

LRU_data *LRU_cache_insert(LRU_cache_t *LRU, LRU_data *data)
{
    LRU_data *query_data = LRU_cache_query(LRU, data);
    if (query_data != NULL)
    {
        query_data = data;
        return NULL;
    }

    LRU_data *removed_data = NULL;
    if (LRU_cache_full(LRU))
    {
        LRU_cache_node_t *node = LRU->head->next;
        LRU->head->next        = node->next;
        node->next->prev       = LRU->head;
        removed_data           = node->data;
        free(node);
        --LRU->size;
    }

    LRU_cache_node_t *node = (LRU_cache_node_t *)malloc(sizeof(LRU_cache_node_t));
    node->data             = data;
    node->prev             = LRU->head->prev;
    node->next             = LRU->head;
    LRU->head->prev->next  = node;
    LRU->head->prev        = node;
    ++LRU->size;

    return removed_data;
}

LRU_data *LRU_cache_least_recently_used(LRU_cache_t *LRU) { return LRU_cache_empty(LRU) ? NULL : LRU->head->next->data; }

LRU_data *LRU_cache_remove(LRU_cache_t *LRU, LRU_data *key)
{
    LRU_cache_node_t *node         = LRU->head->next;
    LRU_data         *removed_data = NULL;
    while (node != LRU->head)
    {
        if (LRU->data_equal(node->data, key))
        {
            node->prev->next = node->next;
            node->next->prev = node->prev;
            removed_data     = node->data;
            free(node);
            --LRU->size;
            return removed_data;
        }
        node = node->next;
    }
    return NULL;
}

void LRU_cache_print(LRU_cache_t *LRU, print_t print_func)
{
    LRU_cache_node_t *node = LRU->head->next;
    while (node != LRU->head)
    {
        print_func(node->data);
        node = node->next;
    }
}