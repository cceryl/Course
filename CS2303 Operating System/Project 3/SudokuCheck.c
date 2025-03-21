// SudokuCheck.c
// Check sudoku using pthreads

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct CheckSubgridArgs
{
    int (*sudoku)[9];
    int subgrid_row;
    int subgrid_column;
};

void *checkRows(void *args);
void *checkColumns(void *args);
void *checkSubgrids(void *args);

int rowResult;
int columnResult;
int subgridResult[9];

int main()
{
    int sudoku[9][9];

    printf("Enter the sudoku:\n");
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            scanf("%d", &sudoku[i][j]);

    pthread_t tid_row, tid_column, tid_subgrid[9];

    pthread_create(&tid_row, NULL, checkRows, sudoku);
    pthread_create(&tid_column, NULL, checkColumns, sudoku);
    struct CheckSubgridArgs csas[9];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            csas[i * 3 + j].sudoku         = sudoku;
            csas[i * 3 + j].subgrid_row    = i;
            csas[i * 3 + j].subgrid_column = j;
            pthread_create(&tid_subgrid[i * 3 + j], NULL, checkSubgrids, &csas[i * 3 + j]);
        }

    pthread_join(tid_row, NULL);
    pthread_join(tid_column, NULL);
    for (int i = 0; i < 9; ++i)
        pthread_join(tid_subgrid[i], NULL);

    if (!rowResult || !columnResult)
    {
        printf("The sudoku is invalid.\n");
        return 0;
    }
    for (int i = 0; i < 9; ++i)
        if (!subgridResult[i])
        {
            printf("The sudoku is invalid.\n");
            return 0;
        }
    printf("The sudoku is valid.\n");
    return 0;
}

void *checkRows(void *args)
{
    int(*sudoku)[9] = args;

    for (int i = 0; i < 9; ++i)
    {
        int check[9] = {0};
        for (int j = 0; j < 9; ++j)
        {
            if (sudoku[i][j] < 1 || sudoku[i][j] > 9 || check[sudoku[i][j] - 1] == 1)
            {
                rowResult = 0;
                pthread_exit(NULL);
            }
            check[sudoku[i][j] - 1] = 1;
        }
    }

    rowResult = 1;
    pthread_exit(NULL);
}

void *checkColumns(void *args)
{
    int(*sudoku)[9] = args;

    for (int i = 0; i < 9; ++i)
    {
        int check[9] = {0};
        for (int j = 0; j < 9; ++j)
        {
            if (sudoku[j][i] < 1 || sudoku[j][i] > 9 || check[sudoku[j][i] - 1] == 1)
            {
                columnResult = 0;
                pthread_exit(NULL);
            }
            check[sudoku[j][i] - 1] = 1;
        }
    }

    columnResult = 1;
    pthread_exit(NULL);
}

void *checkSubgrids(void *args)
{
    struct CheckSubgridArgs *csa = (struct CheckSubgridArgs *)args;

    int(*sudoku)[9] = csa->sudoku;
    int row         = csa->subgrid_row * 3;
    int column      = csa->subgrid_column * 3;

    int check[9] = {0};
    for (int i = row; i < row + 3; ++i)
        for (int j = column; j < column + 3; ++j)
        {
            if (sudoku[i][j] < 1 || sudoku[i][j] > 9 || check[sudoku[i][j] - 1] == 1)
            {
                subgridResult[row + column / 3] = 0;
                pthread_exit(NULL);
            }
            check[sudoku[i][j] - 1] = 1;
        }

    subgridResult[row + column / 3] = 1;
    pthread_exit(NULL);
}

/* Valid Sudoku */
// 5 3 4 6 7 8 9 1 2
// 6 7 2 1 9 5 3 4 8
// 1 9 8 3 4 2 5 6 7
// 8 5 9 7 6 1 4 2 3
// 4 2 6 8 5 3 7 9 1
// 7 1 3 9 2 4 8 5 6
// 9 6 1 5 3 7 2 8 4
// 2 8 7 4 1 9 6 3 5
// 3 4 5 2 8 6 1 7 9