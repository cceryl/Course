#include <immintrin.h>
#include <stdlib.h>
#include <time.h>

#include "impl.h"

#ifdef ENABLE_COMPILER_OPTIMIZATION
    #define ATTR __attribute__((optimize("Ofast")))
#else
    #define ATTR
#endif

ATTR inline static int vec_dot(const int *restrict a, const int *restrict b, int size)
{
    __m256i sum_0 = _mm256_setzero_si256();
    __m256i sum_1 = _mm256_setzero_si256();
    __m256i sum_2 = _mm256_setzero_si256();
    __m256i sum_3 = _mm256_setzero_si256();
    __m256i sum_4 = _mm256_setzero_si256();
    __m256i sum_5 = _mm256_setzero_si256();
    __m256i sum_6 = _mm256_setzero_si256();
    __m256i sum_7 = _mm256_setzero_si256();

    int remaining_size = size;

    while (remaining_size >= 64)
    {
        __m256i va_0, va_1, va_2, va_3, va_4, va_5, va_6, va_7;
        __m256i vb_0, vb_1, vb_2, vb_3, vb_4, vb_5, vb_6, vb_7;
        va_0 = _mm256_loadu_si256((__m256i *)a);
        va_1 = _mm256_loadu_si256((__m256i *)(a + 8));
        va_2 = _mm256_loadu_si256((__m256i *)(a + 16));
        va_3 = _mm256_loadu_si256((__m256i *)(a + 24));
        va_4 = _mm256_loadu_si256((__m256i *)(a + 32));
        va_5 = _mm256_loadu_si256((__m256i *)(a + 40));
        va_6 = _mm256_loadu_si256((__m256i *)(a + 48));
        va_7 = _mm256_loadu_si256((__m256i *)(a + 56));
        vb_0 = _mm256_loadu_si256((__m256i *)b);
        vb_1 = _mm256_loadu_si256((__m256i *)(b + 8));
        vb_2 = _mm256_loadu_si256((__m256i *)(b + 16));
        vb_3 = _mm256_loadu_si256((__m256i *)(b + 24));
        vb_4 = _mm256_loadu_si256((__m256i *)(b + 32));
        vb_5 = _mm256_loadu_si256((__m256i *)(b + 40));
        vb_6 = _mm256_loadu_si256((__m256i *)(b + 48));
        vb_7 = _mm256_loadu_si256((__m256i *)(b + 56));

        sum_0 = _mm256_add_epi32(sum_0, _mm256_mullo_epi32(va_0, vb_0));
        sum_1 = _mm256_add_epi32(sum_1, _mm256_mullo_epi32(va_1, vb_1));
        sum_2 = _mm256_add_epi32(sum_2, _mm256_mullo_epi32(va_2, vb_2));
        sum_3 = _mm256_add_epi32(sum_3, _mm256_mullo_epi32(va_3, vb_3));
        sum_4 = _mm256_add_epi32(sum_4, _mm256_mullo_epi32(va_4, vb_4));
        sum_5 = _mm256_add_epi32(sum_5, _mm256_mullo_epi32(va_5, vb_5));
        sum_6 = _mm256_add_epi32(sum_6, _mm256_mullo_epi32(va_6, vb_6));
        sum_7 = _mm256_add_epi32(sum_7, _mm256_mullo_epi32(va_7, vb_7));

        a              += 64;
        b              += 64;
        remaining_size -= 64;
    }

    int result = 0;
    for (int i = 0; i < 8; ++i)
    {
        result += ((int *)&sum_0)[i];
        result += ((int *)&sum_1)[i];
        result += ((int *)&sum_2)[i];
        result += ((int *)&sum_3)[i];
        result += ((int *)&sum_4)[i];
        result += ((int *)&sum_5)[i];
        result += ((int *)&sum_6)[i];
        result += ((int *)&sum_7)[i];
    }

    for (int i = 0; i < remaining_size; ++i)
        result += a[i] * b[i];

    return result;
}

