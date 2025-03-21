#include <iostream>
#include <stack>
#include <vector>

using namespace std;

constexpr long long inf  = 0x7fffffff;
constexpr int       maxN = 5e4;
int                 n, m, s, t;

struct edge
{
    int u;
    int v;
    int weight;
};

struct node
{
    long long   cost = inf;
    int         suc;
    vector<int> fel;
};

vector<edge> edges;
node         weight_start[maxN];
node         weight_target[maxN];
bool         visit_start[maxN];
bool         visit_target[maxN];
bool         visit[maxN];

inline void insert(int l, int r)
{
    weight_start[l].fel.push_back(r);
    weight_target[r].fel.push_back(l);
}

void dfs_start(int u)
{
    stack<int> S;
    S.push(u);
    visit_start[u] = true;
    while (!S.empty())
    {
        int  top  = S.top();
        int  len  = weight_start[S.top()].fel.size();
        bool flag = false;
        for (int i = 0; i < len; i++)
        {
            if (!visit_start[weight_start[top].fel[i]])
            {
                flag                                  = true;
                visit_start[weight_start[top].fel[i]] = true;
                S.push(weight_start[top].fel[i]);
            }
        }
        if (!flag)
            S.pop();
    }
}

void dfs_target(int u)
{
    stack<int> S;
    S.push(u);
    visit_target[u] = true;
    while (!S.empty())
    {
        int  top  = S.top();
        int  len  = weight_target[S.top()].fel.size();
        bool flag = false;
        for (int i = 0; i < len; i++)
        {
            if (!visit_target[weight_target[top].fel[i]])
            {
                flag                                    = true;
                visit_target[weight_target[top].fel[i]] = true;
                S.push(weight_target[top].fel[i]);
            }
        }
        if (!flag)
            S.pop();
    }
}

vector<int> BellFord()
{
    vector<int> res;
    for (int i = 0; i < n; i++)
    {
        bool flag = false;
        for (unsigned int j = 0; j < edges.size(); j++)
        {
            if (weight_start[edges[j].v].cost > weight_start[edges[j].u].cost + edges[j].weight)
            {
                flag                          = true;
                weight_start[edges[j].v].suc  = edges[j].u;
                weight_start[edges[j].v].cost = weight_start[edges[j].u].cost + edges[j].weight;
            }
        }
        if (!flag)
            return res;
    }
    for (unsigned int j = 0; j < edges.size(); j++)
    {
        if (weight_start[edges[j].v].cost > weight_start[edges[j].u].cost + edges[j].weight)
        {
            weight_start[edges[j].v].suc = edges[j].u;
            res.push_back(edges[j].v);
            weight_start[edges[j].v].cost = weight_start[edges[j].u].cost + edges[j].weight;
        }
    }
    return res;
}

bool neg(int u)
{
    while (!visit[u])
    {
        if (visit_start[u] && visit_target[u])
        {
            return true;
        }
        visit[u] = true;
        u        = weight_start[u].suc;
    }
    return false;
}

void search(vector<int> &v)
{
    int len = v.size();
    for (int i = 0; i < len; i++)
    {
        if (neg(v[i]))
        {
            cout << 1;
            return;
        }
        for (int i = 1; i <= n; i++)
            visit[i] = false;
    }
    cout << 0;
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n >> m >> s >> t;
    edges.reserve(m);
    int u, v, w;
    while (m--)
    {
        cin >> u >> v >> w;
        edges.push_back({u, v, w});
        insert(u, v);
    }
    dfs_start(s);
    dfs_target(t);
    weight_start[s].cost = 0;
    vector<int> vec      = BellFord();
    if (vec.empty())
        cout << 0;
    else
        search(vec);
    return 0;
}