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
    for (i = 2; i <= n; i += 2)
    {
        first  = (first + second) % mod;
        second = (first + second) % mod;
    }
    result = (n & 1) ? second : first;
    end    = clock();
    printf("result = %lld\n", result);
    printf("time ticks are %ld\n", end - start);
    return 0;
}