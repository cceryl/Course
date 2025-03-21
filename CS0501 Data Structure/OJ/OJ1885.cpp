#include <iostream>

const int maxM = 1e5 + 5;
int       n, m;

int father[maxM];

struct relation
{
    int       catA, catB;
    long long weight;
};

relation relations[maxM];

int find(int x)
{
    if (father[x] == x)
        return x;
    return father[x] = find(father[x]);
}

void Union(int x, int y)
{
    int faX = find(x), faY = find(y);
    father[faX] = faY;
}

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

bool cmp(relation &x, relation &y) { return x.weight > y.weight; }

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cin >> n >> m;
    for (int i = 1; i <= 2 * n; ++i)
        father[i] = i;
    for (int i = 1; i <= m; ++i)
        std::cin >> relations[i].catA >> relations[i].catB >> relations[i].weight;
    sort<relation>(relations + 1, relations + m + 1, cmp);
    for (int i = 1; i <= m; ++i)
    {
        if (find(relations[i].catA) == find(relations[i].catB))
        {
            std::cout << relations[i].weight;
            return 0;
        }
        Union(relations[i].catA, relations[i].catB + n);
        Union(relations[i].catB, relations[i].catA + n);
    }
    std::cout << 0;
    return 0;
}