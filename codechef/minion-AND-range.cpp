// Simon St James (ssjgz) - 2020-01-10
// 
// Solution to: https://www.codechef.com/PLIT2020/problems/MINIAND/
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

struct Query
{
    int leftIndex = -1;
    int rightIndex = -1;
};

#if 1
vector<bool> solveBruteForce(const vector<int>& a, const vector<Query>& queries)
{
    vector<bool> queryResults;
    
    for (const auto& query : queries)
    {
        int andResult = a[query.leftIndex];
        for (int index = query.leftIndex + 1; index <= query.rightIndex; index++)
        {
            andResult = (andResult & a[index]);
        }
        cout << "and result: " << andResult << endl;
        queryResults.push_back((andResult % 2 == 1));
    }
    
    return queryResults;
}
#endif

#if 1
vector<bool> solveOptimised(const vector<int>& a, const vector<Query>& queries)
{
    vector<bool> queryResults;

    vector<int> numEvensInPrefixOfLen;
    numEvensInPrefixOfLen.push_back(0);
    int numEvens = 0;
    for (const auto value : a)
    {
        if (value % 2 == 0)
            numEvens++;
        numEvensInPrefixOfLen.push_back(numEvens);
    }


    for (const auto& query : queries)
    {
        const int numEvensInRange = numEvensInPrefixOfLen[query.rightIndex + 1] - numEvensInPrefixOfLen[query.leftIndex];
        const bool andIsOdd = (numEvensInRange == 0);
        queryResults.push_back(andIsOdd);
    }

    return queryResults;

}
#endif


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int N = 1 + rand() % 10;
            const int numQueries = 1 + rand() % 10;

            cout << N << endl;
            for (int i = 0; i < N; i++)
            {
                cout << 1 + (rand() % 10);
                if (i != N)
                    cout << " ";
            }
            cout << endl;
            cout << numQueries << endl;
            for (int i = 0; i < numQueries; i++)
            {
                int l = 1 + rand() % N;
                int r = 1 + rand() % N;
                if (l > r)
                    swap(l, r);
                cout << l << " " << r << endl;
            }
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        vector<int> a(N);
        for (auto& value : a)
            value = read<int>();

        const int numQueries = read<int>();
        vector<Query> queries(numQueries);

        for (auto& query : queries)
        {
            query.leftIndex = read<int>() - 1;
            query.rightIndex = read<int>() - 1;
        }

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(a, queries);
        for (bool queryResult : solutionBruteForce)
        {
            cout << "solutionBruteForce: " << (queryResult ? "ODD" : "EVEN") << endl;
        }
#endif
#if 1
        const auto solutionOptimised = solveOptimised(a, queries);
        for (bool queryResult : solutionOptimised)
        {
            cout << (queryResult ? "ODD" : "EVEN") << endl;
        }

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised(a, queries);
        for (bool queryResult : solutionOptimised)
        {
            cout << (queryResult ? "ODD" : "EVEN") << endl;
        }
#endif
    }

    assert(cin);
}
