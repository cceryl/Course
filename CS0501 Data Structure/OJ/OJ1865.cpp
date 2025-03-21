#include <iostream>

const int maxN = 1e5 + 5;

struct node
{
    long long flower   = 0;
    long long lazyAdd  = 0;
    long long lazyMult = 1;
};

node      tree[maxN << 2];
int       N, Q;
long long mod;

void pushdown(int n, int left, int right)
{
    if (left == right)
        return;
    int mid = (left + right) >> 1;

    tree[n << 1].flower   = (tree[n << 1].flower * tree[n].lazyMult + tree[n].lazyAdd * (mid - left + 1)) % mod;
    tree[n << 1].lazyAdd  = (tree[n << 1].lazyAdd * tree[n].lazyMult + tree[n].lazyAdd) % mod;
    tree[n << 1].lazyMult = (tree[n << 1].lazyMult * tree[n].lazyMult) % mod;

    tree[n << 1 | 1].flower   = (tree[n << 1 | 1].flower * tree[n].lazyMult + tree[n].lazyAdd * (right - mid)) % mod;
    tree[n << 1 | 1].lazyAdd  = (tree[n << 1 | 1].lazyAdd * tree[n].lazyMult + tree[n].lazyAdd) % mod;
    tree[n << 1 | 1].lazyMult = (tree[n << 1 | 1].lazyMult * tree[n].lazyMult) % mod;

    tree[n].lazyAdd  = 0;
    tree[n].lazyMult = 1;
}

void add(int n, int left, int right, int addLeft, int addRight, long long val)
{
    pushdown(n, left, right);
    if (addLeft <= left && right <= addRight)
    {
        tree[n].flower  = (tree[n].flower + (right - left + 1) * val) % mod;
        tree[n].lazyAdd = (tree[n].lazyAdd + val) % mod;
        return;
    }
    int mid = (left + right) >> 1;
    if (addLeft <= mid)
        add(n << 1, left, mid, addLeft, addRight, val);
    if (addRight > mid)
        add(n << 1 | 1, mid + 1, right, addLeft, addRight, val);
    tree[n].flower = (tree[n << 1].flower + tree[n << 1 | 1].flower) % mod;
}

void mult(int n, int left, int right, int multLeft, int multRight, long long val)
{
    pushdown(n, left, right);
    if (multLeft <= left && right <= multRight)
    {
        tree[n].flower   = tree[n].flower * val % mod;
        tree[n].lazyAdd  = tree[n].lazyAdd * val % mod;
        tree[n].lazyMult = tree[n].lazyMult * val % mod;
        return;
    }
    int mid = (left + right) >> 1;
    if (multLeft <= mid)
        mult(n << 1, left, mid, multLeft, multRight, val);
    if (multRight > mid)
        mult(n << 1 | 1, mid + 1, right, multLeft, multRight, val);
    tree[n].flower = (tree[n << 1].flower + tree[n << 1 | 1].flower) % mod;
}

long long query_sum(int n, int left, int right, int queryLeft, int queryRight)
{
    pushdown(n, left, right);
    if (queryLeft <= left && right <= queryRight)
        return tree[n].flower;
    int mid = (left + right) >> 1;
    if (queryRight <= mid)
        return query_sum(n << 1, left, mid, queryLeft, queryRight);
    if (queryLeft > mid)
        return query_sum(n << 1 | 1, mid + 1, right, queryLeft, queryRight);
    return (query_sum(n << 1, left, mid, queryLeft, queryRight) + query_sum(n << 1 | 1, mid + 1, right, queryLeft, queryRight)) % mod;
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cin >> N >> mod;
    int flower;
    for (int i = 1; i <= N; i++)
    {
        std::cin >> flower;
        add(1, 1, N, i, i, flower);
    }
    int       left, right, op;
    long long updateVal;
    std::cin >> Q;
    while (Q--)
    {
        std::cin >> op;
        switch (op)
        {
        case 1:
            std::cin >> left >> right >> updateVal;
            mult(1, 1, N, left, right, updateVal);
            break;
        case 2:
            std::cin >> left >> right >> updateVal;
            add(1, 1, N, left, right, updateVal);
            break;
        case 3:
            std::cin >> left >> right;
            std::cout << query_sum(1, 1, N, left, right) << '\n';
            break;
        }
    }
    return 0;
}