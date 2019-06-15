// Simon St James (ssjgz) - 2019-06-15
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <cassert>

#include <sys/time.h>

using namespace std;

vector<int> solveBruteForce(const vector<int>& a, const vector<int>& queries)
{
    vector<int> results;
    results.reserve(queries.size());
    for (const auto query : queries)
    {
        //cout << "query: " << query << endl;
        int largestXor = -1;
        for (const auto x : a)
        {
            const auto thisXor = x ^ query;
            if (thisXor > largestXor)
            {
                //cout << " found new best of " << thisXor << " by xoring with " << x << endl;
                largestXor = thisXor;
            }
        }
        results.push_back(largestXor);
    }
    return results;
}

vector<int> solveOptimised(const vector<int>& originalA, const vector<int>& queries)
{
    vector<int> results;
    results.reserve(queries.size());

    vector<int> sortedA(originalA);
    sort(sortedA.begin(), sortedA.end());

    for (const auto query : queries)
    {
#ifdef BRUTE_FORCE
        vector<int> dbgSortedA(sortedA);
#endif
        int rangeBegin = 0;
        int rangeEnd = sortedA.size() - 1;
        uint32_t cumulativeSubtraction = 0;
        uint32_t powerOf2 = (1U << 31U);
        while (powerOf2 >= 1)
        {
            auto firstInRangeAtLeastPowerOf2Iter = std::lower_bound(sortedA.begin() + rangeBegin, sortedA.begin() + rangeEnd + 1, powerOf2 + cumulativeSubtraction);
            const bool haveAtLeastPowerOf2InRange = (firstInRangeAtLeastPowerOf2Iter != sortedA.begin() + rangeEnd + 1);
            if (query & powerOf2)
            {
                if (haveAtLeastPowerOf2InRange)
                {
                    if (firstInRangeAtLeastPowerOf2Iter != sortedA.begin() + rangeBegin)
                    {
                        firstInRangeAtLeastPowerOf2Iter = std::prev(firstInRangeAtLeastPowerOf2Iter);
                        rangeEnd = firstInRangeAtLeastPowerOf2Iter - sortedA.begin();
                    }
                    else
                    {
                        cumulativeSubtraction += powerOf2;
#ifdef BRUTE_FORCE
                        for(auto& x : dbgSortedA)
                        {
                            x -= powerOf2;
                        }
#endif
                    }
                }

            }
            else
            {
                if (haveAtLeastPowerOf2InRange)
                {
                    rangeBegin = firstInRangeAtLeastPowerOf2Iter - sortedA.begin();

                    cumulativeSubtraction += powerOf2;
#ifdef BRUTE_FORCE
                    for(auto& x : dbgSortedA)
                    {
                        x -= powerOf2;
                    }
#endif
                }
            }
#ifdef BRUTE_FORCE
            for (int i = rangeBegin; i <= rangeEnd; i++)
            {
                assert (dbgSortedA[i] >= 0);
                assert (dbgSortedA[i] < powerOf2);
                assert(sortedA[i] == dbgSortedA[i] + cumulativeSubtraction);
            }
#endif


            powerOf2 >>= 1;
        }
        assert(rangeEnd - rangeBegin <= 1);
        auto maxXor = max(query ^ sortedA[rangeBegin], query ^ sortedA[rangeEnd]);
        results.push_back(maxXor);

    }
    return results;
}

int main(int argc, char* argv[])
{
    // A fair amount of I/O on this one.
    ios::sync_with_stdio(false);

    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int n = rand() % 100 + 1;
        const int m = rand() % 100 + 1;
        //const int n = 10;
        //const int m = 1;

        const int maxA = rand() % 10'000'000;
        const int maxQ = rand() % 10'000'000;
        //const int maxA = rand() % 512 + 1;
        //const int maxQ = rand() % 512 + 1;

        cout << n << endl;
        for (int i = 0; i < n; i++)
        {
            cout << (rand() % maxA) << " ";
        }
        cout << endl;

        cout << m << endl;
        for (int i = 0; i < m; i++)
        {
            cout << (rand() % maxQ) << " ";
        }
        cout << endl;

        return 0;


    }
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

#ifdef BRUTE_FORCE
    const auto bruteForceResults = solveBruteForce(a, queries);
    cout << "Brute force results: " << endl;
    for (const auto x : bruteForceResults)
    {
        cout << "glarp: " << x << endl;
    }
    cout << endl;
    const auto optimisedResults = solveOptimised(a, queries);
    cout << "optimised results: " << endl;
    for (const auto x : optimisedResults)
    {
        cout << x << endl;
    }
    assert(bruteForceResults == optimisedResults);
#else
    const auto optimisedResults = solveOptimised(a, queries);
    for (const auto x : optimisedResults)
    {
        cout << x << endl;
    }
#endif
}
