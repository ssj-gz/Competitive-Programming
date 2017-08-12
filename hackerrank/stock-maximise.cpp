// Simon St James (ssjgz) 2017-08-12 10:27
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    // Oops - was very sloppy with this one :/
    // Anyway, the optimal strategy is an easy one: pick the largest stock price (if multiple ones
    // have the same price, pick the first of them - although I  guess it doesn't actually matter),
    // and say it occurs at index x.
    // Now, we can buy at any index y < x and sell at a profit (or at least break even) because the
    // stock price at index y < stock price at index x; moreover, if we buy a stock at index y, selling at x
    // gives us the greatest possible profit for that stock.  The max profit we can make at index x, then, occurs
    // when we buy at every index y < x and then sell at x, and this is the stock price at x * the total number
    // of indexes (numUnsoldStocks) y < x minus the total cost of all stocks at index y < x (costOfUnsoldStocks).
    // Having reached x, we wipe the slate clean, and repeat: in the remaining stocks (those with index >= x + 1), pick
    // the largest one, etc.
    // The rest is an exercise in book-keeping; one tricky part is keeping track of the highest remaining stock after we sell:
    // this can be accomplished by sorting all stock prices in decreasing order, keeping them in sync with their original
    // indices (stockPricesWithIndicesDecreasing), and after selling at x, discarding each one whose index <= x: we're then left with the largest stock
    // price in the remaining stocks with indices >= x + 1.
    // All of this can be accomplished in O(N) - the cost of keeping track of the highest remaining stock after each sell
    // is amortised linear as we only make one pass of stockPricesWithIndicesDecreasing in total.
    int T;
    cin >> T;
    for (int t = 1; t <= T; t++)
    {
        int N;
        cin >> N;
        vector<int64_t> stockPrices(N);
        struct StockPriceWithIndex
        {
            StockPriceWithIndex(const int64_t stockPrice, const int index)
                : stockPrice{stockPrice}, index{index}
            {
            }
            StockPriceWithIndex()
                : stockPrice{-1}, index{-1}
            {
            }
            int64_t stockPrice;
            int index;
        };
        vector<StockPriceWithIndex> stockPricesWithIndicesDecreasing(N);
        for (int i = 0; i < N; i++)
        {
            cin >> stockPrices[i];
            stockPricesWithIndicesDecreasing[i] = StockPriceWithIndex(stockPrices[i], i);
        }
        sort(stockPricesWithIndicesDecreasing.begin(), stockPricesWithIndicesDecreasing.end(), [](const auto& lhs, const auto& rhs) { return rhs.stockPrice < lhs.stockPrice; });

        auto nextHighestStockPriceIter = stockPricesWithIndicesDecreasing.begin();
        int64_t numUnsoldStocks = 0;
        int64_t costOfUnsoldStocks = 0;
        int64_t totalProfit = 0;
        int stockPriceIndex = 0;
        for (const int64_t stockPrice : stockPrices)
        {
            assert(stockPrice <= nextHighestStockPriceIter->stockPrice);
            if (nextHighestStockPriceIter != stockPricesWithIndicesDecreasing.end() && stockPrice == nextHighestStockPriceIter->stockPrice)
            {
                const int64_t revenueFromSellingUnsold = (stockPrice * numUnsoldStocks);
                const int64_t profitFromSellingUnsold = revenueFromSellingUnsold - costOfUnsoldStocks;
                assert(profitFromSellingUnsold >= 0);

                totalProfit += profitFromSellingUnsold;

                numUnsoldStocks = 0;
                costOfUnsoldStocks = 0;
                while (nextHighestStockPriceIter != stockPricesWithIndicesDecreasing.end() && nextHighestStockPriceIter->index <= stockPriceIndex)
                    nextHighestStockPriceIter++;
            }
            else
            {
                numUnsoldStocks++;
                costOfUnsoldStocks += stockPrice;
            }
            stockPriceIndex++;
        }
        cout << totalProfit << endl;
    }
}
