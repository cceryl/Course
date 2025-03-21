#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    clock_t start, end;
    start         = clock();
    int       mod = 10007;
    long long n   = 800000000;
    long long square, cube, sum;
    square = 0;
    cube   = 0;
    sum    = 0;
    int i;
    for (i = 1; i <= n; ++i)
    {
        square = ((i % mod) * (i % mod)) % mod;
        cube   = ((square % mod) * (i % mod)) % mod;
        sum    = (sum + square + cube) % mod;
    }
    end = clock();
    printf("sum = %d\n", (int)sum);
    printf("time ticks are %d\n", (int)((end - start)));
    return 0;
}