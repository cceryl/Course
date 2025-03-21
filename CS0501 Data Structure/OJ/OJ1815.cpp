#include <iostream>

using std::cin, std::cout;

struct missile
{
    int         x, y;
    friend bool operator<(const missile &lhs, const missile &rhs) { return lhs.x < rhs.x; }
} missiles[100005], tmp[100005];

void sort(int left, int right)
{
    if (left == right)
        return;
    int mid = (left + right) / 2;
    sort(left, mid), sort(mid + 1, right);
    int p = left, q = mid + 1, top = left - 1;
    while (p <= mid && q <= right)
    {
        if (missiles[p] < missiles[q])
            tmp[++top] = missiles[p++];
        else
            tmp[++top] = missiles[q++];
    }
    while (p <= mid)
        tmp[++top] = missiles[p++];
    while (q <= right)
        tmp[++top] = missiles[q++];
    for (int i = left; i <= right; ++i)
        missiles[i] = tmp[i];
}

int  n, d;
int  queMin[100005], headMin, rearMin;
int  queMax[100005], headMax, rearMax;
bool exist(int len)
{
    headMin = headMax = 1, rearMin = rearMax = 0;
    for (int i = 1; i <= n; ++i)
    {
        while (headMin <= rearMin && missiles[queMin[rearMin]].y >= missiles[i].y)
            --rearMin;
        queMin[++rearMin] = i;
        while (headMin <= rearMin && missiles[queMin[headMin]].x < missiles[i].x - len)
            ++headMin;
        while (headMax <= rearMax && missiles[queMax[rearMax]].y <= missiles[i].y)
            --rearMax;
        queMax[++rearMax] = i;
        while (headMax <= rearMax && missiles[queMax[headMax]].x < missiles[i].x - len)
            ++headMax;
        if (missiles[queMax[headMax]].y - missiles[queMin[headMin]].y >= d)
            return true;
    }
    return false;
}
int main()
{
    cin >> n >> d;
    for (int i = 1; i <= n; ++i)
        cin >> missiles[i].x >> missiles[i].y;
    sort(1, n);
    int ans    = -1;
    int lenMin = 1, lenMax = 1e8 + 5;
    while (lenMin <= lenMax)
    {
        int mid = (lenMin + lenMax) / 2;
        if (exist(mid))
        {
            lenMax = mid - 1;
            ans    = mid;
        }
        else
            lenMin = mid + 1;
    }
    cout << ans;
    return 0;
}