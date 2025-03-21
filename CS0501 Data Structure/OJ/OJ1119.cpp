#include <iostream>

struct node
{
    double sum    = 0;
    double sqrsum = 0;
    double lazy   = 0;
};

const int maxN = 1e5 + 5;
int       N, M;
double    nums[maxN];
node      tree[maxN << 3];

void pushup(int n)
{
    tree[n].sum    = tree[(n << 1)].sum + tree[(n << 1) + 1].sum;
    tree[n].sqrsum = tree[(n << 1)].sqrsum + tree[(n << 1) + 1].sqrsum;
}

void build(int n, int left, int right)
{
    if (left == right)
    {
        tree[n].sum    = nums[left];
        tree[n].sqrsum = nums[left] * nums[left];
        return;
    }
    int mid = (left + right) >> 1;
    build((n << 1), left, mid);
    build((n << 1) + 1, mid + 1, right);
    pushup(n);
}

void change(int n, int left, int right, double changeVal)
{
    tree[n].sqrsum += (right - left + 1) * changeVal * changeVal + 2 * changeVal * tree[n].sum;
    tree[n].sum    += (right - left + 1) * changeVal;
    tree[n].lazy   += changeVal;
}

void pushdown(int n, int left, int right)
{
    if (tree[n].lazy == 0)
        return;
    int mid = (left + right) >> 1;
    change((n << 1), left, mid, tree[n].lazy);
    change((n << 1) + 1, mid + 1, right, tree[n].lazy);
    tree[n].lazy = 0;
}

void update(int n, int left, int right, int updateLeft, int updateRight, double updateVal)
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

double querySum(int n, int left, int right, int queryLeft, int queryRight)
{
    pushdown(n, left, right);
    if (queryLeft <= left && queryRight >= right)
        return tree[n].sum;
    int mid = (left + right) >> 1;
    if (queryRight <= mid)
        return querySum((n << 1), left, mid, queryLeft, queryRight);
    if (queryLeft > mid)
        return querySum((n << 1) + 1, mid + 1, right, queryLeft, queryRight);
    else
        return querySum((n << 1), left, mid, queryLeft, queryRight) + querySum((n << 1) + 1, mid + 1, right, queryLeft, queryRight);
}

double querySqrsum(int n, int left, int right, int queryLeft, int queryRight)
{
    pushdown(n, left, right);
    if (queryLeft <= left && queryRight >= right)
        return tree[n].sqrsum;
    int mid = (left + right) >> 1;
    if (queryRight <= mid)
        return querySqrsum((n << 1), left, mid, queryLeft, queryRight);
    if (queryLeft > mid)
        return querySqrsum((n << 1) + 1, mid + 1, right, queryLeft, queryRight);
    else
        return querySqrsum((n << 1), left, mid, queryLeft, queryRight) + querySqrsum((n << 1) + 1, mid + 1, right, queryLeft, queryRight);
}

void input()
{
    std::cin >> N >> M;
    for (int i = 1; i <= N; ++i)
        std::cin >> nums[i];
    build(1, 1, N);
}

void solve()
{
    int    op, x, y;
    double k, avg;
    while (M--)
    {
        std::cin >> op;
        switch (op)
        {
        case 1:
            std::cin >> x >> y >> k;
            update(1, 1, N, x, y, k);
            break;
        case 2:
            std::cin >> x >> y;
            std::cout << (int)(100 * querySum(1, 1, N, x, y) / (y - x + 1)) << '\n';
            break;
        case 3:
            std::cin >> x >> y;
            avg = querySum(1, 1, N, x, y) / (y - x + 1);
            std::cout << (int)(100 * (querySqrsum(1, 1, N, x, y) / (y - x + 1) - avg * avg)) << '\n';
            break;
        default: break;
        }
    }
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    input();
    solve();
    return 0;
}