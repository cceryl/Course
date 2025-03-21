#include <iostream>

using std::cin, std::cout;

const int maxN = 1e5 + 5;
int       cost[maxN];
long long sum[maxN] = {0};
int       stack[maxN];

int main()
{
    int       n, size = 0;
    long long max = 0;
    cin >> n;
    cin >> cost[0];
    sum[0] = cost[0];
    for (int i = 1; i < n; ++i)
    {
        cin >> cost[i];
        sum[i] = sum[i - 1] + cost[i];
    }
    for (int i = 0; i < n; ++i)
    {
        while (size > 0 && cost[stack[size - 1]] > cost[i])
        {
            --size;
            long long res = cost[stack[size]] * (size == 0 ? sum[i - 1] : sum[i - 1] - sum[stack[size - 1]]);
            if (max < res)
                max = res;
        }
        stack[size++] = i;
    }
    while (size > 1)
    {
        --size;
        long long res = cost[stack[size]] * (sum[n - 1] - sum[stack[size - 1]]);
        if (max < res)
            max = res;
    }
    max = cost[stack[0]] * sum[n - 1] > max ? cost[stack[0]] * sum[n - 1] : max;
    cout << max;
}