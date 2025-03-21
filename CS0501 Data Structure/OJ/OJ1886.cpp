#include <algorithm>
#include <cstring>
#include <iostream>

const int maxN = 1e4 + 5;
struct homework
{
    int ddl;
    int score;
};

homework list[maxN];
template <typename T>

void sort(T *begin, T *end, bool (*cmp)(T &, T &))
{
    int len = end - begin;
    if (len <= 1)
        return;
    T *i = begin, *j = end - 1;
    T  pivot = *(begin + (len + 1) / 2 - 1);
    while (j - i >= 0)
    {
        while (cmp(*i, pivot))
            i++;
        while (cmp(pivot, *j))
            j--;
        if (j - i >= 0)
        {
            std::swap(*i, *j);
            i++, j--;
        }
    }
    if (j - begin > 0)
        sort(begin, i, cmp);
    if (end - i > 1)
        sort(i, end, cmp);
}

bool cmp(homework &x, homework &y) { return x.score > y.score; }

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int round;
    std::cin >> round;
    while (round--)
    {
        int n, score = 0;
        std::cin >> n;
        for (int i = 0; i < n; ++i)
        {
            std::cin >> list[i].score >> list[i].ddl;
            score += list[i].score;
        }
        sort(list, list + n, cmp);
        bool occupied[maxN] = {0};
        for (int i = 0; i < n; ++i)
        {
            int finishDate = list[i].ddl;
            while (finishDate > 0)
            {
                if (!occupied[finishDate])
                {
                    occupied[finishDate] = true;
                    break;
                }
                --finishDate;
            }
            if (finishDate <= 0)
                score -= list[i].score;
        }
        std::cout << score << '\n';
    }
    return 0;
}