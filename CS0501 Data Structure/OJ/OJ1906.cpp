#include <iostream>
#include <queue>

using namespace std;

constexpr int maxN = 5e3 + 5;
int           n, m, k;

struct edge
{
    int  u = 0, v = 0, w = -1;
    bool operator<(const edge &other) const { return w > other.w; }
};

priority_queue<edge> pq;

void input()
{
    cin >> n >> m >> k;
    int u, v, w;
    for (int i = 0; i < m; ++i)
    {
        cin >> u >> v >> w;
        pq.push(edge({u, v, w}));
    }
}

int root[maxN];
int find(int x)
{
    if (x != root[x])
        root[x] = find(root[x]);
    return root[x];
}

long long result;
void      kruskal()
{
    int  remain = n - k;
    edge e;
    for (int i = 1; i <= m && remain > 1; ++i)
    {
        e = pq.top();
        pq.pop();
        if (find(e.u) != find(e.v))
        {
            result          += e.w;
            root[find(e.u)]  = find(e.v);
            --remain;
        }
    }
}

void solve()
{
    for (int i = 1; i <= n; ++i)
        root[i] = i;
    kruskal();
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    input();
    solve();
    cout << result;
    return 0;
}