#include <iostream>

using std::cin, std::cout;

#define MAXLEN 100000

struct neighbor
{
    int  prev  = -1;
    int  next  = -1;
    bool exist = 1;
};

int main()
{
    neighbor array[MAXLEN];
    int      begin = 0;
    int      n;
    cin >> n; // num of student
    int k, p;
    for (int i = 1; i < n; i++)
    {
        cin >> k >> p;
        --k;
        if (p) // right
        {
            if (array[k].next == -1)
            {
                array[k].next = i;
                array[i].prev = k;
            }
            else
            {
                array[i].next             = array[k].next;
                array[array[i].next].prev = i;
                array[k].next             = i;
                array[i].prev             = k;
            }
        }
        else // left
        {
            if (k == begin)
                begin = i;
            if (array[k].prev == -1)
            {
                array[k].prev = i;
                array[i].next = k;
            }
            else
            {
                array[i].prev             = array[k].prev;
                array[array[i].prev].next = i;
                array[k].prev             = i;
                array[i].next             = k;
            }
        }
    }
    int m; // num of delete
    cin >> m;
    while (m--)
    {
        cin >> k;
        array[k - 1].exist = 0;
    }
    for (int i = begin; i != -1; i = array[i].next)
        if (array[i].exist)
            cout << i + 1 << ' ';
}