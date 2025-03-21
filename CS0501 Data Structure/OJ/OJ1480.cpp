#include <iostream>

const int maxN = 1e6, Log2_maxN = 20;
// 1-base
int       sum[maxN + 1];       // prefix sum
int       var[maxN + 1];       // i - 2 * sum[i]
int       ST[maxN][Log2_maxN]; // RMQ ST for var[]
int       Log2[maxN];

void _Log2(int n)
{
    Log2[0] = Log2[1] = 0;
    for (int i = 2; i <= n; ++i)
        Log2[i] = Log2[i >> 1] + 1;
}

void RMQ_construct(int n)
{
    for (int i = 1; i <= n; ++i)
        ST[i][0] = var[i];
    for (int j = 1; j <= Log2[n]; ++j)
        for (int i = 0; i + (1 << (j - 1)) - 1 <= n; ++i)
            ST[i][j] = std::max(ST[i][j - 1], ST[i + (1 << (j - 1))][j - 1]);
}

int RMQ_query(int l, int r)
{
    // i != l
    int x = Log2[r - l + 1];
    return std::max(ST[l][x], ST[r + 1 - (1 << x)][x]);
}

void input(int n)
{
    _Log2(n);
    sum[0] = 0, var[0] = 0;
    int tmp;
    for (int i = 1; i <= n; ++i)
    {
        scanf("%d", &tmp);
        sum[i] = sum[i - 1] + tmp;
        var[i] = i - 2 * sum[i];
    }
    RMQ_construct(n);
}

void query(int m)
{
    int type, l, r;
    while (m--)
    {
        scanf("%d %d %d", &type, &l, &r);
        if (type == 1)
            printf("%d\n", std::max(RMQ_query(l, r) - l + 1 + sum[r] + sum[l - 1], sum[r] - sum[l - 1]));
        else
        {
            int numOf1 = sum[r] - sum[l - 1], numOf0 = r - l + 1 - numOf1;
            if (sum[r] != sum[r - numOf0])
                printf("%d\n", 2);
            else
                printf("%d\n", 1);
        }
    }
}

int main()
{
    int n, m;
    scanf("%d %d", &n, &m);
    input(n);
    query(m);
    return 0;
}