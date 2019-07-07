// Simon St James (ssjgz) - 2019-04-07
#include <iostream>
#include <vector>
#include <limits>

#include <cassert>

using namespace std;

int findMinDownToZero(int N, vector<int>& minDownToZeroLookup, const vector<vector<int>>& factorsLookup)
{
    //cout << "N: " << N << endl;
    if (minDownToZeroLookup[N] != -1)
    {
        //cout << " returning cached" << endl;
        return minDownToZeroLookup[N];
    }


    int minDownToZero = std::numeric_limits<int>::max();
    minDownToZero = min(minDownToZero, 1 + findMinDownToZero(N - 1, minDownToZeroLookup, factorsLookup));
    for (const auto factor : factorsLookup[N])
    {
        if (factor * factor > N)
            break;

        assert(N / factor == max(N / factor, factor));
        minDownToZero = min(minDownToZero, 1 + findMinDownToZero(N / factor, minDownToZeroLookup, factorsLookup));
    }


    //cout << " caching " << N << " = " << minDownToZero << endl; 
    minDownToZeroLookup[N] = minDownToZero;

    return minDownToZero;
}

int main()
{
    int Q;
    cin >> Q;

    const int maxN = 1'000'000;
    vector<int> minDownToZeroLookup(maxN + 1, -1);
    minDownToZeroLookup[0] = 0;

    vector<vector<int>> factorsLookup(maxN);
    for (int factor = 2; factor <= maxN; factor++)
    {
        for (int multiple = factor; multiple <= maxN; multiple += factor)
        {
            if (factor * factor <= multiple)
            {
                factorsLookup[multiple].push_back(factor);
            }
        }
    }
    //cout << "Built factorsLookup" << endl;
#if 0
    for (int i = 1; i <= maxN; i++)
    {
        cout << "N: " << i << " factors: " << endl;
        for (const auto f : factorsLookup[i])
        {
            cout << f << " ";
        }
        cout << endl;
    }
#endif

    for (int i = 1; i <= maxN; i++)
    {
#if 0
        if ((i % 100) == 0)
            cout << "i: " << i << " / " << maxN << endl;
#endif
        findMinDownToZero(i, minDownToZeroLookup, factorsLookup);
    }

    for (int q = 0; q < Q; q++)
    {
        int N;
        cin >> N;
        assert(cin);

        assert(N <= maxN);

        cout << findMinDownToZero(N, minDownToZeroLookup, factorsLookup) << endl;
    }
}
