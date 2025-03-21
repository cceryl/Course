#ifndef IMPL_H
#define IMPL_H

#include "config.h"

#define TILE_SIZE 32
// #define ENABLE_COMPILER_OPTIMIZATION

void optimized_matmul_impl(const int *const restrict L, const int *const restrict R, int *const restrict result);

#endif