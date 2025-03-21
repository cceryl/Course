// Wasted

#include <iostream>

using std::cin, std::cout, std::max;

const int       maxTime     = 2005;
const int       maxmaxStock = 2005;
const long long minlonglong = -((long long)1 << 32);

long long money[maxTime /*days*/][maxmaxStock /*stocks*/];

struct stockInfo
{
    int buyPrice;
    int sellPrice;
    int buyLimit;
    int sellLimit;
};

stockInfo stockMarket[maxTime];

int main()
{
    int time, maxStock, tradeGap;
    cin >> time >> maxStock >> tradeGap;
    for (int i = 1; i <= time; ++i)
        cin >> stockMarket[i].buyPrice >> stockMarket[i].sellPrice >> stockMarket[i].buyLimit >> stockMarket[i].sellLimit;
    for (int i = 0; i <= time; ++i)
        for (int j = 0; j <= maxStock; ++j)
            money[i][j] = minlonglong;
    // T <= tradeGap + 1
    for (int T = 1; T <= 1 + tradeGap; ++T)
    {
        for (int S = 0; S <= stockMarket[T].buyLimit; ++S)
            money[T][S] = max(money[T - 1][S] /*no deal*/, (long long)-stockMarket[T].buyPrice * S /*buy stock*/);
        for (int S = stockMarket[T].buyLimit + 1; S <= maxStock; ++S)
            money[T][S] = money[T - 1][S];
    }
    // T > tradeGap + 1
    for (int T = tradeGap + 2; T <= time; ++T)
    {
        stockInfo *today         = &stockMarket[T];
        int        lastTradeTime = T - tradeGap - 1;
        int        queue[maxStock], head, tail;
        // no deal
        for (int S = 0; S <= maxStock; ++S)
            money[T][S] = money[T - 1][S];
        // buy stock
        queue[0] = 0, head = 0, tail = 1;
        for (int S = 1; S <= maxStock; ++S)
        {
            while (head < tail && queue[head] + today->buyLimit < S)
                ++head;
            while (head < tail
                   && money[lastTradeTime][queue[tail - 1]] - today->buyPrice * (S - queue[tail - 1]) <= money[lastTradeTime][S])
                --tail;
            queue[tail++] = S;
            money[T][S]   = max(money[T][S], money[lastTradeTime][queue[head]] - today->buyPrice * (S - queue[head]));
        }
        // sell stock
        queue[0] = maxStock, head = 0, tail = 1;
        for (int S = maxStock - 1; S >= 0; --S)
        {
            while (head < tail && queue[head] - today->sellLimit > S)
                ++head;
            while (head < tail
                   && money[lastTradeTime][queue[tail - 1]] + today->sellPrice * (queue[tail - 1] - S) <= money[lastTradeTime][S])
                --tail;
            queue[tail++] = S;
            money[T][S]   = max(money[T][S], money[lastTradeTime][queue[head]] + today->sellPrice * (queue[head] - S));
        }
    }
    cout << money[time][0];
    return 0;
}