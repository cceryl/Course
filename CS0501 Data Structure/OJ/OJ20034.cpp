#include <cstring>
#include <iostream>

constexpr int maxN = 5e4 + 5;
int           N;

struct edge
{
    int to, next;
};
edge edges[maxN << 2];
int  head[maxN];
int  Cnt;

void add(int x, int y)
{
    ++Cnt;
    edges[Cnt].next = head[x], edges[Cnt].to = y, head[x] = Cnt;
}

int  root, child;
int  tarCnt;
int  dfn[maxN], low[maxN];
bool cut[maxN];

void tarjan(int n, int father)
{
    dfn[n] = low[n] = ++tarCnt;
    for (int i = head[n]; i != 0; i = edges[i].next)
    {
        if (dfn[edges[i].to] == 0)
        {
            tarjan(edges[i].to, n);
            low[n] = std::min(low[n], low[edges[i].to]);
            if (low[edges[i].to] >= dfn[n])
            {
                if (n == root)
                    ++child;
                if (n != root || child >= 2)
                    cut[n] = true;
            }
        }
        else if (edges[i].to != father)
            low[n] = std::min(low[n], dfn[edges[i].to]);
    }
}

int       exits, ex;
long long options, op;

int group;
int visit[maxN];

void dfs(int n)
{
    visit[n] = group;
    if (cut[n])
        return;
    for (int i = head[n]; i != 0; i = edges[i].next)
    {
        if (cut[edges[i].to] && visit[edges[i].to] != group)
            ++ex, visit[edges[i].to] = group;
        if (!visit[edges[i].to])
            dfs(edges[i].to);
    }
    ++op;
}

int nodes, cases;

void initialize()
{
    Cnt = tarCnt = nodes = group = 0;
    exits = 0, options = 1;
    memset(dfn, 0, sizeof(dfn));
    memset(low, 0, sizeof(low));
    memset(visit, 0, sizeof(visit));
    memset(cut, 0, sizeof(cut));
    memset(edges, 0, sizeof(edges));
    memset(head, 0, sizeof(head));
}

void input()
{
    int x, y;
    while (N--)
    {
        std::cin >> x >> y;
        add(x, y);
        add(y, x);
        nodes = std::max(std::max(x, y), nodes);
    }
}

int main()
{
    while (true)
    {
        ++cases;
        std::cin >> N;
        if (N == 0)
            break;
        initialize();
        input();
        for (int i = 1; i <= nodes; i++)
        {
            if (dfn[i] == 0)
            {
                root  = i;
                child = 0;
                tarjan(i, 0);
            }
        }
        for (int i = 1; i <= nodes; i++)
        {
            if (!cut[i] && !visit[i])
            {
                ++group;
                op = ex = 0;
                dfs(i);
                if (ex == 0)
                {
                    exits   += 2;
                    options *= op * (op - 1) / 2;
                }
                else if (ex == 1) // 有一个割点
                {
                    ++exits;
                    options *= op;
                }
            }
        }
        std::cout << "Case " << cases << ": " << exits << ' ' << options << '\n';
    }
    return 0;
}