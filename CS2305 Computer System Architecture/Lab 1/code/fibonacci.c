#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(int argc, char *argv[])
{
    clock_t start, end;
    start         = clock();
    int       mod = 10007;
    long long n   = 800000001;
    long long first, second, result;
    first  = 0;
    second = 1;
    result = 0;
    int i;
    for (i = 2; i <= n; ++i)
    {
        result = (first + second) % mod;
        first  = second;
        second = result;
    }
    end = clock();
    printf("result = %d\n", (int)result);
    printf("time ticks are %d\n", (int)((end - start)));
    return 0;
}