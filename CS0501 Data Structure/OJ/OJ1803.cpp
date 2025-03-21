#include <iostream>

using std::cin, std::cout, std::endl, std::string;

const int maxlen = 1e6 + 5;
int       next[maxlen];

void getNext(string &str)
{
    next[0]           = -1;
    unsigned int pos  = 0;
    int          last = -1;
    while (pos < str.length())
    {
        if (last == -1 || str[pos] == str[last])
            next[++pos] = ++last;
        else
            last = next[last];
    }
}

int main()
{
    int n; // num of str
    cin >> n;
    string str;
    while (n--)
    {
        cin >> str;
        getNext(str);
        long long sumlen = 0;
        for (unsigned int i = 1; i <= str.length(); ++i)
        {
            int minlen = next[i];
            if (minlen == 0)
                continue;
            while (next[minlen] != 0)
                minlen = next[minlen];
            next[i]  = minlen;
            sumlen  += i - minlen;
        }
        cout << sumlen << endl;
    }
    return 0;
}