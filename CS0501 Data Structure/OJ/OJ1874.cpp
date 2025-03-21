#include <iostream>

const int maxN = 1000000;

struct node
{
    unsigned long long herb    = 1;
    bool               lazy    = false;
    int                lazyval = 0;
};

node *tree = new node[maxN << 2];

void pushup(int n) { tree[n].herb = tree[n << 1].herb | tree[n << 1 | 1].herb; }

void change(int n, int left, int right, unsigned long long val)
{
    tree[n].herb    = 1 << (val - 1);
    tree[n].lazy    = true;
    tree[n].lazyval = val;
}

void pushdown(int n, int left, int right)
{
    if (tree[n].lazy == false)
        return;
    if (left == right)
    {
        tree[n].lazy = false;
        return;
    }
    tree[n << 1].lazy = tree[n << 1 | 1].lazy = true;
    tree[n << 1].herb = tree[n << 1 | 1].herb = 1 << (tree[n].lazyval - 1);
    tree[n << 1].lazyval = tree[n << 1 | 1].lazyval = tree[n].lazyval;
    tree[n].lazy                                    = false;
}

void update(int n, int left, int right, int updateLeft, int updateRight, unsigned long long updateVal)
{
    pushdown(n, left, right);
    if (updateLeft <= left && updateRight >= right)
    {
        change(n, left, right, updateVal);
        return;
    }
    int mid = (left + right) >> 1;
    if (updateLeft <= mid)
        update((n << 1), left, mid, updateLeft, updateRight, updateVal);
    if (updateRight > mid)
        update((n << 1) + 1, mid + 1, right, updateLeft, updateRight, updateVal);
    pushup(n);
}

unsigned long long query(int n, int left, int right, int queryLeft, int queryRight)
{
    pushdown(n, left, right);
    if (queryLeft <= left && queryRight >= right)
        return tree[n].herb;
    int mid = (left + right) >> 1;
    if (queryRight <= mid)
        return query((n << 1), left, mid, queryLeft, queryRight);
    if (queryLeft > mid)
        return query((n << 1) + 1, mid + 1, right, queryLeft, queryRight);
    else
        return query((n << 1), left, mid, queryLeft, queryRight) | query((n << 1) + 1, mid + 1, right, queryLeft, queryRight);
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int  N, M, K;
    char op;
    int  left, right, herb;
    std::cin >> N >> K >> M;
    while (M--)
    {
        std::cin >> op;
        if (op == 'C')
        {
            std::cin >> left >> right >> herb;
            update(1, 1, N, left, right, herb);
        }
        else // op == 'P'
        {
            std::cin >> left >> right;
            unsigned long long x   = query(1, 1, N, left, right) << '\n';
            int                cnt = 0;
            unsigned long long lowbit;
            while (x != 0)
            {
                lowbit  = x & -x;
                x      -= lowbit;
                ++cnt;
            }
            std::cout << cnt << '\n';
        }
    }
    return 0;
}