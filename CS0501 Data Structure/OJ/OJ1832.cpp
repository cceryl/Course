#include <iostream>
using std::getchar;

struct data
{
    int same;
    int position;
};

int       n, q;
const int maxN = 1e5 + 5, log2maxN = 20;
data      ST[maxN][log2maxN];
int       Log2[maxN];

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

void _Log2()
{
    Log2[0] = Log2[1] = 0;
    for (int i = 2; i < maxN; ++i)
        Log2[i] = Log2[i >> 1] + 1;
}

void input()
{
    n = read(), q = read();
    ST[1][0].same = ST[1][0].position = 1;
    int last                          = read(), current;
    for (int i = 2; i <= n; ++i)
    {
        current = read();
        if (current == last)
            ST[i][0].same = ST[i - 1][0].same + 1;
        else
            ST[i][0].same = 1;
        ST[i][0].position = i;
        last              = current;
    }
}

void ST_constuct()
{
    for (int j = 1; j <= Log2[n]; ++j)
        for (int i = 1; i + (1 << j) - 1 <= n; ++i)
            if (ST[i][j - 1].same > ST[i + (1 << (j - 1))][j - 1].same)
                ST[i][j] = ST[i][j - 1];
            else
                ST[i][j] = ST[i + (1 << (j - 1))][j - 1];
}

data query(int l, int r)
{
    int x = Log2[r - l + 1];
    if (ST[l][x].same > ST[r + 1 - (1 << x)][x].same)
        return ST[l][x];
    else
        return ST[r + 1 - (1 << x)][x];
}

void solve()
{
    int l, r;
    while (q--)
    {
        l = read(), r = read();
        if (l == r)
            printf("%d\n", 1);
        else
        {
            data res = query(l, r);
            if (res.same <= res.position - l + 1)
                printf("%d\n", res.same);
            else if (res.position == r)
                printf("%d\n", res.position - l + 1);
            else
            {
                data _res = query(res.position + 1, r);
                printf("%d\n", std::max(res.position - l + 1, _res.same));
            }
        }
    }
}

int main()
{
    _Log2();
    int T = read();
    while (T--)
    {
        input();
        ST_constuct();
        solve();
    }
    return 0;
}