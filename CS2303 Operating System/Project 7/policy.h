#pragma once

#include "freelist.h"

typedef ptrdiff_t (*policy_t)(freelist_t*, size_t);

/// @brief Use the first fit policy to find a free block
/// @param list The freelist to search
/// @param size Size of the allocation
/// @return The address of the allocation, or -1 if the allocation failed
ptrdiff_t first_fit(freelist_t *list, size_t size);

/// @brief Use the best fit policy to find a free block
/// @param list The freelist to search
/// @param size Size of the allocation
/// @return The address of the allocation, or -1 if the allocation failed
ptrdiff_t best_fit(freelist_t *list, size_t size);

/// @brief Use the worst fit policy to find a free block
/// @param list The freelist to search
/// @param size Size of the allocation
/// @return The address of the allocation, or -1 if the allocation failed
ptrdiff_t worst_fit(freelist_t *list, size_t size);