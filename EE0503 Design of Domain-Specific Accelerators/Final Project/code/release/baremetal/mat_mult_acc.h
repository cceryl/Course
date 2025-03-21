#ifndef MAT_MULT_ACC_H
#define MAT_MULT_ACC_H

#include <stdint.h>

void mat_mult_acc(const int8_t *mat_l, const int8_t *mat_r, int *result, const unsigned int N, const unsigned int K, const unsigned int M);

#endif