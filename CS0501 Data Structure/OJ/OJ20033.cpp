#include <stdio.h>

constexpr int maxN = 2e5, log2maxN = 18;

struct side
{
    int   from, to;
    int   next;
    short weight;
    side() : from(0), to(0), next(-1), weight(-1) {}
};

side sides[maxN];
int  ST[maxN][log2maxN + 1];
int  firstSide[maxN];
int  sumWeight[maxN];
int  depth[maxN];
int  n, m;

void input()
{
    scanf("%d%d", &n, &m);
    int from, to, weight;
    int sideCnt = 1;
    for (int i = 0; i < m; ++i)
    {
        scanf("%d%d%d", &from, &to, &weight);
        sides[sideCnt].next = firstSide[from], firstSide[from] = sideCnt;
        sides[sideCnt].from = from, sides[sideCnt].to = to, sides[sideCnt].weight = weight;
        ++sideCnt;
        sides[sideCnt].next = firstSide[to], firstSide[to] = sideCnt;
        sides[sideCnt].from = to, sides[sideCnt].to = from, sides[sideCnt].weight = weight;
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
            sumWeight[sides[s].to] = sumWeight[point] + sides[s].weight, tree(sides[s].to, point);
        s = sides[s].next;
    }
}

void LCA()
{
    for (int j = 1; j <= log2maxN; ++j)
        for (int i = 1; i <= n; ++i)
            ST[i][j] = ST[ST[i][j - 1]][j - 1];
}

void solve()
{
    int Q;
    scanf("%d", &Q);
    int x, y, sum;
    while (Q--)
    {
        scanf("%d%d", &x, &y);
        sum = sumWeight[x] + sumWeight[y];
        if (x == y)
            printf("0\n");
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
            printf("%d\n", sum - 2 * sumWeight[x]);
        }
    }
}

int main()
{
    input();
    tree();
    LCA();
    solve();
    return 0;
}