// Simon St James (ssjgz) - 2019-05-30
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int64_t findBestPriceForSellingAllGold(int N, int X, const vector<int>& v, const vector<int>& a)
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

    for (const auto amountOfGoldToSell : distinctGoldAmounts)
    {
        vector<int64_t> bestPriceForNextSoldGold = bestPriceForSoldGold; // Implicitly accounts for the numBuyers == 0 case.
        const int totalBuyersForGoldAmount = buyersOfGoldDecreasingPrice[amountOfGoldToSell].size();
        for (int existingGold = 0; existingGold <= X; existingGold++)
        {
            if (bestPriceForSoldGold[existingGold] == -1)
                continue;
            int64_t priceOfSoldGoldAmount = 0;
            int64_t amountOfSoldGoldAmount = 0;
            for (int numBuyers = 1; numBuyers <= totalBuyersForGoldAmount; numBuyers++)
            {
                priceOfSoldGoldAmount += buyersOfGoldDecreasingPrice[amountOfGoldToSell][numBuyers - 1];
                amountOfSoldGoldAmount += amountOfGoldToSell;
                if (existingGold + amountOfSoldGoldAmount > X)
                {
                    // Can't sell more gold that we started with!
                    break;
                }
                bestPriceForNextSoldGold[existingGold + amountOfSoldGoldAmount] = max(bestPriceForNextSoldGold[existingGold + amountOfSoldGoldAmount], 
                                                                                      bestPriceForSoldGold[existingGold] + priceOfSoldGoldAmount);
            }
        }
        bestPriceForSoldGold = bestPriceForNextSoldGold;
    }

    return bestPriceForSoldGold[X];
}

int main()
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

    const auto result = findBestPriceForSellingAllGold(N, X, v, a);
    if (result == -1)
        cout << "Got caught!";
    else
        cout << result;
    cout << endl;
}
