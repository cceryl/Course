#include "ISA.h"

#include <stdio.h>

#define INPUT_WIDTH  8
#define RESULT_WIDTH 32

#define TEST_N 10
#define TEST_K 11
#define TEST_M 12

#define MAX_MATA_ROW 1
#define MAX_MATA_COL 784
#define MAX_MATB_ROW 784
#define MAX_MATB_COL 128

#define CORESIZE 8

#define SIGNED_L 1
#define SIGNED_R 1

long mat_l_buf[TEST_N * TEST_K];
long mat_r_buf[TEST_K * TEST_M];
long result_buf[TEST_N * TEST_M];
long result_true[TEST_N * TEST_M];

uint32_t pad(uint32_t size, uint32_t core_size) { return (size + core_size - 1) & ~(core_size - 1); }

void print_mat(const long *mat, const unsigned int M, const unsigned int N)
{
    for (int i = 0; i < M; ++i)
    {
        for (int j = 0; j < N; ++j)
            printf("%ld ", mat[i * N + j]);
        printf("\n");
    }
}

void mat_mult_acc(const long *mat_l, const long *mat_r, long *result, const unsigned int N, const unsigned int K, const unsigned int M)
{
    uint32_t onChipLAddr   = 0;
    uint32_t onChipRAddr   = pad(MAX_MATA_ROW, CORESIZE) * pad(MAX_MATA_COL, CORESIZE);
    uint32_t onChipResAddr = onChipRAddr + pad(MAX_MATB_ROW, CORESIZE) * pad(MAX_MATB_COL, CORESIZE);

    uint32_t readAddr;
    uint32_t writeAddr;

    mat_info(N, K, M);

    readAddr  = (unsigned long)mat_l;
    writeAddr = onChipLAddr;
    load_data(readAddr, writeAddr, N * K);
    printf("\"mat_l\" loaded\n");

    readAddr  = (unsigned long)mat_r;
    writeAddr = onChipRAddr;
    load_data(readAddr, writeAddr, K * M);
    printf("\"mat_r\" loaded\n");

    compute();
    printf("Compute complete\n");

    readAddr  = onChipResAddr;
    writeAddr = (unsigned long)result;
    store_data(writeAddr, readAddr, N * M);
    printf("\"result\" stored\n");
}

void gen_random_input(long *mat, const unsigned int M, const unsigned int N, int isSigned)
{
    for (int i = 0; i < M * N; ++i)
    {
        long time        = rdcycle();
        int  mask        = (1 << INPUT_WIDTH) - 1;
        int  sign_offset = (1 << (INPUT_WIDTH - 1));
        if (isSigned)
            mat[i] = (time & mask) - sign_offset;
        else
            mat[i] = time & mask;
    }
}

void calc_gemm(const long *mat_l, const long *mat_r, long *result, const unsigned int N, const unsigned int K, const unsigned int M)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
        {
            long sum = 0;
            for (int k = 0; k < K; k++)
                sum += mat_l[i * K + k] * mat_r[k * M + j];
            result[i * M + j] = sum;
        }

    printf("True result calculated\n");
}

void check(const long *check, const long *true_res, int num)
{
    for (int i = 0; i < num; i++)
        if (check[i] != true_res[i])
        {
            printf("Result[%d][%d], expected: %ld, got: %ld\n", i / TEST_M, i % TEST_M, true_res[i], check[i]);
            return;
        }
    printf("All correct!\n");
}

int main()
{
    gen_random_input(mat_l_buf, TEST_N, TEST_K, SIGNED_L);
    gen_random_input(mat_r_buf, TEST_K, TEST_M, SIGNED_R);

    // printf("mat_l:\n");
    // print_mat(mat_l_buf, TEST_N, TEST_K);

    // printf("mat_r:\n");
    // print_mat(mat_r_buf, TEST_K, TEST_M);

    mat_mult_acc(mat_l_buf, mat_r_buf, result_buf, TEST_N, TEST_K, TEST_M);
    calc_gemm(mat_l_buf, mat_r_buf, result_true, TEST_N, TEST_K, TEST_M);

    // printf("result:\n");
    // print_mat(result_buf, TEST_N, TEST_M);

    check(result_buf, result_true, TEST_N * TEST_M);

    return 0;
}