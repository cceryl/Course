#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TILE_SIZE  128
#define TEST_TIMES 50

void naive_mat_mul(int *mat_a, int *mat_b, int *mat_c, int size)
{
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            for (int k = 0; k < size; ++k)
                mat_c[i * size + j] += mat_a[i * size + k] * mat_b[k * size + j];
}

void __attribute((optimize("O3"))) opt_mat_mul(int *mat_a, int *mat_b, int *mat_c, int size)
{
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            for (int k = 0; k < size; ++k)
                mat_c[i * size + j] += mat_a[i * size + k] * mat_b[k * size + j];
}

int my_vec_dot(int *vec_a, int *vec_b, int size)
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
        va_0 = _mm256_loadu_si256((__m256i *)vec_a);
        va_1 = _mm256_loadu_si256((__m256i *)(vec_a + 8));
        va_2 = _mm256_loadu_si256((__m256i *)(vec_a + 16));
        va_3 = _mm256_loadu_si256((__m256i *)(vec_a + 24));
        va_4 = _mm256_loadu_si256((__m256i *)(vec_a + 32));
        va_5 = _mm256_loadu_si256((__m256i *)(vec_a + 40));
        va_6 = _mm256_loadu_si256((__m256i *)(vec_a + 48));
        va_7 = _mm256_loadu_si256((__m256i *)(vec_a + 56));
        vb_0 = _mm256_loadu_si256((__m256i *)vec_b);
        vb_1 = _mm256_loadu_si256((__m256i *)(vec_b + 8));
        vb_2 = _mm256_loadu_si256((__m256i *)(vec_b + 16));
        vb_3 = _mm256_loadu_si256((__m256i *)(vec_b + 24));
        vb_4 = _mm256_loadu_si256((__m256i *)(vec_b + 32));
        vb_5 = _mm256_loadu_si256((__m256i *)(vec_b + 40));
        vb_6 = _mm256_loadu_si256((__m256i *)(vec_b + 48));
        vb_7 = _mm256_loadu_si256((__m256i *)(vec_b + 56));

        sum_0 = _mm256_add_epi32(sum_0, _mm256_mullo_epi32(va_0, vb_0));
        sum_1 = _mm256_add_epi32(sum_1, _mm256_mullo_epi32(va_1, vb_1));
        sum_2 = _mm256_add_epi32(sum_2, _mm256_mullo_epi32(va_2, vb_2));
        sum_3 = _mm256_add_epi32(sum_3, _mm256_mullo_epi32(va_3, vb_3));
        sum_4 = _mm256_add_epi32(sum_4, _mm256_mullo_epi32(va_4, vb_4));
        sum_5 = _mm256_add_epi32(sum_5, _mm256_mullo_epi32(va_5, vb_5));
        sum_6 = _mm256_add_epi32(sum_6, _mm256_mullo_epi32(va_6, vb_6));
        sum_7 = _mm256_add_epi32(sum_7, _mm256_mullo_epi32(va_7, vb_7));

        vec_a          += 64;
        vec_b          += 64;
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
        result += vec_a[i] * vec_b[i];

    return result;
}

int *my_transpose(int *mat, int size)
{
    int *transposed = (int *)malloc(sizeof(int) * size * size);
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            transposed[i * size + j] = mat[j * size + i];
    return transposed;
}

void my_mat_mul(int *mat_a, int *mat_b, int *mat_c, int size)
{
    int *transposed_mat_b = my_transpose(mat_b, size); // this should be done by caller

    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            mat_c[i * size + j] = my_vec_dot(mat_a + i * size, transposed_mat_b + j * size, size);

    free(transposed_mat_b);

    // int i, j, k;
    //
    // for (i = 0; i < size; i++)
    //{
    //    for (j = 0; j < size; j++)
    //    {
    //        int        sum  = 0;
    //        const int *ptr1 = mat_a + i * size;
    //        const int *ptr2 = mat_b + j;
    //
    //        for (k = 0; k < size; k++)
    //        {
    //            sum += *ptr1 * *ptr2;
    //            ptr1++;
    //            ptr2 += size;
    //        }
    //
    //        mat_c[i * size + j] = sum;
    //    }
    //}
}

int main()
{
    int *mat_a, *mat_b, *mat_c;
    mat_a = (int *)malloc(sizeof(int) * TILE_SIZE * TILE_SIZE);
    mat_b = (int *)malloc(sizeof(int) * TILE_SIZE * TILE_SIZE);
    mat_c = (int *)malloc(sizeof(int) * TILE_SIZE * TILE_SIZE);

    FILE *input = fopen("input.dat", "rb");
    fread(mat_a, sizeof(int), TILE_SIZE * TILE_SIZE, input);
    fread(mat_b, sizeof(int), TILE_SIZE * TILE_SIZE, input);

    clock_t start, end;
    int     naive_time, opt_time, my_time;
    int     naive_result, opt_result, my_result;

    start = clock();
    for (int i = 0; i < TEST_TIMES; ++i)
        naive_mat_mul(mat_a, mat_b, mat_c, TILE_SIZE);
    end        = clock();
    naive_time = end - start;

    start = clock();
    for (int i = 0; i < TEST_TIMES; ++i)
        opt_mat_mul(mat_a, mat_b, mat_c, TILE_SIZE);
    end      = clock();
    opt_time = end - start;

    start = clock();
    for (int i = 0; i < TEST_TIMES; ++i)
        my_mat_mul(mat_a, mat_b, mat_c, TILE_SIZE);
    end     = clock();
    my_time = end - start;

    int  error_flag = 0;
    int *correct    = (int *)malloc(sizeof(int) * TILE_SIZE * TILE_SIZE);
    int *answer     = (int *)malloc(sizeof(int) * TILE_SIZE * TILE_SIZE);
    memset(correct, 0, sizeof(int) * TILE_SIZE * TILE_SIZE);
    memset(answer, 0, sizeof(int) * TILE_SIZE * TILE_SIZE);
    opt_mat_mul(mat_a, mat_b, correct, TILE_SIZE);
    my_mat_mul(mat_a, mat_b, answer, TILE_SIZE);
    for (int i = 0; i < TILE_SIZE * TILE_SIZE; ++i)
        if (correct[i] != answer[i])
        {
            error_flag = 1;
            printf("Wrong answer\n");
            break;
        }

    printf("Naive time:    %dms\n", error_flag ? -1 : naive_time);
    printf("Opt time:      %dms\n", error_flag ? -1 : opt_time);
    printf("My time:       %dms\n", error_flag ? -1 : my_time);
}