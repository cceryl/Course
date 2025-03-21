#include <iostream>

using namespace std;

int main()
{
    int n, cnt = 0;
    cin >> n;
    cin.get();
    char strs[n][101];
    for (int i = 0; i < n; i++)
    {
        cin.getline(strs[i], 101);
        bool eq = 0;
        for (int j = 0; j < i; j++)
        {
            int le[26];
            for (int k = 0; k < 26; k++)
                le[k] = 0;
            for (int k = 0; strs[i][k] != '\0'; k++)
                le[strs[i][k] - 'a']++;
            for (int k = 0; strs[j][k] != '\0'; k++)
                le[strs[j][k] - 'a']--;
            bool f = 1;
            for (int k = 0; k < 26; k++)
            {
                if (le[k])
                {
                    f = 0;
                    break;
                }
            }
            if (f)
            {
                eq = 1;
                break;
            }
        }
        if (eq == false)
            cnt++;
    }
    cout << cnt;
}