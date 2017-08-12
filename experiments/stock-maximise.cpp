// Simon St James (ssjgz) 2017-08-12 10:27
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
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
