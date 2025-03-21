#include <algorithm>
#include <stdio.h>
#include <vector>

const int maxN = 3e5 + 5;
struct mole
{
    int       x;
    int       t;
    int       w;
    long long nx;
    long long ny;
    int       disny;
};
std::vector<mole>      moles;
std::vector<long long> res;
int                    n, v;

inline long long lowbit(long long x) { return x & -x; }

void input()
{
    moles.reserve(maxN);
    res.reserve(maxN);
    scanf("%d%d", &n, &v);
    for (int i = 1; i <= n; ++i)
        scanf("%d%d%d", &moles[i].x, &moles[i].t, &moles[i].w), moles[i].nx = v * moles[i].t + moles[i].x,
                                                                moles[i].ny = v * moles[i].t - moles[i].x;
}

void discretize()
{
    std::sort(&moles[1], &moles[n + 1], [](const mole &a, const mole &b) { return a.ny < b.ny; });
    moles[1].disny = 1;
    for (int i = 2; i <= n; ++i)
        if (moles[i].ny == moles[i - 1].ny)
            moles[i].disny = moles[i - 1].disny;
        else
            moles[i].disny = moles[i - 1].disny + 1;
}

void insert(int x, long long value)
{
    while (x <= n)
    {
        if (value > res[x])
            res[x] = value;
        x += lowbit(x);
    }
}

long long query(int x)
{
    long long ans = 0;
    while (x > 0)
    {
        if (res[x] > ans)
            ans = res[x];
        x -= lowbit(x);
    }
    return ans;
}

void solve()
{
    std::sort(&moles[1], &moles[n + 1], [](const mole &a, const mole &b) { return a.nx < b.nx || (a.nx == b.nx && a.ny < b.ny); });
    for (int i = 1; i <= n; ++i)
        insert(moles[i].disny, query(moles[i].disny) + moles[i].w);
    printf("%lld", query(n));
}

int main()
{
    input();
    discretize();
    solve();
    return 0;
}