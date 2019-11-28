// Simon St James (ssjgz) - 2019-11-28
// 
// Solution to: https://www.codechef.com/problems/COOK82C
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <deque>
#include <limits>
#include <algorithm>

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

#if 1
vector<int64_t> solveBruteForce(const vector<int64_t>& aOriginal, const vector<int>& queries)
{
    vector<int64_t> result;

    vector<int64_t> a(aOriginal);

    int currentOperationNum = 1;
    int queryIndex = 0;

    while (queryIndex < queries.size())
    {
        const auto maxElementIter = max_element(a.begin(), a.end());
        if (currentOperationNum == queries[queryIndex])
        {
            result.push_back(*maxElementIter);
            queryIndex++;
        }
        const auto maxElement = *maxElementIter;
        a.erase(maxElementIter);
        if (maxElement / 2 > 0)
        {
            a.push_back(maxElement / 2);
        }
        currentOperationNum++;
    }
    
    return result;
}
#endif

#if 1
vector<int64_t> solveOptimised(const vector<int64_t>& aOriginal, const vector<int>& queries)
{
    vector<int64_t> result;

    deque<int64_t> newElements;


    vector<int64_t> a(aOriginal);
    sort(a.begin(), a.end());

    int currentOperationNum = 1;
    int queryIndex = 0;

    while (queryIndex < queries.size())
    {
        bool maxIsInA = (newElements.empty() || (!a.empty() && a.back() >= newElements.back()));
        const auto maxElement = maxIsInA ? a.back() : newElements.back();
        if (currentOperationNum == queries[queryIndex])
        {
            result.push_back(maxElement);
            queryIndex++;
        }
        if (maxIsInA)
        {
            a.pop_back();
        }
        else
        {
            newElements.pop_back();
        }
        if (maxElement / 2 > 0)
        {
            assert(newElements.empty() || maxElement / 2 <= newElements.front());
            newElements.push_front(maxElement / 2);
        }
        currentOperationNum++;
    }
    
    return result;

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

        const int N = 1'000'000;
        const int M = N;
        const int maxPowerOf2 = 63;
        const int64_t maxValue = std::numeric_limits<int64_t>::max();

        cout << N << " " << M << endl;
        for (int i = 0; i < N; i++)
        {
            const int powerOf2 = rand() % (maxPowerOf2 + 1);
            int64_t twoToThePower = 1;
            for (int j = 0; j < powerOf2; j++)
            {
                twoToThePower *= 2;
            }
            int multiplier = rand() % (maxValue / twoToThePower + 1);

            cout << (multiplier * twoToThePower) << " ";
        }
        cout << endl;
        for (int i = 0; i < M; i++)
        {
            cout << (i + 1) << endl;
        }

        return 0;
    }


    const int N = read<int>();
    const int M = read<int>();

    vector<int64_t> a(N);
    for (auto& x : a)
        x = read<int64_t>();

    vector<int> queries(M);
    for (auto& x : queries)
        x = read<int>();


#ifdef BRUTE_FORCE
#if 1
    const auto solutionBruteForce = solveBruteForce(a, queries);
    cout << "solutionBruteForce-: " << endl;
    for (const auto x : solutionBruteForce)
    {
        cout << "solutionBruteForce: " << x << endl;
    }
#endif
#if 1
    const auto solutionOptimised = solveOptimised(a, queries);
    cout << "solutionOptimised:  " << endl;
    for (const auto x : solutionOptimised)
    {
        cout << "solutionOptimised: " << x << endl;
    }

    assert(solutionOptimised == solutionBruteForce);
#endif
#else
    const auto solutionOptimised = solveOptimised(a, queries);
    for (const auto x : solutionOptimised)
    {
        cout << x << '\n';
    }
#endif

    assert(cin);
}
