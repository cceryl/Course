#include <iostream>

const int maxN = 50005;
int       N, M;

struct node
{
    int leftVacant = 0, rightVacant = 0, maxVacant = 0;
    int left = 0, right = 0;
    int lazy = 1;
};

node tree[maxN << 2];

void pushup(int n, int left, int right)
{
    int mid            = (left + right) >> 1;
    tree[n].leftVacant = (tree[tree[n].left].leftVacant == (mid - left + 1) ? tree[tree[n].left].leftVacant + tree[tree[n].right].leftVacant
                                                                            : tree[tree[n].left].leftVacant);
    tree[n].rightVacant =
        (tree[tree[n].right].rightVacant == (right - mid) ? tree[tree[n].right].rightVacant + tree[tree[n].left].rightVacant
                                                          : tree[tree[n].right].rightVacant);
    tree[n].maxVacant = std::max(std::max(tree[tree[n].left].maxVacant, tree[tree[n].right].maxVacant),
                                 tree[tree[n].right].leftVacant + tree[tree[n].left].rightVacant);
    if (((tree[tree[n].left].lazy | tree[tree[n].right].lazy) == 2) || tree[tree[n].left].lazy != tree[tree[n].right].lazy)
        tree[n].lazy = 2;
}

void pushdown(int n, int left, int right)
{
    int mid = (left + right) >> 1;
    if (tree[n].lazy == 1)
    {
        tree[tree[n].left].leftVacant = tree[tree[n].left].maxVacant = tree[tree[n].left].rightVacant = (mid - left + 1);
        tree[tree[n].right].leftVacant = tree[tree[n].right].rightVacant = tree[tree[n].right].maxVacant = (right - mid);
        tree[tree[n].left].lazy = tree[tree[n].right].lazy = 1;
    }
    if (!tree[n].lazy)
    {
        tree[tree[n].left].leftVacant = tree[tree[n].left].maxVacant = tree[tree[n].left].rightVacant = tree[tree[n].right].leftVacant =
            tree[tree[n].right].rightVacant = tree[tree[n].right].maxVacant = 0;
        tree[tree[n].left].lazy = tree[tree[n].right].lazy = 0;
    }
}

int  buildCnt = 1;
void build(int n, int left, int right)
{
    if (left == right)
    {
        tree[n].leftVacant = tree[n].maxVacant = tree[n].rightVacant = 1;
        return;
    }
    int mid       = (left + right) >> 1;
    tree[n].left  = ++buildCnt;
    tree[n].right = ++buildCnt;
    build(tree[n].left, left, mid);
    build(tree[n].right, mid + 1, right);
    pushup(n, left, right);
}

void change(int n, int left, int right, int changLeft, int changeRight, bool op)
{
    pushdown(n, left, right);
    if (changLeft <= left && right <= changeRight)
    {
        tree[n].lazy       = op;
        tree[n].leftVacant = tree[n].maxVacant = tree[n].rightVacant = op ? (right - left + 1) : 0;
        return;
    }
    if (left > changeRight || right < changLeft)
        return;
    int mid = (left + right) >> 1;
    change(tree[n].left, left, mid, changLeft, changeRight, op);
    change(tree[n].right, mid + 1, right, changLeft, changeRight, op);
    pushup(n, left, right);
}

int query(int n, int left, int right, int len)
{
    pushdown(n, left, right);
    if (tree[n].maxVacant < len)
        return -1;
    if (tree[n].leftVacant >= len)
        return left;
    int mid       = (left + right) >> 1;
    int leftQuery = query(tree[n].left, left, mid, len);
    if (leftQuery != -1)
        return leftQuery;
    if (len <= tree[tree[n].left].rightVacant + tree[tree[n].right].leftVacant)
        return (mid - tree[tree[n].left].rightVacant + 1);
    return query(tree[n].right, mid + 1, right, len);
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cin >> N >> M;
    build(1, 1, N);
    int op, X, D;
    while (M--)
    {
        std::cin >> op;
        switch (op)
        {
        case 1:
        {
            std::cin >> D;
            int res = query(1, 1, N, D);
            if (res != -1)
            {
                std::cout << res << '\n';
                change(1, 1, N, res, res + D - 1, 0);
            }
            else
                std::cout << "0\n";
            break;
        }
        case 2:
            std::cin >> X >> D;
            change(1, 1, N, X, X + D - 1, 1);
            break;
        }
    }
    return 0;
}