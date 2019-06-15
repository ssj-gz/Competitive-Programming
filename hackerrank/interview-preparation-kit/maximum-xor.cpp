// Simon St James (ssjgz) - 2019-06-15
//#define SUBMISSION
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
    for (const auto query : queries)
    {
        cout << "query: " << query << endl;
        int largestXor = -1;
        for (const auto x : a)
        {
            const auto thisXor = x ^ query;
            if (thisXor > largestXor)
            {
                cout << " found new best of " << thisXor << " by xoring with " << x << endl;
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
    vector<int> sortedA(originalA);
    sort(sortedA.begin(), sortedA.end());

#ifdef BRUTE_FORCE
    vector<int> dbgSortedA(sortedA);
#endif

    cout << "sorted A" << endl;
    for (const auto x : sortedA)
    {
        cout << x << " ";
    }
    cout << endl;
    for (const auto query : queries)
    {
        cout << "query: " << query << endl;
        int rangeBegin = 0;
        int rangeEnd = sortedA.size() - 1;
        uint32_t cumulativeSubtraction = 0;
        uint32_t powerOf2 = (1U << 20U); // TODO - restore this!
        //uint32_t powerOf2 = (1U << 10U);
        while (powerOf2 >= 1)
        {
            cout << " powerOf2: " << powerOf2 << " query & powerOf2: " << (query & powerOf2) << endl;
            if (query & powerOf2)
            {
                auto blah = std::lower_bound(sortedA.begin() + rangeBegin, sortedA.begin() + rangeEnd + 1, powerOf2 + cumulativeSubtraction);
                const int oldRangeEnd = rangeEnd;
                if (blah != sortedA.begin() + rangeEnd + 1)
                {
                    if (blah != sortedA.begin())
                    {
                        blah = std::prev(blah);
                        if (blah - sortedA.begin() >= rangeBegin)
                        {
                            cout << " adjusting rangeEnd" << endl;
                            rangeEnd = blah - sortedA.begin();
                        }
                        else
                        {
                        }
                    }
                }
                if (oldRangeEnd == rangeEnd)
                {
                            //cumulativeSubtraction += powerOf2;
#ifdef BRUTE_FORCE
                            //for(auto& x : dbgSortedA)
                            //{
                                //x -= powerOf2;
                            //}
#endif
                }

            }
            else
            {
                auto blah = std::lower_bound(sortedA.begin() + rangeBegin, sortedA.begin() + rangeEnd + 1, powerOf2 + cumulativeSubtraction);
                if (blah != sortedA.begin() + rangeEnd + 1)
                {
                    cout << " adjusting rangeBegin" << endl;
                    rangeBegin = blah - sortedA.begin();

                    cumulativeSubtraction += powerOf2;
#ifdef BRUTE_FORCE
                        for(auto& x : dbgSortedA)
                        {
                            x -= powerOf2;
                        }
#endif
                }
            }
            cout << " rangeBegin: " << rangeBegin << " rangeEnd: " << rangeEnd << " cumulativeSubtraction: " << cumulativeSubtraction << endl;
#ifdef BRUTE_FORCE
            cout << "dbgSortedA" << endl;
            for(const auto& x : dbgSortedA)
            {
                cout << x << " ";
            }
            cout << endl;
            for (int i = rangeBegin; i <= rangeEnd; i++)
            {
                assert (dbgSortedA[i] >= 0);
                assert(sortedA[i] == dbgSortedA[i] + cumulativeSubtraction);
            }
#endif


            powerOf2 >>= 1;
        }
        auto blah = max(query ^ sortedA[rangeBegin], query ^ sortedA[rangeEnd]);
        results.push_back(blah);

    }
    return results;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        //const int n = rand() % 100 + 1;
        //const int m = rand() % 100 + 1;
        const int n = 10;
        const int m = 1;

        //const int maxA = rand() % 10'000'000;
        //const int maxQ = rand() % 10'000'000;
        const int maxA = rand() % 512 + 1;
        const int maxQ = rand() % 512 + 1;

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
        cout << "glarp: " << x << endl;
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
