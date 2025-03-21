#include <stdio.h>

constexpr int maxN = 5e5, log2maxN = 19;

struct node
{
    int from;
    int to;
    int next;
    node() : next(0) {}
};

node sides[2 * maxN + 10];
int  firstSide[maxN + 5];
int  depth[maxN + 5];
int  subnodes[maxN + 5];
int  ST[maxN + 5][log2maxN + 1];
int  n, q;
int  sidesCnt = 1;

inline int read()
{
    char ch = getchar();
    int  x  = 0;
    while (ch < '0' || ch > '9')
        ch = getchar();
    while ('0' <= ch && ch <= '9')
        x = x * 10 + ch - '0', ch = getchar();
    return x;
}

void input()
{
    n = read(), q = read();
    int from, to;
    for (int i = 0; i < n - 1; ++i)
    {
        from = read(), to = read();
        sides[sidesCnt].from = from, sides[sidesCnt].to = to;
        sides[sidesCnt].next = firstSide[from], firstSide[from] = sidesCnt;
        ++sidesCnt;
        sides[sidesCnt].from = to, sides[sidesCnt].to = from;
        sides[sidesCnt].next = firstSide[to], firstSide[to] = sidesCnt;
        ++sidesCnt;
    }
}

void tree(int current = 1, int father = 0)
{
    ST[current][0]    = father;
    depth[current]    = depth[father] + 1;
    subnodes[current] = 1;
    int s             = firstSide[current];
    while (s != 0)
    {
        if (sides[s].to != father)
            tree(sides[s].to, current), subnodes[current] += subnodes[sides[s].to];
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

int subnode(int node, int root)
{
    if (node == root)
        return 0;
    for (int i = log2maxN; i >= 0; --i)
    {
        if (depth[ST[node][i]] > depth[root])
            node = ST[node][i];
    }
    return subnodes[node];
}

void solve()
{
    int a, b, c;
    int lcaAB, lcaBC, lcaAC;
    while (q--)
    {
        a = read(), b = read(), c = read();
        lcaAB = query(a, b), lcaBC = query(b, c), lcaAC = query(a, c);
        if (query(c, lcaAB) != lcaAB)
            printf("0\n");
        else
        {
            if (lcaAB == c)
                printf("%d\n", n - subnode(a, c) - subnode(b, c));
            else if (lcaAC == c)
                printf("%d\n", subnodes[c] - subnode(a, c));
            else if (lcaBC == c)
                printf("%d\n", subnodes[c] - subnode(b, c));
            else
                printf("0\n");
        }
    }
}

int main()
{
    input();
    tree();
    LCA();
    solve();
}