#include <iostream>

using std::cin, std::cout;

typedef unsigned long long ull;
const int                  maxstr = 1e4 + 5;
const ull                  base   = 233;
// mod = MAXULL

ull hash[maxstr];
ull lenHash[maxstr]; // hash of base ^ len

int main()
{
    int n; // num of string
    cin >> n;
    cin.get();
    for (int i = 0; i < n; ++i)
        lenHash[i] = 1;
    for (int s = 0; s < n; ++s)
    {
        ull chash = 0; // current hash
        while (true)
        {
            char c = cin.get();
            switch (c)
            {
            case '\n': goto end;
            case '{':
                int id;
                cin >> id;
                cin.get();
                chash       = chash * lenHash[id] + hash[id];
                lenHash[s] *= lenHash[id];
                break;
            default:
                chash       = chash * base + c;
                lenHash[s] *= base;
                break;
            }
        }
end:
        hash[s] = chash;
    }
    int unique = 1;
    for (int i = 1; i < n; i++)
        if (hash[i] != hash[i - 1])
            ++unique;
    cout << unique;
}