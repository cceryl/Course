#include <stdio.h>
#include <stdlib.h>
#define ATTR //__attribute__((optimize("O3")))

ATTR static void naive_matmul(int size, int x_row_offset, const int *L, int y_row_offset, const int *R, int result_row_offset, int *result)
{
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
        {
            int sum = 0;
            for (int k = 0; k < size; ++k)
                sum += L[i * x_row_offset + k] * R[k * y_row_offset + j];
            result[i * result_row_offset + j] = sum;
        }
}

ATTR static void madd(int size, int x_row_offset, const int *L, int y_row_offset, const int *R, int result_row_offset, int *result)
{
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            result[i * result_row_offset + j] = L[i * x_row_offset + j] + R[i * y_row_offset + j];
}

ATTR static void msub(int size, int x_row_offset, const int *L, int y_row_offset, const int *R, int result_row_offset, int *result)
{
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            result[i * result_row_offset + j] = L[i * x_row_offset + j] - R[i * y_row_offset + j];
}

ATTR void strassen_matmul(int size, int x_row_offset, const int *L, int y_row_offset, const int *R, int result_row_offset, int *result)
{
    if (size <= 16)
    {
        naive_matmul(size, x_row_offset, L, y_row_offset, R, result_row_offset, result);
        return;
    }

    const int n = size / 2;

    const int *A = L;
    const int *B = L + n;
    const int *C = L + n * x_row_offset;
    const int *D = C + n;

    const int *E = R;
    const int *F = R + n;
    const int *G = R + n * y_row_offset;
    const int *H = G + n;

    int      *P[7];
    const int elem_size = n * n * sizeof(int);
    for (int i = 0; i < 7; ++i)
        P[i] = (int *)malloc(elem_size);
    int *tmp_0 = (int *)malloc(elem_size);
    int *tmp_1 = (int *)malloc(elem_size);

    // P0 = A * (F - H);
    msub(n, y_row_offset, F, y_row_offset, H, n, tmp_0);
    strassen_matmul(n, x_row_offset, A, n, tmp_0, n, P[0]);

    // P1 = (A + B) * H
    madd(n, x_row_offset, A, x_row_offset, B, n, tmp_0);
    strassen_matmul(n, n, tmp_0, y_row_offset, H, n, P[1]);

    // P2 = (C + D) * E
    madd(n, x_row_offset, C, x_row_offset, D, n, tmp_0);
    strassen_matmul(n, n, tmp_0, y_row_offset, E, n, P[2]);

    // P3 = D * (G - E);
    msub(n, y_row_offset, G, y_row_offset, E, n, tmp_0);
    strassen_matmul(n, x_row_offset, D, n, tmp_0, n, P[3]);

    // P4 = (A + D) * (E + H)
    madd(n, x_row_offset, A, x_row_offset, D, n, tmp_0);
    madd(n, y_row_offset, E, y_row_offset, H, n, tmp_1);
    strassen_matmul(n, n, tmp_0, n, tmp_1, n, P[4]);

    // P5 = (B - D) * (G + H)
    msub(n, x_row_offset, B, x_row_offset, D, n, tmp_0);
    madd(n, y_row_offset, G, y_row_offset, H, n, tmp_1);
    strassen_matmul(n, n, tmp_0, n, tmp_1, n, P[5]);

    // P6 = (A - C) * (E + F)
    msub(n, x_row_offset, A, x_row_offset, C, n, tmp_0);
    madd(n, y_row_offset, E, y_row_offset, F, n, tmp_1);
    strassen_matmul(n, n, tmp_0, n, tmp_1, n, P[6]);

    // result upper left = (P3 + P4) + (P5 - P1)
    madd(n, n, P[4], n, P[3], n, tmp_0);
    msub(n, n, P[5], n, P[1], n, tmp_1);
    madd(n, n, tmp_0, n, tmp_1, result_row_offset, result);

    // result lower left = P2 + P3
    madd(n, n, P[2], n, P[3], result_row_offset, result + n * result_row_offset);

    // result upper right = P0 + P1
    madd(n, n, P[0], n, P[1], result_row_offset, result + n);

    // result lower right = (P0 + P4) - (P2 + P6)
    madd(n, n, P[0], n, P[4], n, tmp_0);
    madd(n, n, P[2], n, P[6], n, tmp_1);
    msub(n, n, tmp_0, n, tmp_1, result_row_offset, result + n * (result_row_offset + 1));

    free(tmp_0);
    free(tmp_1);
    for (int i= 0; i < 7; ++i)
        free(P[i]);
}