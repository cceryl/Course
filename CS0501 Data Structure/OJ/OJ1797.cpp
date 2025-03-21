#include <iostream>

using std::cin, std::cout, std::endl;

struct tree
{
    int  coordinate;
    int  height;
    int  sick = 0;
    bool operator<(tree &obj) { return coordinate < obj.coordinate; }
};

const int maxN = 5e4 + 5;
tree      forest[maxN];
// store tree index
int       queue[maxN];

void sort(tree *first, tree *last)
{
    if (first >= last - 1)
        return;
    tree *F = first, *L = last;
    tree  pivot = *first;
    while (first != last - 1)
    {
        while (first != last - 1)
        {
            if (*(last - 1) < pivot)
            {
                *first++ = *(last - 1);
                break;
            }
            else
                last--;
        }
        while (first != last - 1)
        {
            if (!(*first < pivot))
            {
                *(last-- - 1) = *first;
                break;
            }
            else
                first++;
        }
    }
    *first = pivot;
    sort(F, first);
    sort(last, L);
}

int main()
{
    int N, D;
    cin >> N >> D;
    for (int i = 0; i < N; ++i)
        cin >> forest[i].coordinate >> forest[i].height;
    sort(forest, forest + N);
    int left, right, head, tail, sick = 0;
    // forward scan
    queue[0] = 0;
    right = 1, head = 0, tail = 1;
    while (right < N)
    {
        while (head < tail && forest[right].coordinate - forest[queue[head]].coordinate > D)
            ++head;
        while (head < tail && forest[queue[tail - 1]].height <= forest[right].height)
            --tail;
        queue[tail++] = right;
        if (forest[queue[head]].height >= 2 * forest[right].height)
            ++forest[right].sick;
        ++right;
    }
    // backward scan
    queue[0] = N - 1;
    left = N - 2, head = 0, tail = 1;
    while (left >= 0)
    {
        while (head < tail && forest[queue[head]].coordinate - forest[left].coordinate > D)
            ++head;
        while (head < tail && forest[queue[tail - 1]].height <= forest[left].height)
            --tail;
        queue[tail++] = left;
        if (forest[queue[head]].height >= 2 * forest[left].height)
            ++forest[left].sick;
        if (forest[left].sick == 2)
            ++sick;
        --left;
    }
    cout << sick << endl;
    for (int i = 0; i < N; ++i)
        if (forest[i].sick == 2)
            cout << forest[i].coordinate << ' ';
    return 0;
}