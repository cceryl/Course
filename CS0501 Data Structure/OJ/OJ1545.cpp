#include <iostream>

constexpr int maxN = 3005;
int           n, m;
int           tree[2 * maxN][2 * maxN];
// x' = x - y + n;
// y' = m + n + 1 - x - y;

inline int read()
{
    char ch = getchar();
    if (ch == EOF)
        return -1;
    int x = 0, f = 1;
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

void add(int x, int y, int value)
{
    for (int i = x; i <= m + n - 1; i += i & -i)
        for (int j = y; j <= m + n - 1; j += j & -j)
            tree[i][j] += value;
}

int query(int x, int y)
{
    int value = 0;
    for (int i = x; i > 0; i -= i & -i)
        for (int j = y; j > 0; j -= j & -j)
            value += tree[i][j];
    return value;
}

void input()
{
    m = read(), n = read();
    for (int j = n; j >= 1; --j)
        for (int i = 1; i <= m; ++i)
            add(i - j + n, m + n + 1 - i - j, read());
}

void solve()
{
    int op, x, y, midx, midy, value;
    while ((op = read()) != -1)
    {
        x = read(), y = read(), value = read();
        if (op == 1)
        {
            y = n + 1 - y;
            add(x - y + n, m + n + 1 - x - y, value);
        }
        else
        {
            y    = n + 1 - y;
            midx = x - y + n, midy = m + n + 1 - x - y;
            printf("%d\n", query(midx + value, midy + value) - query(midx + value, midy - value - 1) - query(midx - value - 1, midy + value)
                               + query(midx - value - 1, midy - value - 1));
        }
    }
}

int main()
{
    input();
    solve();
    return 0;
}