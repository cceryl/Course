#include <stdio.h>

inline int read()
{
    int  x  = 0;
    char ch = getchar();
    while (ch < '0' || ch > '9')
        ch = getchar();
    while (ch >= '0' && ch <= '9')
        x = x * 10 + ch - '0', ch = getchar();
    return x;
}

class trie
{
    struct node
    {
        bool  key;
        int   val;
        node *left, *right;
        node(bool k = 0, int v = 0) : key(k), val(v), left(nullptr), right(nullptr) {}
        node(const node &other) = delete;
    };

private:
    node root;
    void remove(node *r)
    {
        if (r == nullptr)
            return;
        remove(r->left), remove(r->right);
        delete r;
    }

public:
    ~trie() { remove(root.left), remove(root.right); }
    void insert(int num)
    {
        node *r = &root;
        int   i = 1 << 30;
        while (i > 0)
        {
            if (num & i)
            {
                if (r->right == nullptr)
                    r->right = new node(1, r->val + i);
                r = r->right;
            }
            else
            {
                if (r->left == nullptr)
                    r->left = new node(0, r->val);
                r = r->left;
            }
            i = i >> 1;
        }
    }
    int search(int num)
    {
        node *r = &root;
        int   i = 1 << 30;
        while (i > 0)
        {
            if (num & i)
            {
                if (r->left == nullptr)
                    r = r->right;
                else
                    r = r->left;
            }
            else
            {
                if (r->right == nullptr)
                    r = r->left;
                else
                    r = r->right;
            }
            i = i >> 1;
        }
        return r->val ^ num;
    }
};

trie tree;
int  n, m;

void input()
{
    scanf("%d%d", &n, &m);
    for (int i = 0; i < n; ++i)
        tree.insert(read());
}

void solve()
{
    int xorVal;
    for (int i = 0; i < m; ++i)
        xorVal = read(), printf("%d\n", tree.search(xorVal));
}

int main()
{
    input();
    solve();
    return 0;
}