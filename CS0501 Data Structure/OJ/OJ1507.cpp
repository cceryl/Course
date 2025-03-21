#include <iostream>

using std::cin, std::cout, std::endl, std::string;

const int maxlen = 1e6 + 5;
int       next[maxlen];

void getNext(string target, int *next)
{
    next[0]           = -1;
    unsigned int pos  = 0;
    int          last = -1;
    while (pos < target.length())
    {
        if (last == -1 || target[pos] == target[last])
            next[++pos] = ++last;
        else
            last = next[last];
    }
}

void KMP(string source, string target, int *next, int start)
{
    unsigned int src = start;
    int          tgt = 0;
    while (src < source.length())
    {
        if (source[src] == target[tgt] || tgt == -1)
        {
            ++src, ++tgt;
            if (tgt == (int)target.length())
            {
                cout << src - tgt + 1 << endl;
                KMP(source, target, next, src - tgt + 1);
                return;
            }
        }
        else
            tgt = next[tgt];
    }
}

int main()
{
    string source, target;
    cin >> source >> target;
    getNext(target, next);
    KMP(source, target, next, 0);
    for (unsigned int i = 1; i <= target.length(); ++i)
        cout << next[i] << ' ';
    return 0;
}