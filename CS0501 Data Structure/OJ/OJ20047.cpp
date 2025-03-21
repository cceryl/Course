#include <cstdio>

using namespace std;

constexpr int maxstrlen = 1e6 + 5;

struct node
{
    bool  end;
    node *next[26], *fail;
};

node  trie[maxstrlen];
node *queue[maxstrlen];
node *root = trie;

void insert(const char *str)
{
    static int cnt     = 1;
    node      *current = root;
    while (*str)
    {
        int ch = *str - 'a';
        if (current->next[ch] == nullptr)
            current->next[ch] = trie + cnt, ++cnt;
        current = current->next[ch];
        ++str;
    }
    current->end = true;
}

void Trie()
{
    int queHead = 0, queTail = -1;
    for (int i = 0; i < 26; ++i)
    {
        if (root->next[i])
        {
            root->next[i]->fail = root;
            queue[++queTail]    = root->next[i];
        }
        else
            root->next[i] = root;
    }
    while (queHead <= queTail)
    {
        node *current = queue[queHead++];
        for (int i = 0; i < 26; ++i)
        {
            if (current->next[i] != nullptr)
            {
                current->next[i]->fail = current->fail->next[i];
                queue[++queTail]       = current->next[i];
            }
            else
                current->next[i] = current->fail->next[i];
        }
    }
}

bool query(const char *str)
{
    node *current = root;
    if (current->end)
        return true;
    while (*str)
    {
        int ch  = *str - 'a';
        current = current->next[ch];
        if (current->end)
            return true;
        ++str;
    }
    return false;
}

char str[maxstrlen];

int main()
{
    int n;
    scanf("%d", &n);
    while (n--)
    {
        scanf("%s", str);
        insert(str);
    }
    Trie();
    scanf("%s", str);
    printf(query(str) ? "valar morghulis" : "valar dohaeris");
}