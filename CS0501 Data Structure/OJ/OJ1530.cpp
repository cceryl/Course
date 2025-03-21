#include <iostream>

typedef unsigned long long ull;

using std::cin, std::cout, std::endl, std::string;

const int maxW = 1005;
const int maxP = 1e5 + 5;
const ull base = 233;
// mod = MAX_ULL
ull       hashW[maxW];             // words hash
ull       hashP[maxP];             // passage hash
bool      exist[maxW] = {0};       // words mentioned in passage
int       wordID[maxP];            // id of passage word
bool      windowExist[maxW] = {0}; // window has the word

inline ull hashstr(string str)
{
    ull len  = str.length();
    ull hash = 0;
    for (ull i = 0; i < len; ++i)
        hash = hash * base + str[i];
    return hash;
}

int main()
{
    int    n; // num of words
    int    m; // len of passage
    int    wordExist = 0;
    string str;
    cin >> n;
    for (int i = 0; i < n; ++i)
    {
        cin >> str;
        hashW[i] = hashstr(str);
    }
    cin >> m;
    for (int i = 0; i < m; ++i)
    {
        cin >> str;
        hashP[i] = hashstr(str);
        for (int j = 0; j < n; ++j)
        {
            if (hashP[i] == hashW[j])
            {
                if (exist[j] == 0)
                {
                    exist[j] = 1;
                    ++wordExist;
                }
                wordID[i] = j;
                break;
            }
            wordID[i] = -1;
        }
    }
    int minLen = maxP, wordCnt = 0;
    for (int left = 0, right = 0; right < m; ++right)
    {
        if (wordID[right] != -1) // right is valid word
            if (windowExist[wordID[right]] == 0)
            {
                windowExist[wordID[right]] = 1;
                ++wordCnt;
            }
checkrep:                                       // check repetition and modify left
        while (wordID[left] == -1)              // left is not valid word
            ++left;
        for (int i = left + 1; i <= right; ++i) // left is unnecessary
        {
            if (wordID[left] == wordID[i])
            {
                ++left;
                goto checkrep;
            }
        }
        if (wordCnt == wordExist && right - left + 1 < minLen)
            minLen = right - left + 1;
    }
    cout << wordExist << endl << minLen;
    return 0;
}