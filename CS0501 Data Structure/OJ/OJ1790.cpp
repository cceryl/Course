#include <iostream>
#include <unordered_set>

using std::cin, std::cout;

const unsigned long long maxn = 1e6 + 5;
const unsigned long long base = 37;
// mod MAXLONGLONG

unsigned long long quickpow(unsigned long long i)
{
    if (i == 0)
        return 1;
    unsigned long long res = 1;
    if (i % 2)
        res = res * base;
    res = res * quickpow(i / 2) * quickpow(i / 2);
    return res;
}

int main()
{
    unsigned long long n, m;
    cin >> n >> m;
    cin.get();
    char str[maxn];
    cin.getline(str, maxn);
    unsigned long long hash = 0, mbase;
    mbase                   = quickpow(m - 1);
    std::unordered_set<unsigned long long> hashtable;
    hashtable.reserve(n - m);
    // first hash
    for (unsigned long long i = 0; i < m; ++i)
        hash = hash * base + str[i];
    hashtable.insert(hash);
    for (unsigned long long i = 0; i < n - m; ++i)
    {
        hash = hash - str[i] * mbase;
        hash = hash * base + str[i + m];
        hashtable.insert(hash);
    }
    cout << hashtable.size();
    return 0;
}