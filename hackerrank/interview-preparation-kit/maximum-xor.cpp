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
        int largestXor = -1;
        for (const auto x : a)
        {
            largestXor = std::max(largestXor, x ^ query);
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
    for (const auto query : queries)
    {
        //cout << "query: " << query << endl;
        int rangeBegin = 0;
        int rangeEnd = sortedA.size() - 1;
        uint32_t powerOf2 = (1U << 31U);
        while (powerOf2 >= 1)
        {
            //cout << " powerOf2: " << powerOf2 << endl;
            if (query & powerOf2)
            {
                auto blah = std::lower_bound(sortedA.begin() + rangeBegin, sortedA.begin() + rangeEnd + 1, powerOf2);
                if (blah != sortedA.begin() + rangeEnd + 1)
                {
                    if (blah != sortedA.begin())
                    {
                        blah = std::prev(blah);
                        rangeEnd = blah - sortedA.begin();
                    }
                }

            }
            else
            {
                auto blah = std::lower_bound(sortedA.begin() + rangeBegin, sortedA.begin() + rangeEnd + 1, powerOf2);
                if (blah != sortedA.begin() + rangeEnd + 1)
                {
                    rangeBegin = blah - sortedA.begin();
                }
            }
            //cout << " rangeBegin: " << rangeBegin << " rangeEnd: " << rangeEnd << endl;

            powerOf2 >>= 1;
        }
        results.push_back(sortedA[rangeEnd]);

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

        const int n = rand() % 100;
        const int m = rand() % 100;

        const int maxA = rand() % 10'000'000;
        const int maxQ = rand() % 10'000'000;

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
}
