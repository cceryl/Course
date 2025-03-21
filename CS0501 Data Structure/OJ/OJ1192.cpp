#include <cmath>
#include <iostream>

constexpr int maxM = 105;
constexpr int inf  = 1e9;
int           m, n;

const int next_x[4] = {1, 0, -1, 0};
const int next_y[4] = {0, 1, 0, -1};

int color[maxM][maxM];
int cost[maxM][maxM];

void dfs(int x, int y, int Cost, int Color)
{
    if (x == m && y == m)
    {
        cost[m][m] = std::min(cost[m][m], Cost);
        return;
    }
    if (Cost >= cost[m][m] || cost[x][y] <= Cost)
        return;
    else
        cost[x][y] = Cost;
    for (int i = 0; i < 4; i++)
    {
        int nextX = x + next_x[i], nextY = y + next_y[i];
        if (nextX < 1 || nextY < 1 || nextX > m || nextY > m)
            continue;
        if (Color == color[nextX][nextY] && Color != 0)
            dfs(nextX, nextY, Cost, Color);
        if (Color != color[nextX][nextY] && Color != 0 && color[nextX][nextY] != 0)
            dfs(nextX, nextY, Cost + 1, -Color);
        if (color[x][y] != 0 && Color * color[nextX][nextY] == 0)
            dfs(nextX, nextY, Cost + 2, Color);
    }
    return;
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cin >> m >> n;
    while (n--)
    {
        int x, y, c;
        std::cin >> x >> y >> c;
        if (c)
            color[x][y] = 1;
        else
            color[x][y] = -1;
    }
    for (int i = 1; i <= m; i++)
        for (int j = 1; j <= m; j++)
            cost[i][j] = inf;
    dfs(1, 1, 0, color[1][1]);
    std::cout << (cost[m][m] == inf ? -1 : cost[m][m]);
    return 0;
}