#include "rocc.h"
#include <stdbool.h>
#include <stdint.h>

#define ROCC_OPCODE 0

#define OP_LOAD    0
#define OP_STORE   1
#define OP_INFO    2
#define OP_COMPUTE 3

void load_data(uint32_t base_address_DRAM, uint32_t base_address_chip, uint32_t len)
{
    uint64_t rs1 = 0;
    rs1          = len;
    rs1          = (rs1 << 32) + base_address_chip;

    uint32_t rs2 = 0;
    rs2          = base_address_DRAM;

    ROCC_INSTRUCTION_SS(ROCC_OPCODE, rs1, rs2, OP_LOAD);
}

void store_data(uint32_t base_address_DRAM, uint32_t base_address_chip, uint32_t len)
{
    uint64_t rs1 = 0;
    rs1          = len;
    rs1          = (rs1 << 32) + base_address_chip;

    uint32_t rs2 = 0;
    rs2          = base_address_DRAM;

    ROCC_INSTRUCTION_SS(ROCC_OPCODE, rs1, rs2, OP_STORE);
    __asm__ volatile("fence.i");
}

void mat_info(uint32_t N, uint32_t K, uint32_t M)
{
    uint64_t rs1 = 0;
    rs1          = N;
    rs1          = (rs1 << 32) + K;

    uint32_t rs2 = 0;
    rs2          = M;

    ROCC_INSTRUCTION_SS(ROCC_OPCODE, rs1, rs2, OP_INFO);
}

void compute() { ROCC_INSTRUCTION(ROCC_OPCODE, OP_COMPUTE); }