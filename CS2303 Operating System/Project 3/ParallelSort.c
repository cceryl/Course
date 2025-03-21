// ParallelSort.c
// Parallel merge sort using pthreads

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MergeSortArgs
{
    int *array;
    int  begin;
    int  end;
};

struct MergeArgs
{
    int *array;
    int  begin;
    int  middle;
    int  end;
};

void *merge_sort(void *args);
void *merge(void *args);

int main()
{
    int  n;
    int *array;
    printf("Enter the number of elements: ");
    scanf("%d", &n);
    array = malloc(n * sizeof(int));
    printf("Enter the elements: ");
    for (int i = 0; i < n; i++)
        scanf("%d", &array[i]);

    pthread_t            tid;
    struct MergeSortArgs msa = {.array = array, .begin = 0, .end = n - 1};
    pthread_create(&tid, NULL, merge_sort, &msa);
    pthread_join(tid, NULL);

    printf("Sorted array: ");
    for (int i = 0; i < n; i++)
        printf("%d ", array[i]);
    printf("\n");

    free(array);
    return 0;
}

void *merge_sort(void *args)
{
    struct MergeSortArgs *msa    = (struct MergeSortArgs *)args;
    int                   middle = (msa->begin + msa->end) / 2;

    pthread_t            tid1, tid2;
    struct MergeSortArgs msa1 = {.array = msa->array, .begin = msa->begin, .end = middle},
                         msa2 = {.array = msa->array, .begin = middle + 1, .end = msa->end};

    if (msa->begin < msa->end)
    {
        pthread_create(&tid1, NULL, merge_sort, &msa1);
        pthread_create(&tid2, NULL, merge_sort, &msa2);
        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);

        pthread_t        tid_merge;
        struct MergeArgs ma = {
            .array  = msa->array,
            .begin  = msa->begin,
            .middle = middle,
            .end    = msa->end,
        };
        pthread_create(&tid_merge, NULL, merge, &ma);
        pthread_join(tid_merge, NULL);
    }

    pthread_exit(0);
}

void *merge(void *args)
{
    struct MergeArgs *ma     = (struct MergeArgs *)args;
    int               begin  = ma->begin;
    int               middle = ma->middle;
    int               end    = ma->end;
    int               n1     = middle - begin + 1;
    int               n2     = end - middle;

    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));
    memcpy(L, ma->array + begin, n1 * sizeof(int));
    memcpy(R, ma->array + middle + 1, n2 * sizeof(int));

    int i = 0;
    int j = 0;
    int k = begin;
    while (i < n1 && j < n2)
        ma->array[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    while (i < n1)
        ma->array[k++] = L[i++];
    while (j < n2)
        ma->array[k++] = R[j++];

    free(L);
    free(R);
    pthread_exit(0);
}