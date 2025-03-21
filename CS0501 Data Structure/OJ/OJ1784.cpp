#include <iostream>

using std::cin, std::cout, std::endl;

bool Solve(int **, int, int);

int main()
{
    int **mat;
    mat = new int *[9];
    for (int i = 0; i < 9; i++)
        mat[i] = new int[9];
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            cin >> mat[i][j];
    if (!Solve(mat, 0, 0))
    {
        cout << endl << "Wrong input.";
        return 0;
    }
    cout << endl;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
            cout << mat[i][j] << ' ';
        cout << endl;
    }
    for (int i = 0; i < 9; i++)
        delete[] mat[i];
    delete[] mat;
    return 0;
}

bool Solve(int **mat, int row = 0, int column = 0)
{
    if (row == 9)
        return 1;
    int nextcolumn = column + 1, nextrow = row;
    if (nextcolumn == 9)
    {
        nextcolumn = 0;
        nextrow++;
    }
    if (mat[row][column] != 0)
        if (Solve(mat, nextrow, nextcolumn))
            return 1;
        else
            return 0;
    else
    {
        for (int i = 1; i <= 9; i++)
        {
            bool isvalid = 1;
            for (int j = 0; j < 9; j++)
            {
                if (mat[row][j] == i)
                {
                    isvalid = 0;
                    break;
                }
                if (mat[j][column] == i)
                {
                    isvalid = 0;
                    break;
                }
            }
            if (isvalid == 0)
                continue;
            for (int j = 3 * (row / 3); j < 3 * (row / 3) + 3; j++)
            {
                for (int k = 3 * (column / 3); k < 3 * (column / 3) + 3; k++)
                    if (mat[j][k] == i)
                    {
                        isvalid = 0;
                        break;
                    }
                if (isvalid == 0)
                    break;
            }
            if (isvalid == 0)
                continue;
            mat[row][column] = i;
            if (Solve(mat, nextrow, nextcolumn))
                return 1;
            else
                mat[row][column] = 0;
        }
        return 0;
    }
}