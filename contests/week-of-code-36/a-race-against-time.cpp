// Simon St James (ssjgz) - 2018-02-08
#define BRUTE_FORCE
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef BRUTE_FORCE
#endif
#include <iostream>
#include <vector>
#include <limits>
#include <cassert>

using namespace std;

int64_t minCostBruteForce(const vector<int>& heights, const vector<int64_t>& prices)
{
    const int n = heights.size();
    vector<int64_t> minCostStartingWithStudent(n);
    minCostStartingWithStudent.back() = 0; // Just run instantly to finish line.

    for (int i = n - 2; i >= 0; i--)
    {
        cout << "i: " << i << endl;
        int64_t minCostStartingHere = numeric_limits<int64_t>::max();
        bool forcedExchange = false;
        for (int nextStudent = i + 1; nextStudent < n; nextStudent++)
        {
            minCostStartingHere = (nextStudent - i) + prices[nextStudent] + abs(heights[nextStudent] - heights[i]) + minCostStartingWithStudent[nextStudent];
            cout << " nextStudent: " << nextStudent << " minCostStartingHere becomes " << minCostStartingHere << endl;
            if (heights[nextStudent] > heights[i])
            {
                cout << "  Forced exchange!" << endl;
                forcedExchange = true;
                break;
            }
        }
        if (!forcedExchange)
        {
            // Run to finish line.
            minCostStartingHere = min(minCostStartingHere, static_cast<int64_t>(n - i));
            cout << " Run straight to finish line" << endl;
        }
        cout << " minCostStartingHere: " << minCostStartingHere << endl;
        minCostStartingWithStudent[i] = minCostStartingHere;


    }

    for (int i = 0; i < n; i++)
    {
        cout << "minCostStartingWithStudent[" << i << "] = " << minCostStartingWithStudent[i] << endl;
    }

    return minCostStartingWithStudent[0];
}

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

    vector<int64_t> prices(N);
    prices[0] = 0; // No charge for Madison.

    for (int i = 1; i <= N - 1; i++)
    {
        cin >> prices[i];
    }

#ifdef BRUTE_FORCE
    const auto resultBruteForce = minCostBruteForce(heights, prices);
    cout << "resultBruteForce: " << resultBruteForce << endl;
#endif

}
