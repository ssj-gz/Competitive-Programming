#include <iostream>
#include <vector>
#include <algorithm>

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
    vector<int64_t> bestPriceForSoldGold(X + 1, 0);

    for (const auto goldToSell : distinctGoldAmounts)
    {
        vector<int64_t> nextBestPriceForSoldGold = bestPriceForSoldGold;
        const int totalBuyersForGold = buyersOfGoldDecreasingPrice[goldToSell].size();
        for (int existingGold = 0; existingGold <= X; existingGold++)
        {
            int64_t priceOfSoldGold = 0;
            int64_t amountOfSoldGold = 0;
            for (int numBuyers = 1; numBuyers <= totalBuyersForGold; numBuyers++)
            {
                priceOfSoldGold += buyersOfGoldDecreasingPrice[goldToSell][numBuyers - 1];
                amountOfSoldGold += goldToSell;
                if (existingGold - amountOfSoldGold < 0)
                    break;
                nextBestPriceForSoldGold[existingGold - amountOfSoldGold] = max(nextBestPriceForSoldGold[existingGold - amountOfSoldGold], bestPriceForSoldGold[existingGold] + priceOfSoldGold);
            }
        }
        bestPriceForSoldGold = nextBestPriceForSoldGold;
    }

    return bestPriceForSoldGold[X];
}

int main(int argc, char* argv[])
{
    int N, X;
    cin >> N >> X;

    vector<int> v(N);
    vector<int> a(N);

    for (int i = 0; i < N; i++)
    {
        cin >> v[i];
        cin >> a[i];
    }

    const auto resultBruteForce = solveBruteForce(N, X, v, a);
    cout << "resultBruteForce: " << resultBruteForce << endl;

    const auto resultOptimised = solveOptimised(N, X, v, a);
    cout << "resultOptimised: " << resultOptimised << endl;
}
