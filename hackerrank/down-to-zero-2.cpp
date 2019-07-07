// Simon St James (ssjgz) - 2019-04-07
#include <iostream>
#include <vector>
#include <limits>

#include <cassert>

using namespace std;

int findMinDownToZero(int N, vector<int>& minDownToZeroLookup)
{
    if (minDownToZeroLookup[N] != -1)
        return minDownToZeroLookup[N];


    int minDownToZero = std::numeric_limits<int>::max();
    for (int factor = 2; factor * factor <= N; factor++)
    {
        if ((N % factor) != 0)
            continue;

        assert(N / factor == max(N / factor, factor));

        minDownToZero = min(minDownToZero, 1 + findMinDownToZero(N / factor, minDownToZeroLookup));
    }

    minDownToZero = min(minDownToZero, 1 + findMinDownToZero(N - 1, minDownToZeroLookup));

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

    for (int q = 0; q < Q; q++)
    {
        int N;
        cin >> N;
        assert(cin);
        cout << "N: " << N << endl;

        assert(N <= maxN);

        cout << findMinDownToZero(N, minDownToZeroLookup) << endl;
    }
}
