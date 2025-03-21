#include <stdio.h>
using namespace std;

const int maxN = 1e5 + 5, log2maxN = 20;
long long ST[maxN][log2maxN];
int       Log2[maxN];
int       n, q;

long long max(long long a, long long b) { return a > b ? a : b; }

void _Log2()
{
    Log2[0] = Log2[1] = 0;
    for (int i = 2; i <= n; ++i)
        Log2[i] = Log2[i >> 1] + 1;
}

inline int read()
{
    char ch = getchar();
    int  x = 0, f = 1;
    while (ch < '0' || ch > '9')
    {
        if (ch == '-')
            f = -1;
        ch = getchar();
    }
    while ('0' <= ch && ch <= '9')
    {
        x  = x * 10 + ch - '0';
        ch = getchar();
    }
    return x * f;
}

void input()
{
    ST[0][0] = 0;
    n = read(), q = read();
    for (int i = 1; i <= n; ++i)
        ST[i][0] = ST[i - 1][0] + read();
}

void solve()
{
    for (int j = 1; j <= Log2[n]; ++j)
        for (int i = 1; i + (1 << (j - 1)) - 1 <= n; ++i)
            ST[i][j] = max(ST[i][j - 1], ST[i + (1 << (j - 1))][j - 1]);
    int p, l, r, x;
    for (int i = 0; i < q; ++i)
    {
        p = read(), l = read(), r = read();
        // p + l - 1 ~ p + r - 1
        x = Log2[r - l + 1];
        printf("%ld\n", max(ST[p + l - 1][x], ST[p + r - (1 << x)][x]) - ST[p - 1][0]);
    }
}

int main()
{
    input();
    _Log2();
    solve();
    return 0;
}