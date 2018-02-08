#include <iostream>
#include <vector>
#include <limits>

using namespace std;

int main()
{
    int N;
    cin >> N;

    vector<int> heights(N);

    // Treat Madison's height as heights[0].
    for (int i = 0; i < N; i++)
    {
        cin >> heights[i];
    }

    vector<int> prices(N);
    prices[0] = 0; // No charge for Madison.

    for (int i = 1; i <= N - 1; i++)
    {
        cin >> prices[i];
    }

    vector<int64_t> totalSumAndPriceForLastM(N);
    totalSumAndPriceForLastM.back() = 0; // No charge for last guy.

    for (int i = N - 2; i >= 0; i--)
    {
        int64_t lowestSumAndPriceFromHere = numeric_limits<int64_t>::max();
        const int 
        if (i + 2 < N)
        {
            const int64_t costOfSkipping = lowestSumAndPriceFromHere[i + 2] + 
        }
        
    }

}
