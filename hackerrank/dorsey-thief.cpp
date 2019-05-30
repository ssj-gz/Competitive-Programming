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
        for (int allGoldSoldPreviously = 0; allGoldSoldPreviously <= X; allGoldSoldPreviously++)
        {
            if (bestPriceForSoldGold[allGoldSoldPreviously] == -1)
                continue;
            int64_t priceOfSoldGoldAmount = 0;
            int64_t amountOfSoldGoldAmount = 0;
            for (int numBuyers = 1; numBuyers <= totalBuyersForGoldAmount; numBuyers++)
            {
                priceOfSoldGoldAmount += buyersOfGoldDecreasingPrice[amountOfGoldToSell][numBuyers - 1];
                amountOfSoldGoldAmount += amountOfGoldToSell;
                if (allGoldSoldPreviously + amountOfSoldGoldAmount > X)
                {
                    // Can't sell more gold that we started with!
                    break;
                }
                bestPriceForNextSoldGold[allGoldSoldPreviously + amountOfSoldGoldAmount] 
                    = max(bestPriceForNextSoldGold[allGoldSoldPreviously + amountOfSoldGoldAmount], 
                          bestPriceForSoldGold[allGoldSoldPreviously] + priceOfSoldGoldAmount);
            }
        }
        bestPriceForSoldGold = bestPriceForNextSoldGold;
    }

    return bestPriceForSoldGold[X];
}

int main()
{
    // A nice, easy 85 points :)
    //
    // So: first of all, note that we need to sell *exactly* X units of gold, so all
    // passengers that will only buy > X units of gold can be discarded.
    // This means that we can group all remaining passengers by the number of 
    // units of gold they buy, and that there will be at most X + 1 such groups, and
    // we can easily determine how many passengers are in a given group.  Let distinctGoldAmounts
    // be the amount of gold in each of these groups.
    //
    // Let bestPriceForSoldGold[n][g] be the best price found for selling exactly g
    // units of gold after dealing with (that is: deciding whether to sell to, or not)
    // all passengers in the first n gold-amount-groups.
    //
    // Then it's hopefully obvious that bestPriceForSoldGold[n+1][g'] can be found by iterating 
    // over all total amounts of gold we could have sold so far and then, for each possible subset of buyers of distinctGoldAmounts[n+1],
    // updating bestPriceForSoldGold[n+1] in the obvious way i.e.
    //
    //    for allGoldSoldPreviously = 0, 1, 2, ... , X:
    //       for buyers is a subset of passengers buying distinctGoldAmounts[n+1]:
    //          // i.e. amount of gold and price obtained by selling distinctGoldAmounts[n+1] units of gold
    //          // to each passenger in this subset.
    //          priceOfSoldGoldAmount = 0
    //          amountOfSoldGoldAmount = 0
    //          for passenger in buyers:
    //             amountOfSoldGoldAmount += distinctGoldAmounts[n+1]
    //             priceOfSoldGoldAmount += v[buyer]
    //          bestPriceForSoldGold[n+1][allGoldSoldPreviously + amountOfSoldGoldAmount] 
    //            = max(bestPriceForSoldGold[n+1][allGoldSoldPreviously + amountOfSoldGoldAmount],
    //                  bestPriceForSoldGold[n][allGoldSoldPreviously] + priceOfSoldGoldAmount);
    // 
    // (while adding some breaks in so that we don't compute bestPriceForSoldGold[n+1][g] for g > X!)
    //
    // But not all subsets of buyers of distinctGoldAmounts[n+1] need be considered: we want to maximise the
    // price, so subsets of "stingy" buyers should not be preferred over subsets of "extravagant" buyers.
    // In fact, only the size of the subset should be considered: for a subset of size x, we should obviously
    // pick the x passengers who buy distinctGoldAmounts[n+1] for the highest prices.  Thus, after grouping our
    // passengers by the amount they buy, we should then sort each group in decreasing order of price (buyersOfGoldDecreasingPrice).
    //
    // Now instead of all subsets of buyers we just need to consider the number of buyers:
    //
    //    for allGoldSoldPreviously = 0, 1, 2, ... , X:
    //       for numBuyers = 0, 1, ..., total that buy distinctGoldAmounts[n+1]:
    //          priceOfSoldGoldAmount = <total price of top numBuyers passengers who buy distinctGoldAmounts[n+1]> 
    //          amountOfSoldGoldAmount = numBuyers * distinctGoldAmounts[n+1]
    //          bestPriceForSoldGold[n+1][allGoldSoldPreviously + amountOfSoldGoldAmount] 
    //            = max(bestPriceForSoldGold[n+1][allGoldSoldPreviously + amountOfSoldGoldAmount],
    //                  bestPriceForSoldGold[n][allGoldSoldPreviously] + priceOfSoldGoldAmount);
    //
    // This is much more efficient.  The value of priceOfSoldGoldAmount can be calculated incrementally.
    //
    // Doing this over all distinctGoldAmounts, an upper bound for the complexity is O(N * X) - the
    //
    //    for n = 0, 1, ... , |distinctGoldAmounts| - 1:
    //       <snip>
    //       for numBuyers = 0, 1, ..., total that buy distinctGoldAmounts[n+1]:
    //
    // is the same as iterating over all passengers, and for each passenger we loop over X + 1 values of allGoldSoldPreviously.
    //
    // O(N * X) is not quite good enough, though, with X = 5000 and N = 1'000'000 - it would be a few times too slow.  Can we do
    // better? Actually, this upper bound is pessimistic: imagine that distinctGoldAmounts is sorted.  Now take a closer look
    // at the portion:
    //
    //       for numBuyers = 0, 1, ..., total that buy distinctGoldAmounts[n+1]:
    //          priceOfSoldGoldAmount = <total price of top numBuyers passengers who buy distinctGoldAmounts[n+1]> 
    //          amountOfSoldGoldAmount = numBuyers * distinctGoldAmounts[n+1]
    //          bestPriceForSoldGold[n+1][allGoldSoldPreviously + amountOfSoldGoldAmount] 
    //            = <snip>
    //
    // Since we want to avoid computing bestPriceForSoldGold[n+1][g] for g > X, and amountOfSoldGoldAmount = numBuyers * distinctGoldAmounts,
    // we see that this loop actually terminates earlier and earlier as we work our way across distinctGoldAmounts: in fact, it's always
    // bounded by X / distinctGoldAmounts i.e. first time we encounter it it is bounded by X / 1; then by X / 2; then by X / 3 (as distinctGoldAmounts is
    // increasing) etc.  This diminishes very quickly, and we end up performing (asymptotically?) fewer computations than
    // O(N * X), though I can't quite calculate the tighter asymptotic bound.  
    //
    // Anyway, the simple step of avoiding computing bestPriceForSoldGold[n+1][g] for g > X, which we need to do for correctness anyway,
    // actually ends up collapsing the amount of work we need to do quite significantly, and we can then easily handle testcases
    // with the maximum X and N.
    //
    // And that's it!
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
