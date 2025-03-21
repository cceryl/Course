#include <stdio.h>

const int maxN = 500005, maxLog2N = 20;
int       n, m;
// 1-base
int       a[maxN];
int       gcd[maxN][maxLog2N];
int       Log2[maxN];

void input()
{
    for (int i = 1; i <= n; ++i)
        scanf("%d", a + i);
}

void _Log2()
{
    Log2[0] = Log2[1] = 0;
    for (int i = 2; i <= n; ++i)
        Log2[i] = Log2[i >> 1] + 1;
}

int GCD(int a, int b)
{
    if (b == 0)
        return a;
    else
        return GCD(b, (a % b));
}

int search(int left, int right)
{
    int x = Log2[right - left + 1];
    return GCD(gcd[left][x], gcd[right + 1 - (1 << x)][x]);
}

int binary_search(int L, int R, int ans)
{
    // r' <= r
    int left = L, right = R;
    while (left < right)
    {
        int mid = (left + right) / 2;
        if (ans == search(L, mid))
            right = mid;
        else
            left = mid + 1;
    }
    int ansLeft = left;
    // r' > r
    left = R, right = n + 1;
    while (left < right)
    {
        int mid = (right + left) / 2;
        if (ans == search(L, mid))
            left = mid + 1;
        else
            right = mid;
    }
    int ansRight = left;
    return (ansRight - ansLeft - 1);
}

void solve()
{
    _Log2();
    for (int i = 1; i <= n; ++i)
        gcd[i][0] = a[i];
    for (int j = 1; j <= Log2[n]; ++j)
        for (int i = 1; i + (1 << j) - 1 <= n; ++i)
            gcd[i][j] = GCD(gcd[i][j - 1], gcd[i + (1 << (j - 1))][j - 1]);
    int L, R, ans;
    for (int i = 0; i < m; ++i)
    {
        scanf("%d%d", &L, &R);
        ans = search(L, R);
        printf("%d %d\n", ans, binary_search(L, R, ans));
    }
}

int main()
{
    scanf("%d%d", &n, &m);
    input();
    solve();
    return 0;
}