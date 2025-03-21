#include <iostream>

const int maxN = 1e5 + 5;

struct node
{
    int  sum[26] = {0};
    int  order;
    bool lazy = 0;
    void change(int source[26], int len, int sortOrder)
    {
        for (int i = 0; i < 26; ++i)
            sum[i] = 0;
        lazy = true, order = sortOrder;
        if (order == 1)
        {
            for (int i = 0, total = 0; i < 26; ++i)
            {
                sum[i]     = source[i];
                total     += sum[i];
                source[i]  = 0;
                if (total >= len)
                {
                    source[i]  = total - len;
                    sum[i]    -= source[i];
                    return;
                }
            }
        }
        else
        {
            for (int i = 25, total = 0; i >= 0; --i)
            {
                sum[i]     = source[i];
                total     += sum[i];
                source[i]  = 0;
                if (total >= len)
                {
                    source[i]  = total - len;
                    sum[i]    -= source[i];
                    return;
                }
            }
        }
    }
};
node tree[maxN << 2];
int  n, m;
char str[maxN];
int  Left, Right, res[26];

void pushup(int n)
{
    for (int i = 0; i < 26; ++i)
        tree[n].sum[i] = tree[n << 1].sum[i] + tree[n << 1 | 1].sum[i];
}

void pushdown(int n, int left, int right, int mid, int order)
{
    int tmp[26];
    for (int i = 0; i < 26; ++i)
        tmp[i] = tree[n].sum[i];
    tree[n << 1].change(tmp, mid - left + 1, tree[n].order);
    tree[n << 1 | 1].change(tmp, right - mid, tree[n].order);
    tree[n].lazy = false;
}

void build(int n, int left, int right)
{
    if (left == right)
    {
        tree[n].sum[str[left] - 'a'] = 1;
        tree[n].lazy                 = true;
        return;
    }
    int mid = (left + right) >> 1;
    build(n << 1, left, mid);
    build(n << 1 | 1, mid + 1, right);
    pushup(n);
}

void query(int n, int left, int right)
{
    if (Left <= left && right <= Right)
    {
        for (int i = 0; i < 26; ++i)
            res[i] += tree[n].sum[i];
        return;
    }
    int mid = (left + right) >> 1;
    if (tree[n].lazy)
        pushdown(n, left, right, mid, tree[n].order);
    if (Left <= mid)
        query(n << 1, left, mid);
    if (Right > mid)
        query(n << 1 | 1, mid + 1, right);
}

void update(int n, int left, int right, int order)
{
    if (Left <= left && right <= Right)
    {
        tree[n].change(res, right - left + 1, order);
        return;
    }
    int mid = (left + right) >> 1;
    if (Left <= mid)
        update(n << 1, left, mid, order);
    if (Right > mid)
        update(n << 1 | 1, mid + 1, right, order);
    pushup(n);
}

void print(int n, int left, int right)
{
    if (tree[n].lazy)
    {
        if (tree[n].order == 1)
            for (int i = 0; i < 26; ++i)
                for (int j = 0; j < tree[n].sum[i]; ++j)
                    putchar(i + 'a');
        else
            for (int i = 25; i >= 0; --i)
                for (int j = 0; j < tree[n].sum[i]; ++j)
                    putchar(i + 'a');
        return;
    }
    int mid = (left + right) >> 1;
    print(n << 1, left, mid);
    print(n << 1 | 1, mid + 1, right);
}

int main()
{
    scanf("%d%d", &n, &m);
    scanf("%s", str + 1);
    build(1, 1, n);
    for (int i = 1, order; i <= m; ++i)
    {
        scanf("%d%d%d", &Left, &Right, &order);
        for (int i = 0; i < 26; ++i)
            res[i] = 0;
        query(1, 1, n);
        update(1, 1, n, order);
    }
    print(1, 1, n);
    return 0;
}