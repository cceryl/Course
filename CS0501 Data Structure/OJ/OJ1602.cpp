#include <iostream>
using std::cin, std::cout;

// 1-base
const int maxN = 1e5 + 5, maxH = 1e5 + 5;
int       height[maxN];
int       left[maxN], right[maxN];
int       leftHigher[maxN], rightHigher[maxN];
int       n;

void input()
{
    cin >> n;
    for (int i = 1; i <= n; ++i)
    {
        cin >> height[i];
        left[i] = right[i] = 0;
    }
}

inline int lowbit(int x) { return x & -x; }

void insert(int index, int *tree)
{
    while (index <= maxH)
    {
        ++tree[index];
        index += lowbit(index);
    }
}

int profix(int index, int *tree)
{
    int ans = 0;
    while (index != 0)
    {
        ans   += tree[index];
        index -= lowbit(index);
    }
    return ans;
}

int solve()
{
    int ans = 0;
    for (int i = 1; i <= n; ++i)
    {
        insert(height[i], left);
        leftHigher[i] = i - profix(height[i], left);
    }
    for (int i = n; i >= 1; --i)
    {
        insert(height[i], right);
        rightHigher[i] = (n + 1 - i) - profix(height[i], right);
    }
    for (int i = 1; i <= n; ++i)
        if (leftHigher[i] * 2 < rightHigher[i] || leftHigher[i] > rightHigher[i] * 2)
            ++ans;
    return ans;
}

int main()
{
    cin.sync_with_stdio(false);
    input();
    cout << solve();
    return 0;
}