#include <iostream>

constexpr int       maxN = 1e5 + 5, maxLen = 1e6 + 5;
constexpr long long inf = 1e17L;
int                 n;

struct platform
{
    int       left, right;
    int       height;
    long long energy;
};

platform platforms[maxN];
platform tmp[maxN];

void sort(int left, int right)
{
    if (right - left == 1)
        return;
    int mid = (left + right + 1) >> 1;
    int i = left, j = mid, k = left;
    sort(left, mid);
    sort(mid, right);
    while (i < mid && j < right)
    {
        if (platforms[i].height > platforms[j].height)
            tmp[k++] = platforms[i++];
        else
            tmp[k++] = platforms[j++];
    }
    while (i < mid)
        tmp[k++] = platforms[i++];
    while (j < right)
        tmp[k++] = platforms[j++];
    for (i = left; i < right; ++i)
        platforms[i] = tmp[i];
}

struct node
{
    int       left, right;
    long long energy;
    bool      lazy = false;
};

node tree[maxLen << 3];

inline void pushup(int n) { tree[n].energy = std::max(tree[n << 1].energy, tree[n << 1 | 1].energy); }
//
void        build(int n, int left, int right)
{
    tree[n].left = left, tree[n].right = right;
    if (left == right)
    {
        if (left == platforms[0].left || left == platforms[0].right)
            tree[n].energy = 100 + platforms[0].energy;
        else
            tree[n].energy = -inf;
        return;
    }
    int mid = (tree[n].left + tree[n].right) >> 1;
    build(n << 1, left, mid);
    build(n << 1 | 1, mid + 1, right);
    pushup(n);
}

void pushdown(int n)
{
    if (!tree[n].lazy || tree[n].left == tree[n].right)
        return;
    tree[n].lazy        = false;
    tree[n << 1].energy = tree[n << 1 | 1].energy = -inf;
    tree[n << 1].lazy = tree[n << 1 | 1].lazy = true;
}

int       Left, Right;
long long Energy;

void update(int n)
{
    if (Left == tree[n].left && Left == tree[n].right)
    {
        tree[n].energy = std::max(tree[n].energy, Energy);
        return;
    }
    pushdown(n);
    int mid = (tree[n].left + tree[n].right) >> 1;
    if (Left <= mid)
        update(n << 1);
    else
        update(n << 1 | 1);
    pushup(n);
}

void clear(int n)
{
    if (Right < tree[n].left || Left > tree[n].right)
        return;
    if (Left <= tree[n].left && Right >= tree[n].right)
    {
        tree[n].energy = -inf;
        tree[n].lazy   = true;
        return;
    }
    pushdown(n);
    int mid = (tree[n].left + tree[n].right) >> 1;
    if (Left <= mid)
        clear(n << 1);
    if (Right > mid)
        clear(n << 1 | 1);
    pushup(n);
}

long long query(int n)
{
    if (Right < tree[n].left || Left > tree[n].right)
        return -inf;
    if (Left <= tree[n].left && Right >= tree[n].right)
    {
        return tree[n].energy;
    }
    pushdown(n);
    int       mid = (tree[n].left + tree[n].right) >> 1;
    long long ans = -inf;
    if (Left <= mid)
        ans = query(n << 1);
    if (Right > mid)
        ans = std::max(ans, query(n << 1 | 1));
    return ans;
}

void input()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cin >> n;
    int min = 1e9, max = 0;
    for (int i = 0; i < n; ++i)
    {
        std::cin >> platforms[i].height >> platforms[i].left >> platforms[i].right >> platforms[i].energy;
        min = std::min(min, platforms[i].left), max = std::max(max, platforms[i].right);
    }
    sort(0, n);
    build(1, min, max);
}

void solve()
{
    for (int i = 1; i < n; ++i)
    {
        Left = platforms[i].left, Right = platforms[i].right;
        long long current = query(1);
        Energy            = current + platforms[i].energy;
        clear(1);
        if (current > 0 && Energy > 0)
        {
            Left = platforms[i].left;
            update(1);
            if (platforms[i].right != platforms[i].left)
            {
                Left = platforms[i].right;
                update(1);
            }
        }
        if (tree[1].energy <= 0)
            break;
    }
    if (tree[1].energy <= 0)
        std::cout << -1;
    else
        std::cout << tree[1].energy;
}

int main()
{
    input();
    solve();
    return 0;
}