// Simon St James (ssjgz) - 2020-02-27
// 
// Solution to: https://www.codechef.com/problems/COINS
//
#include <iostream>
#include <map>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        const int T = rand() % 10 + 1;
        //const int T = 1;

        for (int t = 0; t < T; t++)
        {
            const int N = 1 + rand() % 1'000'000'000;
            cout << N << endl;
        }

        return 0;
    }

    ios::sync_with_stdio(false);

    map<int64_t, int64_t> maxDollarsForCoinLookup;

    for (int64_t numCoins = 1; numCoins < 1'000'000; numCoins++)
    {
        cout << "Q: 1 lines" << endl;
        cout << numCoins << endl;
        cout << "A: 1 lines" << endl;
        cout << calcMaxDollarsForCoin(numCoins, maxDollarsForCoinLookup) << endl;
    }
}
