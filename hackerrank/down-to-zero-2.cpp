// Simon St James (ssjgz) - 2019-04-07
#include <iostream>
#include <vector>
#include <limits>

#include <cassert>

using namespace std;

int findMinDownToZero(int N, vector<int>& minDownToZeroLookup, const vector<vector<int>>& factorsLookup)
{
    if (minDownToZeroLookup[N] != -1)
    {
        return minDownToZeroLookup[N];
    }

    // The "subtract 1 from N" case.
    int minDownToZero = 1 + findMinDownToZero(N - 1, minDownToZeroLookup, factorsLookup);
    // The "reduce to largest of prodands in product equalling N" case.
    for (const auto factor : factorsLookup[N])
    {
        assert(N / factor == max(N / factor, factor));
        minDownToZero = min(minDownToZero, 1 + findMinDownToZero(N / factor, minDownToZeroLookup, factorsLookup));
    }

    minDownToZeroLookup[N] = minDownToZero;

    return minDownToZero;
}

int main()
{
    // I was a bit over-confident with this one XD
    // It looks like a simple Dynamic Programming problem, but you need to be wary:
    // the recursion step in findMinDownToZero includes the case where we
    // subtract 1 from N and recurse, which for large N is prone to causing a stack
    // overflow (and is very slow besides).  
    // Instead, pre-compute all minDownToZeroLookup from the bottom-up
    // which eliminates this possibility and makes things much more efficient.
    //
    // Not much else besides that - hopefully the comments explain the rest!
    int Q;
    cin >> Q;

    const int maxN = 1'000'000;
    vector<int> minDownToZeroLookup(maxN + 1, -1);
    minDownToZeroLookup[0] = 0;

    // Compute list of factors for all N =  1 ... maxN using Sieve of 
    // Erastophenes.
    // We only care about the factors <= sqrt(N), as the
    // others are just "mirrors" of these (i.e. if F is a factor of
    // N and F > sqrt(N), then N / F is a factor of N and is <= sqrt(N)).
    vector<vector<int>> factorsLookup(maxN);
    for (int64_t factor = 2; factor <= maxN; factor++)
    {
        for (int64_t multiple = factor; multiple <= maxN; multiple += factor)
        {
            if (factor * factor <= multiple)
            {
                factorsLookup[multiple].push_back(factor);
            }
        }
    }

    // Precompute minDownToZeroLookup from bottom-up.
    for (int i = 1; i <= maxN; i++)
    {
        findMinDownToZero(i, minDownToZeroLookup, factorsLookup);
    }

    for (int q = 0; q < Q; q++)
    {
        int N;
        cin >> N;
        assert(cin);

        assert(N <= maxN);

        cout << minDownToZeroLookup[N] << endl;
    }
}
