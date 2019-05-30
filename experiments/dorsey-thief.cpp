// Simon St James (ssjgz) - 2019-05-30
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

#include <sys/time.h>

using namespace std;

void solveBruteForce(int passengerIndex, int remainingGold, int64_t profitSoFar, const vector<int>& v, const vector<int>& a, int64_t& bestProfit)
{
    //cout << "passengerIndex: " << passengerIndex << " remainingGold: " << remainingGold << " profitSoFar: " << profitSoFar << endl;
    if (remainingGold < 0) 
        return;
    if (remainingGold == 0)
    {
        if (profitSoFar > bestProfit)
        {
            //cout << "Found new best profit: " << profitSoFar << endl;
            bestProfit = profitSoFar;
        }
        return;
    }

    const int N = v.size();
    if (passengerIndex == N)
    {
        // No more passengers left.
        return;
    }
    // Don't buy from this passenger.
    solveBruteForce(passengerIndex + 1, remainingGold, profitSoFar, v, a, bestProfit);
    // Buy from this passenger.
    solveBruteForce(passengerIndex + 1, remainingGold - a[passengerIndex], profitSoFar + v[passengerIndex], v, a, bestProfit);
}

int64_t solveBruteForce(int N, int X, const vector<int>& v, const vector<int>& a)
{
    int64_t bestProfit = -1;
    solveBruteForce(0, X, 0, v, a, bestProfit);

    return bestProfit;
}

int64_t solveOptimised(int N, int X, const vector<int>& v, const vector<int>& a)
{
    vector<vector<int>> buyersOfGoldDecreasingPrice(X + 1);
    vector<int> distinctGoldAmounts;
    for (int i = 0; i < N; i++)
    {
        if (a[i] > X)
            continue;
        distinctGoldAmounts.push_back(a[i]);
        buyersOfGoldDecreasingPrice[a[i]].push_back(v[i]);
    }
    sort(distinctGoldAmounts.begin(), distinctGoldAmounts.end());
    distinctGoldAmounts.erase(std::unique(distinctGoldAmounts.begin(), distinctGoldAmounts.end()), distinctGoldAmounts.end());
    for (auto& prices : buyersOfGoldDecreasingPrice)
    {
        sort(prices.begin(), prices.end(), std::greater<>());
    }
    vector<int64_t> bestPriceForSoldGold(X + 1, -1);
    bestPriceForSoldGold[0] = 0;

    for (const auto goldToSell : distinctGoldAmounts)
    {
        vector<int64_t> nextBestPriceForSoldGold = bestPriceForSoldGold;
        const int totalBuyersForGold = buyersOfGoldDecreasingPrice[goldToSell].size();
        //cout << "goldToSell: " << goldToSell << " totalBuyersForGold: " << totalBuyersForGold << endl;
        for (int existingGold = 0; existingGold <= X; existingGold++)
        {
            if (bestPriceForSoldGold[existingGold] == -1)
                continue;
            //cout << " existingGold: " << existingGold << " bestPriceForSoldGold: " << bestPriceForSoldGold[existingGold] << endl;
            int64_t priceOfSoldGold = 0;
            int64_t amountOfSoldGold = 0;
            for (int numBuyers = 1; numBuyers <= totalBuyersForGold; numBuyers++)
            {
                priceOfSoldGold += buyersOfGoldDecreasingPrice[goldToSell][numBuyers - 1];
                amountOfSoldGold += goldToSell;
                //cout << " numBuyers: " << numBuyers << " priceOfSoldGold: " << priceOfSoldGold << " amountOfSoldGold: " << amountOfSoldGold << endl;
                if (existingGold + amountOfSoldGold > X)
                    break;
                nextBestPriceForSoldGold[existingGold + amountOfSoldGold] = max(nextBestPriceForSoldGold[existingGold + amountOfSoldGold], bestPriceForSoldGold[existingGold] + priceOfSoldGold);
            }
        }
        bestPriceForSoldGold = nextBestPriceForSoldGold;
    }

    return bestPriceForSoldGold[X];
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        //const int N = rand() % 1'000'000 + 1;
        //const int X = (rand() % 5000) + 1;
        const int N = 1'000'000;
        const int X = 5'000;
        const int maxA = rand() % 1000;
        const int maxV = rand() % 1000;

        cout << N << " " << X << endl;

        for (int i = 0; i < N / 2; i++)
        {
            cout << ((rand() % maxA) + 1) << " " << ((rand() % maxV) + 1) << endl;
        }
        for (int i = 0; i < N / 2; i++)
        {
            cout << i << " " << ((rand() % maxV) + 1) << endl;
        }
        return 0;
    }
    int N, X;
    cin >> N >> X;

    vector<int> v(N);
    vector<int> a(N);

    for (int i = 0; i < N; i++)
    {
        cin >> v[i];
        cin >> a[i];
    }

#ifdef BRUTE_FORCE
    const auto resultBruteForce = solveBruteForce(N, X, v, a);
    cout << "resultBruteForce: " << resultBruteForce << endl;

    const auto resultOptimised = solveOptimised(N, X, v, a);
    cout << "resultOptimised: " << resultOptimised << endl;
    
    assert(resultOptimised == resultBruteForce);
#else
    const auto resultOptimised = solveOptimised(N, X, v, a);
    if (resultOptimised == -1)
        cout << "Got caught!";
    else
        cout << resultOptimised;
    cout << endl;
#endif
}
