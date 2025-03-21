#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "LRU_cache.h"

/// @brief TLB entry structure
typedef struct TLB_entry
{
    size_t page_number;
    size_t frame_number;
    bool   valid;
} TLB_entry_t;

/// @brief TLB structure
typedef struct TLB
{
    TLB_entry_t *entries;
    LRU_cache_t  cache;
    size_t       hit_count;
    size_t       miss_count;
} TLB_t;

/// @brief Create a TLB
void TLB_create(TLB_t *TLB, size_t capacity);

/// @brief Destroy a TLB
void TLB_destroy(TLB_t *TLB);

/// @brief Clear a TLB
void TLB_clear(TLB_t *TLB);

/// @brief Query for the frame number of a page number in the TLB
/// @return The frame number if the page number is in the TLB, -1 otherwise
int TLB_query(TLB_t *TLB, size_t page_number);

/// @brief Insert a page number and frame number into the TLB
void TLB_insert(TLB_t *TLB, size_t page_number, size_t frame_number);

/// @brief Remove a page number from the TLB
/// @return True if the page number was removed, false if such page number was not in the TLB
bool TLB_remove(TLB_t *TLB, size_t page_number);

/// @brief Get number of valid entries in the TLB
size_t TLB_size(TLB_t *TLB);

/// @brief Get the capacity of the TLB
size_t TLB_capacity(TLB_t *TLB);

/// @brief Check if the TLB is empty
bool TLB_empty(TLB_t *TLB);

/// @brief Check if the TLB is full
bool TLB_full(TLB_t *TLB);

/// @brief Get the number of TLB hits
size_t TLB_hit_count(TLB_t *TLB);

/// @brief Get the number of TLB misses
size_t TLB_miss_count(TLB_t *TLB);

/// @brief Print the TLB
void TLB_print(TLB_t *TLB);