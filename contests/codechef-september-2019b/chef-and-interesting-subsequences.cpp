// Simon St James (ssjgz) - 2019-09-09
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/CHEFINSQ
//
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

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

constexpr auto maxN = 50;

vector<vector<int64_t>> nCrLookup(maxN + 1, vector<int64_t>(maxN + 1, -1));

void buildnCrLookup()
{
    for (auto n = 0; n <= maxN; n++)
    {
        nCrLookup[0][n] = 0;
        nCrLookup[n][n] = 1;
        nCrLookup[n][0] = 1;
    }
    for (auto n = 1; n <= maxN; n++)
    {
        for (auto k = 1; k < maxN; k++)
        {
            assert(nCrLookup[n - 1][k - 1] != -1);
            assert(nCrLookup[n - 1][k] != -1);

            // The usual recurrence relation for nCr: see e.g. https://en.wikipedia.org/wiki/Binomial_coefficient
            nCrLookup[n][k] = nCrLookup[n - 1][k - 1] + nCrLookup[n - 1][k];
        }
    }
}

int64_t findNumInterestingSubsequences(int N, int K, const vector<int>& aOriginal)
{
    if (K > N)
        return 0;

    auto a = aOriginal;

    sort(a.begin(), a.end());
    const auto kthElement = a[K - 1];
    const auto numOfKthElement = count(a.begin(), a.end(), kthElement);
    auto numOfKthElementInSubset = 1;
    K--;
    while (K - 1 >= 0 && a[K - 1] == kthElement)
    {
        K--;
        numOfKthElementInSubset++;
    }
    
    return nCrLookup[numOfKthElement][numOfKthElementInSubset];
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    buildnCrLookup();

    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto N = read<int>();
        const auto K = read<int>();

        vector<int> a(N);
        for (auto& aElement : a)
        {
            aElement = read<int>();
        }

        cout << findNumInterestingSubsequences(N, K, a) << endl;
    }

    assert(cin);
}
