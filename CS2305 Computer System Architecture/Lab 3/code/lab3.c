#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "impl.h"

void optimized_matmul(int *L, int *R, int *result) { optimized_matmul_impl(L, R, result); }

void load_matrix(const char *filename, int *matrix, int rows, int cols)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fread(matrix, sizeof(int), rows * cols, file);
    fclose(file);
}

void matmul(int *L, int *R, int *result)
{
    for (int i = 0; i < L_ROW; i++)
        for (int j = 0; j < R_COL; j++)
        {
            result[i * R_COL + j] = 0;
            for (int k = 0; k < L_COL; k++)
                result[i * R_COL + j] += L[i * L_COL + k] * R[k * R_COL + j];
        }
}

int main()
{
    int *L          = malloc(L_ROW * L_COL * sizeof(int));
    int *R          = malloc(R_ROW * R_COL * sizeof(int));
    int *result     = malloc(L_ROW * R_COL * sizeof(int));
    int *true_value = malloc(L_ROW * R_COL * sizeof(int));

    printf("Loading L matrix.....");
    load_matrix("L.dat", L, L_ROW, L_COL);
    printf("Done\n");

    printf("Loading R matrix.....");
    load_matrix("R.dat", R, R_ROW, R_COL);

    printf("Done\n");

    for (int i = 0; i < L_ROW; i++)
        for (int j = 0; j < R_COL; j++)
            result[i * R_COL + j] = 0;

    printf("Matmul begin....");
    clock_t start = clock();
    matmul(L, R, result);
    clock_t end = clock();
    printf("Done in %.3f s\n", (long long)(end - start) * 1.0 / CLOCKS_PER_SEC);

    for (int i = 0; i < L_ROW; i++)
        for (int j = 0; j < R_COL; j++)
            result[i * R_COL + j] = 0;

    printf("Optimized matmul begin....");
    start = clock();
    optimized_matmul(L, R, result);
    end = clock();
    printf("Done in %.3f s\n", (long long)(end - start) * 1.0 / CLOCKS_PER_SEC);

    // Validate the result
    printf("Loading true value matrix.....");
    load_matrix("true_value.dat", true_value, L_ROW, R_COL);
    printf("Done\n");
    int mismatches = 0;
    for (int i = 0; i < L_ROW; i++)
    {
        if (mismatches >= 64)
            break;
        for (int j = 0; j < R_COL; j++)
        {
            if (result[i * R_COL + j] != true_value[i * R_COL + j])
            {
                printf("Mismatch at (%d, %d): calc=%d, expected=%d\n", i, j, result[i * R_COL + j], true_value[i * R_COL + j]);
                if (++mismatches >= 64)
                {
                    printf("More than 64 mismatches, stopping validation.\n");
                    break;
                }
            }
        }
    }

    if (mismatches == 0)
        printf("Validation passed.\n");
    else
        printf("Validation failed with %d mismatches.\n", mismatches);

    free(L);
    free(R);
    free(result);
    free(true_value);

    return 0;
}