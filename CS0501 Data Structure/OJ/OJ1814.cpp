#include <iostream>
using std::cin, std::cout;

const int maxN = 1e7;
int       height[maxN], stack[maxN];

inline int read()
{
    int  x = 0, f = 1;
    char ch = getchar();
    while (ch < '0' || ch > '9')
    {
        if (ch == '-')
            f = -1;
        ch = getchar();
    }
    while (ch >= '0' && ch <= '9')
        x = x * 10 + ch - '0', ch = getchar();
    return x * f;
}

int main()
{
    int n;
    cin >> n;
    for (int i = 0; i < n; ++i)
        height[i] = read();
    int top = 1, ans = 1;
    stack[0] = 0;
    for (int i = 1; i < n; ++i)
    {
        while (top > 0 && height[i] < height[stack[top - 1]])
            --top;
        if (top > 0 && height[i] == height[stack[top - 1]])
            --top;
        else
            ++ans;
        stack[top++] = i;
    }
    cout << ans;
}