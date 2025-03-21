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
    long long sum = 0;

#pragma omp parallel num_threads(4) shared(sum)
    {
        int       thread_id = omp_get_thread_num();
        long long start     = (n / 4) * thread_id + 1;
        long long end       = (n / 4) * (thread_id + 1);
        if (thread_id == 3)
            end = n;

        long long local_sum = 0;
        long long square, cube;

        for (int i = start; i <= end; ++i)
        {
            square    = ((i % mod) * (i % mod)) % mod;
            cube      = ((square % mod) * (i % mod)) % mod;
            local_sum = (local_sum + square + cube) % mod;
        }

#pragma omp atomic
        sum += local_sum;
    }
    sum %= mod;

    end = clock();
    printf("sum = %lld\n", sum);
    printf("time ticks are %ld\n", end - start);
    return 0;
}