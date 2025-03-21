#include <chrono>
#include <cstdlib>
#include <fstream>
#include <immintrin.h>
#include <iostream>

#define VEC_SIZE   ((1 << 25) + 29)
#define TEST_TIMES 10

int naive_vec_dot(int *vec_a, int *vec_b, int size)
{
    int result = 0;
    for (int i = 0; i < size; ++i)
        result += vec_a[i] * vec_b[i];
    return result;
}

int __attribute__((optimize("O3"))) opt_vec_dot(int *vec_a, int *vec_b, int size)
{
    int result = 0;
    for (int i = 0; i < size; ++i)
        result += vec_a[i] * vec_b[i];
    return result;
}

int simd_vec_dot(int *vec_a, int *vec_b, int size)
{
    __m256i sum            = _mm256_setzero_si256();
    int     remaining_size = size;

    while (remaining_size >= 8)
    {
        __m256i va      = _mm256_loadu_si256((__m256i *)vec_a);
        __m256i vb      = _mm256_loadu_si256((__m256i *)vec_b);
        sum             = _mm256_add_epi32(sum, _mm256_mullo_epi32(va, vb));
        vec_a          += 8;
        vec_b          += 8;
        remaining_size -= 8;
    }

    int result = 0;
    for (int i = 0; i < 8; ++i)
        result += ((int *)&sum)[i];

    for (int i = 0; i < remaining_size; ++i)
        result += vec_a[i] * vec_b[i];

    return result;
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

int my_vec_dot_asm(int *vec_a, int *vec_b, int size)
{
    return 0; // TODO
}

int main()
{
    int *vec_a = new int[VEC_SIZE];
    int *vec_b = new int[VEC_SIZE];

    std::fstream input("input.dat", std::ios::in | std::ios::binary);
    input.read((char *)vec_a, sizeof(int) * VEC_SIZE);
    input.read((char *)vec_b, sizeof(int) * VEC_SIZE);

    std::chrono::time_point<std::chrono::system_clock> start, end;
    int                                                naive_result, opt_result, simd_result, my_result;
    std::chrono::duration<double>                      naive_time, opt_time, simd_time, my_time;

    start = std::chrono::system_clock::now();
    for (int i = 0; i < TEST_TIMES; ++i)
        naive_result = naive_vec_dot(vec_a, vec_b, VEC_SIZE);
    end        = std::chrono::system_clock::now();
    naive_time = end - start;

    start = std::chrono::system_clock::now();
    for (int i = 0; i < TEST_TIMES; ++i)
        opt_result = opt_vec_dot(vec_a, vec_b, VEC_SIZE);
    end      = std::chrono::system_clock::now();
    opt_time = end - start;

    start = std::chrono::system_clock::now();
    for (int i = 0; i < TEST_TIMES; ++i)
        simd_result = simd_vec_dot(vec_a, vec_b, VEC_SIZE);
    end       = std::chrono::system_clock::now();
    simd_time = end - start;

    start = std::chrono::system_clock::now();
    for (int i = 0; i < TEST_TIMES; ++i)
        my_result = my_vec_dot(vec_a, vec_b, VEC_SIZE);
    end     = std::chrono::system_clock::now();
    my_time = end - start;

    std::cout << "Naive result: " << naive_result << std::endl;
    std::cout << "Opt result:   " << opt_result << std::endl;
    std::cout << "Simd result:  " << simd_result << std::endl;
    std::cout << "My result:    " << my_result << std::endl;

    std::cout << "Naive time: " << naive_time.count() * 1000.0 << "ms" << std::endl;
    std::cout << "Opt time:   " << opt_time.count() * 1000.0 << "ms" << std::endl;
    std::cout << "Simd time:  " << simd_time.count() * 1000.0 << "ms" << std::endl;
    std::cout << "My time:    " << my_time.count() * 1000.0 << "ms" << std::endl;
}