#include <iostream>

const int maxN = 3e5 * 4;
const int maxM = 50;

bool oddNode[maxM];
bool visited[maxM];

int to[maxN], next[maxN];
int head[maxN], edge = 0;

void add(int a, int b)
{
    ++edge;
    to[edge] = b, next[edge] = head[a], head[a] = edge;
}

void dfs(int n)
{
    visited[n] = true;
    for (int i = head[n]; i != 0; i = next[i])
        if (!visited[to[i]])
            dfs(to[i]);
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, a, b;
    std::cin >> n;
    while (n--)
    {
        std::cin >> a >> b;
        oddNode[a] = !oddNode[a];
        oddNode[b] = !oddNode[b];
        add(a, b);
        add(b, a);
    }
    for (int i = 0; i < maxM; ++i)
        if (head[i] != 0)
        {
            dfs(i);
            break;
        }
    int sum = 0;
    for (int i = 0; i < maxM; ++i)
    {
        sum += oddNode[i];
        if ((head[i] != 0 && !visited[i]) || sum > 2)
        {
            std::cout << "Impossible";
            return 0;
        }
    }
    std::cout << "Possible";
    return 0;
}