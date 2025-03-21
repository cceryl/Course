#include <stdio.h>

constexpr int maxN = 3e5, log2maxN = 20;

struct side
{
    int from, to;
    int next;
    side() : from(0), to(0), next(-1) {}
};

side sides[maxN];
int  ST[maxN][log2maxN + 1];
int  firstSide[maxN];
int  deltaMilkTea[maxN];
int  milkTea[maxN];
int  depth[maxN];
int  n, k;

void input()
{
    scanf("%d%d", &n, &k);
    int from, to, placeholder;
    int sideCnt = 1;
    for (int i = 0; i < n - 1; ++i)
    {
        scanf("%d%d%d", &from, &to, &placeholder);
        sides[sideCnt].next = firstSide[from], firstSide[from] = sideCnt;
        sides[sideCnt].from = from, sides[sideCnt].to = to;
        ++sideCnt;
        sides[sideCnt].next = firstSide[to], firstSide[to] = sideCnt;
        sides[sideCnt].from = to, sides[sideCnt].to = from;
        ++sideCnt;
    }
}

void tree(int point = 1, int father = 0)
{
    ST[point][0] = father;
    depth[point] = depth[father] + 1;
    int s        = firstSide[point];
    while (s != 0)
    {
        if (sides[s].to != father)
            tree(sides[s].to, point);
        s = sides[s].next;
    }
}

void LCA()
{
    for (int j = 1; j <= log2maxN; ++j)
        for (int i = 1; i <= n; ++i)
            ST[i][j] = ST[ST[i][j - 1]][j - 1];
}

int query(int x, int y)
{
    if (x == y)
        return x;
    else
    {
        if (depth[y] > depth[x])
            x ^= y, y ^= x, x ^= y;
        for (int i = log2maxN; i >= 0; --i)
            if (depth[x] - (1 << i) >= depth[y])
                x = ST[x][i];
        if (x != y)
            for (int i = log2maxN; i >= 0; --i)
                if (ST[x][i] != ST[y][i])
                    x = ST[x][i], y = ST[y][i];
        if (x != y)
            x = ST[x][0], y = ST[y][0];
        return x;
    }
}

int max = 0;

void maxMilkTea(int root = 1, int father = 0)
{
    int s = firstSide[root];
    while (s != 0)
    {
        if (sides[s].to != father)
        {
            maxMilkTea(sides[s].to, root);
            milkTea[root] += milkTea[sides[s].to];
        }
        s = sides[s].next;
    }
    milkTea[root] += deltaMilkTea[root];
    if (milkTea[root] > max)
        max = milkTea[root];
}

void solve()
{
    int x, y, lca;
    while (k--)
    {
        scanf("%d%d", &x, &y);
        lca = query(x, y);
        ++deltaMilkTea[x];
        ++deltaMilkTea[y];
        --deltaMilkTea[lca];
        --deltaMilkTea[ST[lca][0]];
    }
    maxMilkTea();
    printf("%d", max);
}

int main()
{
    input();
    tree();
    LCA();
    solve();
    return 0;
}