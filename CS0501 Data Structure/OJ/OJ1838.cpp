#include <iostream>
using std::cin;
using std::string;

struct node
{
    char letter;
    int  next[26];
    bool end;
    int  endsum;
    node() : next{0}, letter('\0'), end(false), endsum(0) {}
};

constexpr int maxLen = 105, maxN = 1e4 + 5;

node trie[maxLen * maxN];
int  letterCnt = 1;
int  n, m;

void input()
{
    scanf("%d%d", &n, &m);
    while (n--)
    {
        string in;
        int    current = 0;
        cin >> in;
        for (int j = in.length() - 1; j >= 0; --j)
        {
            if (trie[current].next[in[j] - 'a'] == 0)
            {
                trie[letterCnt].letter          = in[j];
                trie[current].next[in[j] - 'a'] = letterCnt;
                ++letterCnt;
            }
            current = trie[current].next[in[j] - 'a'];
            ++trie[current].endsum;
        }
        trie[current].end = true;
    }
}

void find(string str, int start, int current)
{
    for (int i = start; i >= 0; --i)
        if (trie[current].next[str[i] - 'a'] != 0)
            current = trie[current].next[str[i] - 'a'];
        else
        {
            printf("%d\n", 0);
            return;
        }
    printf("%d\n", trie[current].endsum);
}

void solve()
{
    string post;
    while (m--)
    {
        cin >> post;
        find(post, post.length() - 1, 0);
    }
}

int main()
{
    input();
    solve();
    return 0;
}