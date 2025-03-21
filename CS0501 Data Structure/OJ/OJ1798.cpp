#include <iostream>

using std::cin, std::cout, std::getchar;

const int maxlen = 1e7 + 5;
short     luck[maxlen];

inline short read()
{
    short x = 0, f = 1;
    char  ch = getchar();
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
    int n, m;
    cin >> n >> m;
    for (int i = 0; i < n; ++i)
        luck[i] = read();
    int       left, right;
    long long value, maxvalue;
    left = right = 0;
    value = maxvalue = luck[0];
    while (right < n)
    {
        ++right;
        if (right - left + 1 > m)
            value -= luck[left++];
        if (value > 0)
            value += luck[right];
        else
            left = right, value = luck[right];
        if (value > maxvalue)
            maxvalue = value;
    }
    cout << maxvalue;
}