ATTR inline static int *tiled_transpose(const int *const restrict matrix)
{
    int *new_matrix = (int *)malloc(R_ROW * R_COL * sizeof(int));

    int i, j;
    for (i = 0; i <= R_ROW - TILE_SIZE; i += TILE_SIZE)
    {
        for (j = 0; j <= R_COL - TILE_SIZE; j += TILE_SIZE)
        {
            const int *tile     = matrix + i * R_COL + j;
            int       *new_tile = new_matrix + j * R_ROW + i;

            for (int k = 0; k < TILE_SIZE; k += 4)
                for (int l = 0; l < TILE_SIZE; l += 4)
                {
                    new_tile[l * R_ROW + k]           = tile[k * R_COL + l];
                    new_tile[(l + 1) * R_ROW + k]     = tile[k * R_COL + l + 1];
                    new_tile[(l + 2) * R_ROW + k]     = tile[k * R_COL + l + 2];
                    new_tile[(l + 3) * R_ROW + k]     = tile[k * R_COL + l + 3];
                    new_tile[l * R_ROW + k + 1]       = tile[(k + 1) * R_COL + l];
                    new_tile[(l + 1) * R_ROW + k + 1] = tile[(k + 1) * R_COL + l + 1];
                    new_tile[(l + 2) * R_ROW + k + 1] = tile[(k + 1) * R_COL + l + 2];
                    new_tile[(l + 3) * R_ROW + k + 1] = tile[(k + 1) * R_COL + l + 3];
                    new_tile[l * R_ROW + k + 2]       = tile[(k + 2) * R_COL + l];
                    new_tile[(l + 1) * R_ROW + k + 2] = tile[(k + 2) * R_COL + l + 1];
                    new_tile[(l + 2) * R_ROW + k + 2] = tile[(k + 2) * R_COL + l + 2];
                    new_tile[(l + 3) * R_ROW + k + 2] = tile[(k + 2) * R_COL + l + 3];
                    new_tile[l * R_ROW + k + 3]       = tile[(k + 3) * R_COL + l];
                    new_tile[(l + 1) * R_ROW + k + 3] = tile[(k + 3) * R_COL + l + 1];
                    new_tile[(l + 2) * R_ROW + k + 3] = tile[(k + 3) * R_COL + l + 2];
                    new_tile[(l + 3) * R_ROW + k + 3] = tile[(k + 3) * R_COL + l + 3];
                }
        }

        const int *tile     = matrix + i * R_COL + j;
        int       *new_tile = new_matrix + j * R_ROW + i;
        for (int k = 0; k < TILE_SIZE; k += 4)
            for (int l = 0; l < R_COL - j; ++l)
            {
                new_tile[l * R_ROW + k]     = tile[k * R_COL + l];
                new_tile[l * R_ROW + k + 1] = tile[(k + 1) * R_COL + l];
                new_tile[l * R_ROW + k + 2] = tile[(k + 2) * R_COL + l];
                new_tile[l * R_ROW + k + 3] = tile[(k + 3) * R_COL + l];
            }
    }

    for (j = 0; j <= R_COL - TILE_SIZE; j += TILE_SIZE)
    {
        const int *tile     = matrix + i * R_COL + j;
        int       *new_tile = new_matrix + j * R_ROW + i;
        for (int k = 0; k < R_ROW - i; ++k)
            for (int l = 0; l < TILE_SIZE; l += 4)
            {
                new_tile[l * R_ROW + k]       = tile[k * R_COL + l];
                new_tile[(l + 1) * R_ROW + k] = tile[k * R_COL + l + 1];
                new_tile[(l + 2) * R_ROW + k] = tile[k * R_COL + l + 2];
                new_tile[(l + 3) * R_ROW + k] = tile[k * R_COL + l + 3];
            }
    }

    const int *tile     = matrix + i * R_COL + j;
    int       *new_tile = new_matrix + j * R_ROW + i;
    for (int k = 0; k < R_ROW - i; ++k)
        for (int l = 0; l < R_COL - j; ++l)
            new_tile[l * R_ROW + k] = tile[k * R_COL + l];

    return new_matrix;
}

ATTR void optimized_matmul_impl(const int *const restrict L, const int *const restrict R, int *const restrict result)
{
    int *R_T = tiled_transpose(R);
    for (int i = 0; i < L_ROW; ++i)
        for (int j = 0; j < R_COL; ++j)
            result[i * R_COL + j] = vec_dot(L + i * L_COL, R_T + j * R_ROW, L_COL);
    free(R_T);
}