// Simon St James (ssjgz) - 2020-02-27
// 
// Solution to: https://www.codechef.com/problems/COINS
//
#include <iostream>
#include <map>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int64_t calcMaxDollarsForCoin(int64_t numCoins, map<int64_t, int64_t>& maxDollarsForCoinLookup)
{
    if (numCoins == 0)
        return 0;
    if (maxDollarsForCoinLookup[numCoins] != 0)
        return maxDollarsForCoinLookup[numCoins];

    const int64_t dollarsStraightConversion = numCoins; 
    const int64_t dollarsExchange = calcMaxDollarsForCoin(numCoins / 2, maxDollarsForCoinLookup) +
                                    calcMaxDollarsForCoin(numCoins / 3, maxDollarsForCoinLookup) +
                                    calcMaxDollarsForCoin(numCoins / 4, maxDollarsForCoinLookup);

    const int64_t maxDollars = max(dollarsStraightConversion, dollarsExchange);

    maxDollarsForCoinLookup[numCoins] = maxDollars;

    return maxDollars;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    map<int64_t, int64_t> maxDollarsForCoinLookup;

    int64_t numCoins;
    while (cin >> numCoins)
    {
        cout << calcMaxDollarsForCoin(numCoins, maxDollarsForCoinLookup) << endl;
    }
}
