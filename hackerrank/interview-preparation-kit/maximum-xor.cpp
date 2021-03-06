// Simon St James (ssjgz) - 2019-06-15
#define SUBMISSION
#define VERIFY_INVARIANT
#ifdef SUBMISSION
#undef VERIFY_INVARIANT
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <cassert>

using namespace std;


vector<int> findMaxXorForQueries(const vector<int>& originalA, const vector<int>& queries)
{
    vector<int> results;
    results.reserve(queries.size());

    vector<int> sortedA(originalA);
    sort(sortedA.begin(), sortedA.end());

    for (const auto query : queries)
    {
        auto rangeBegin = sortedA.begin();
        auto rangeEnd = sortedA.end();
        uint32_t cumulativeSubtraction = 0;
        uint32_t powerOf2 = (1U << 31U);
        while (powerOf2 >= 1)
        {
            auto firstInRangeAtLeastPowerOf2Iter = std::lower_bound(rangeBegin, rangeEnd, powerOf2 + cumulativeSubtraction);
            const bool haveAtLeastPowerOf2InRange = (firstInRangeAtLeastPowerOf2Iter != rangeEnd);
            if (haveAtLeastPowerOf2InRange)
            {
                if (query & powerOf2)
                {
                    if (firstInRangeAtLeastPowerOf2Iter != rangeBegin)
                    {
                        rangeEnd = firstInRangeAtLeastPowerOf2Iter;
                    }
                    else
                    {
                        cumulativeSubtraction += powerOf2;
                    }

                }
                else
                {
                    rangeBegin = firstInRangeAtLeastPowerOf2Iter;
                    cumulativeSubtraction += powerOf2;
                }
            }
#ifdef VERIFY_INVARIANT
            for (auto i = rangeBegin; i != rangeEnd; i++)
            {
                assert (*i - cumulativeSubtraction >= 0);
                assert (*i - cumulativeSubtraction < powerOf2);
            }
#endif


            powerOf2 >>= 1;
        }
        // Make rangeEnd inclusive, rather than one-past-the-end.
        rangeEnd = std::prev(rangeEnd);
        assert(rangeEnd - rangeBegin <= 1);
        auto maxXor = max(query ^ *rangeBegin, query ^ *rangeEnd);
        results.push_back(maxXor);

    }
    return results;
}

int main(int argc, char* argv[])
{
    // A fair amount of I/O on this one.
    ios::sync_with_stdio(false);

    int n;
    cin >> n;

    vector<int> a(n);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }

    int m;
    cin >> m;

    vector<int> queries(m);
    for (int i = 0; i < m; i++)
    {
        cin >> queries[i];
    }
    assert(cin);

    const auto maxXorsForQueries = findMaxXorForQueries(a, queries);
    for (const auto maxXor : maxXorsForQueries)
    {
        cout << maxXor << endl;
    }
}
