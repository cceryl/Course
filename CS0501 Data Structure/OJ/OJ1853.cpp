#include <iostream>

struct node
{
    int  height     = 0;
    bool lazy       = 0;
    int  lazyHeight = 0;
};

const int maxN = 100005;
int       N, M;
node      tree[maxN << 3];

void pushup(int n) { tree[n].height = std::max(tree[n << 1].height, tree[n << 1 | 1].height); }

void change(int n, int left, int right, int newHeight)
{
    if (newHeight > tree[n].height)
        tree[n].height = newHeight;
    if (tree[n].lazy)
        tree[n].lazyHeight = std::max(tree[n].lazyHeight, newHeight);
    else
    {
        tree[n].lazy       = true;
        tree[n].lazyHeight = newHeight;
    }
}

void pushdown(int n, int left, int right)
{
    if (tree[n].lazy == false)
        return;
    int mid = (left + right) >> 1;
    change(n << 1, left, mid, tree[n].lazyHeight);
    change(n << 1 | 1, mid + 1, right, tree[n].lazyHeight);
    tree[n].lazy = false;
}

void update(int n, int left, int right, int updateLeft, int updateRight, int updateHeight)
{
    pushdown(n, left, right);
    if (updateLeft <= left && updateRight >= right)
    {
        change(n, left, right, updateHeight);
        return;
    }
    int mid = (left + right) >> 1;
    if (updateLeft <= mid)
        update(n << 1, left, mid, updateLeft, updateRight, updateHeight);
    if (updateRight > mid)
        update(n << 1 | 1, mid + 1, right, updateLeft, updateRight, updateHeight);
    pushup(n);
}

int  ans[maxN];
void query(int n, int left, int right)
{
    pushdown(n, left, right);
    if (left == right)
    {
        ans[left] = tree[n].height;
        return;
    }
    int mid = (left + right) >> 1;
    query(n << 1, left, mid);
    query(n << 1 | 1, mid + 1, right);
}

void print()
{
    int last = 0;
    for (int i = 0; i < maxN; ++i)
    {
        if (ans[i] == last)
            continue;
        std::cout << i << ' ' << ans[i] << '\n';
        last = ans[i];
    }
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int L, H, R;
    int num;
    std::cin >> num;
    while (num--)
    {
        std::cin >> L >> H >> R;
        update(1, 1, maxN - 5, L, R - 1, H);
    }
    query(1, 1, maxN - 5);
    print();
    return 0;
}