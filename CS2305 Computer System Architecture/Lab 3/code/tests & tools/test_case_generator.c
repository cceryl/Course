#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DATA_SIZE (1 << 26) // 4-byte integer * 2^26 = 256MB

int main()
{
    FILE *fp = fopen("input.dat", "wb");
    srand(time(NULL));
    for (int i = 0; i < DATA_SIZE; ++i)
    {
        int data = rand();
        fwrite(&data, sizeof(int), 1, fp);
    }
}