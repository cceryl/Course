#include "mat_mult_acc.h"

#include "ISA.h"

#define INPUT_WIDTH  8
#define RESULT_WIDTH 32

#define MAX_MATA_ROW 1
#define MAX_MATA_COL 64
#define MAX_MATB_ROW 64
#define MAX_MATB_COL 10

#define CORESIZE 4

uint32_t pad(uint32_t size, uint32_t core_size) { return (size + core_size - 1) & ~(core_size - 1); }

void mat_mult_acc(const int8_t *mat_l, const int8_t *mat_r, int *result, const unsigned int N, const unsigned int K, const unsigned int M)
{
    long l_buffer[N * K];
    long r_buffer[K * M];
    long res_buffer[N * M];

    uint32_t onChipLAddr   = 0;
    uint32_t onChipRAddr   = pad(MAX_MATA_ROW, CORESIZE) * pad(MAX_MATA_COL, CORESIZE);
    uint32_t onChipResAddr = onChipRAddr + pad(MAX_MATB_ROW, CORESIZE) * pad(MAX_MATB_COL, CORESIZE);

    uint32_t readAddr;
    uint32_t writeAddr;

    for (int i = 0; i < N * K; ++i)
        l_buffer[i] = mat_l[i];
    for (int i = 0; i < K * M; ++i)
        r_buffer[i] = mat_r[i];

    mat_info(N, K, M);

    readAddr  = (unsigned long)l_buffer;
    writeAddr = onChipLAddr;
    load_data(readAddr, writeAddr, N * K);

    readAddr  = (unsigned long)r_buffer;
    writeAddr = onChipRAddr;
    load_data(readAddr, writeAddr, K * M);

    compute();

    readAddr  = onChipResAddr;
    writeAddr = (unsigned long)res_buffer;
    store_data(writeAddr, readAddr, N * M);

    for (int i = 0; i < N * M; i++)
        result[i] = res_buffer[i];
}