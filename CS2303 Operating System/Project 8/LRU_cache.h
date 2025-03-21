#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef void LRU_data;
typedef bool (*equal_t)(LRU_data *a, LRU_data *b);

/// @brief LRU Cache node
typedef struct LRU_cache_node
{
    LRU_data              *data;
    struct LRU_cache_node *prev;
    struct LRU_cache_node *next;
} LRU_cache_node_t;

/// @brief LRU Cache
typedef struct LRU_cache
{
    LRU_cache_node_t *head;
    size_t            size;
    size_t            capacity;
    equal_t           data_equal;
} LRU_cache_t;

/// @brief Create a new LRU cache
/// @param data_equal The function to compare the LRU data
void LRU_cache_create(LRU_cache_t *LRU, size_t capacity, equal_t data_equal);

/// @brief Destroy the LRU cache
void LRU_cache_destroy(LRU_cache_t *LRU);

/// @brief Clear the LRU cache
void LRU_cache_clear(LRU_cache_t *LRU);

/// @brief Get the size of the LRU cache
size_t LRU_cache_size(LRU_cache_t *LRU);

/// @brief Get the capacity of the LRU cache
size_t LRU_cache_capacity(LRU_cache_t *LRU);

/// @brief Check if the LRU cache is empty
bool LRU_cache_empty(LRU_cache_t *LRU);

/// @brief Check if the LRU cache is full
bool LRU_cache_full(LRU_cache_t *LRU);

/// @brief Query the LRU cache for the cache data that matches the key
/// @return The cache data if found, NULL otherwise
LRU_data *LRU_cache_query(LRU_cache_t *LRU, LRU_data *key);

/// @brief Insert the data into the LRU cache, if there exists equivalent data, it will be updated
/// @return The removed data if the LRU cache is full, NULL otherwise
LRU_data *LRU_cache_insert(LRU_cache_t *LRU, LRU_data *data);

/// @brief Get the least recently used data from the LRU cache, will not update the LRU cache
/// @return The least recently used data if the LRU cache is not empty, NULL otherwise
LRU_data *LRU_cache_least_recently_used(LRU_cache_t *LRU);

/// @brief Remove the data from the LRU cache
/// @return The removed data if found, NULL otherwise
LRU_data *LRU_cache_remove(LRU_cache_t *LRU, LRU_data *key);

/// @brief Print the LRU cache
typedef void (*print_t)(LRU_data *data);
void LRU_cache_print(LRU_cache_t *LRU, print_t print_func);