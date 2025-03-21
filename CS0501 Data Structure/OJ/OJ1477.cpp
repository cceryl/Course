#include <cstring>
#include <iostream>

#define maxlen 1000005

using std::cin, std::cout, std::endl, std::string;

int next[maxlen];

void getNext(string &target, int len)
{
    next[0]  = -1;
    int i    = 0;
    int last = -1;
    while (i < len)
    {
        if (last == -1 || target[i] == target[last])
            next[++i] = ++last;
        else
            last = next[last];
    }
}

void output(int len)
{
    if (len == 0 || len == -1)
        return;
    output(next[len]);
    cout << len << endl;
}

int main()
{
    string tgt;
    cin >> tgt;
    int len = tgt.size();
    getNext(tgt, len);
    output(len);
}