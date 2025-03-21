#include <cstring>
#include <iostream>

using namespace std;

int main()
{
    int n; // num of commands
    cin >> n;
    char input[1005];
    char current[10005] = "/";
    int  p              = 1;
    cin.getline(input, 1005); // garbage
    while (n--)
    {
        cin.getline(input, 1005);
        int len = strlen(input);
        int i   = 0;
        // head
        switch (input[0])
        {
        case '/': // definite path
            i = 1;
            p = 1;
            strcpy(current, "/");
            break;
        case '.':
            switch (input[1])
            {
            case '/': // current path
                i = 2;
                break;
            case '.': // prior path
                i = 3;
                --p;
                while (current[p - 1] != '/')
                    --p;
                current[p] = '\0';
                break;
            }
            break;
        }
        while (i < len)
        {
            switch (input[i])
            {
            case '.':
                switch (input[i + 1])
                {
                case '/': i += 2; break;
                case '.':
                    i += 3;
                    --p;
                    while (current[p - 1] != '/')
                        --p;
                    current[p] = '\0';
                    break;
                }
                break;
            default:
                int start = i;
                while (input[i] != '/')
                    ++i;
                ++i;
                strncpy(current + p, input + start, i - start);
                p          += i - start;
                current[p]  = '\0';
            }
        }
end:
        cout << current << endl;
    }
}