#include <stdio.h>

constexpr int maxN = 110005;

class priority_queue
{
private:
    long long data[maxN];
    int       rear;

public:
    priority_queue() : rear(1) {}
    ~priority_queue() = default;
    void push(long long val)
    {
        ++rear;
        int index = rear - 1;
        while (val < data[index / 2] && index != 1)
        {
            data[index]  = data[index / 2];
            index       /= 2;
        }
        data[index] = val;
    }
    long long pop()
    {
        long long tmp = data[1], r = data[--rear];
        int       index = 1;
        while (index * 2 < rear && r > data[index * 2] || index * 2 + 1 < rear && r > data[index * 2 + 1])
        {
            if (index * 2 + 1 < rear && data[index * 2 + 1] < data[index * 2])
                data[index] = data[index * 2 + 1], index = index * 2 + 1;
            else
                data[index] = data[index * 2], index = index * 2;
        }
        data[index] = r;
        return tmp;
    }
    int size() { return rear - 1; }
} queue;

inline int read()
{
    char ch = getchar();
    int  x  = 0;
    while (ch < '0' || ch > '9')
        ch = getchar();
    while ('0' <= ch && ch <= '9')
    {
        x  = x * 10 + ch - '0';
        ch = getchar();
    }
    return x;
}

int main()
{
    int       n, m;
    long long ans = 0;
    scanf("%d%d", &n, &m);
    for (int i = 0; i < n; ++i)
        queue.push(read());
    int sup = (n - 1) % (m - 1) ? m - 1 - (n - 1) % (m - 1) : 0;
    for (int i = 0; i < sup; ++i)
        queue.push(0);
    while (queue.size() > 1)
    {
        unsigned long long tmp = 0;
        for (int i = 0; i < m; ++i)
            tmp += queue.pop();
        queue.push(tmp);
        ans += tmp;
    }
    printf("%lld", ans);
    return 0;
}