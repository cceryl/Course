#include <iostream>
using std::cin, std::cout, std::max;

const short maxLen = 3005;
int         sum[maxLen][maxLen];
short       n, m, a, b, c, d;

int areaAB(const short &x, const short &y) { return sum[x][y] - sum[x - a][y] - sum[x][y - b] + sum[x - a][y - b]; }
int areaCD(const short &x, const short &y) { return sum[x][y] - sum[x - c][y] - sum[x][y - d] + sum[x - c][y - d]; }

short queRow[maxLen][maxLen], que[maxLen];
short headRow[maxLen], rearRow[maxLen], head, rear;

void input()
{
    cin >> n >> m >> a >> b >> c >> d;
    for (short i = 0; i < maxLen; ++i)
        sum[i][0] = sum[0][i] = 0;
    for (short i = 1; i <= n; ++i)
        for (short j = 1; j <= m; ++j)
        {
            cin >> sum[i][j];
            sum[i][j] += sum[i - 1][j] + sum[i][j - 1] - sum[i - 1][j - 1];
        }
}

int solve()
{
    int ans = 0;
    head = 0, rear = 0;
    for (short i = 0; i < maxLen; ++i)
        headRow[i] = rearRow[i] = 0;
    for (short i = c; i < n; ++i)
    {
        head = rear = 0;
        for (short j = d; j < m; ++j)
        {
            short &Head = headRow[j], &Rear = rearRow[j], *queue = queRow[j];
            while (Head < Rear && queue[Head] - c <= i - a + 1)
                ++Head;
            while (Head < Rear && areaCD(queue[Rear - 1], j) >= areaCD(i, j))
                --Rear;
            queue[Rear++] = i;
            while (head < rear && que[head] - d <= j - b + 1)
                ++head;
            while (head < rear && areaCD(queRow[que[rear - 1]][headRow[que[rear - 1]]], que[rear - 1]) >= areaCD(queue[Head], j))
                --rear;
            que[rear++] = j;
            if (i >= a - 1 && j >= b - 1)
                ans = max(ans, areaAB(i + 1, j + 1) - areaCD(queRow[que[head]][headRow[que[head]]], que[head]));
        }
    }
    return ans;
}

int main()
{
    cin.sync_with_stdio(false);
    input();
    cout << solve();
    return 0;
}