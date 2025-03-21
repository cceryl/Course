#include <cmath>
#include <cstring>
#include <iostream>

inline int read()
{
    int  x  = 0;
    char ch = getchar();
    while (ch < '0' || ch > '9')
        ch = getchar();
    while (ch >= '0' && ch <= '9')
        x = x * 10 + ch - '0', ch = getchar();
    return x;
}

constexpr int maxM = 50005;
int           n, m;

struct EdgeGraph
{
    int from, to;
    int weight;
};
EdgeGraph edgeGraph[maxM];

template <typename T>
void sort(T *begin, T *end, bool (*cmp)(T &, T &))
{
    int len = end - begin;
    if (len <= 1)
        return;
    T *i = begin, *j = end - 1;
    T  pivot = *(begin + (len + 1) / 2 - 1);
    while (j - i >= 0)
    {
        while (cmp(*i, pivot))
            i++;
        while (cmp(pivot, *j))
            j--;
        if (j - i >= 0)
        {
            std::swap(*i, *j);
            i++, j--;
        }
    }
    if (j - begin > 0)
        sort(begin, i, cmp);
    if (end - i > 1)
        sort(i, end, cmp);
}

bool cmp(EdgeGraph &x, EdgeGraph &y) { return x.weight > y.weight; }

struct EdgeTree
{
    int to;
    int next;
    int weight;
};
EdgeTree edgetree[maxM << 2];
int      head[maxM];
int      Cnt;

void add(int from, int to, int weight)
{
    ++Cnt;
    edgetree[Cnt].next = head[from], edgetree[Cnt].to = to, head[from] = Cnt;
    edgetree[Cnt].weight = weight;
}

// DSU
int root[maxM];

int find(int x)
{
    if (x != root[x])
        root[x] = find(root[x]);
    return root[x];
}

// LCA
int father[maxM][20], maxWeight[maxM][20], depth[maxM];

void dfs(int n, int Father, int weight)
{
    depth[n]     = depth[Father] + 1;
    father[n][0] = Father, maxWeight[n][0] = weight;
    for (int i = 1; (1 << i) <= depth[n]; ++i)
    {
        father[n][i]    = father[father[n][i - 1]][i - 1];
        maxWeight[n][i] = std::min(maxWeight[n][i - 1], maxWeight[father[n][i - 1]][i - 1]);
    }
    for (int i = head[n]; i != 0; i = edgetree[i].next)
        if (edgetree[i].to != Father)
            dfs(edgetree[i].to, n, edgetree[i].weight);
}

int lca(int x, int y)
{
    if (depth[x] < depth[y])
        std::swap(x, y);
    int max = 1e9;
    while (depth[x] > depth[y])
    {
        for (int i = log2(depth[x] - depth[y]); i >= 0; --i)
            if (depth[father[x][i]] >= depth[y])
            {
                max = std::min(max, maxWeight[x][i]);
                x   = father[x][i];
            }
    }
    if (x == y)
        return max;
    for (int i = log2(depth[x]); i >= 0; --i)
    {
        if (father[x][i] != father[y][i])
        {
            max = std::min(max, maxWeight[x][i]);
            x   = father[x][i];
            max = std::min(max, maxWeight[y][i]);
            y   = father[y][i];
        }
    }
    max = std::min(max, std::min(maxWeight[x][0], maxWeight[y][0]));
    return max;
}

void kruskal()
{
    int remain = n;
    for (int i = 1; i <= m; ++i)
    {
        if (find(edgeGraph[i].from) != find(edgeGraph[i].to))
        {
            add(edgeGraph[i].from, edgeGraph[i].to, edgeGraph[i].weight);
            add(edgeGraph[i].to, edgeGraph[i].from, edgeGraph[i].weight);
            root[find(edgeGraph[i].from)] = find(edgeGraph[i].to);
            if (--remain == 1)
                break;
        }
    }
}

int main()
{
    n = read(), m = read();
    // DSU
    for (int i = 1; i <= n; i++)
        root[i] = i;
    // Input
    for (int i = 1; i <= m; i++)
    {
        edgeGraph[i].from   = read();
        edgeGraph[i].to     = read();
        edgeGraph[i].weight = read();
    }
    sort(edgeGraph + 1, edgeGraph + m + 1, cmp);
    kruskal();
    for (int i = 1; i <= n; i++)
        if (depth[i] == 0)
            dfs(i, 0, 0);
    int query = read();
    int x, y;
    while (query--)
    {
        x = read(), y = read();
        if (find(x) == find(y))
            printf("%d\n", lca(x, y));
        else
            printf("-1\n");
    }
}