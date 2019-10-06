// Simon St James (ssjgz) - 2019-10-06
// 
// Solution to: https://www.codechef.com/OCT19A/problems/MARM
//
//#define SUBMISSION
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

vector<int> solveBruteForce(const vector<int>& aOriginal, int64_t K)
{
    const int N = aOriginal.size();
    vector<int> a(aOriginal);
    for (int64_t i = 0; i <= K - 1; i++)
    {
        vector<int> nextA(a);
        nextA[i % N] = a[i % N] ^ a[N - (i % N) - 1];

        a = nextA;
        cout << "After " << (i + 1) << " operations, a is now: " << endl;
        for (const auto x : a)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    
    return a;
}

vector<int> solveOptimised(const vector<int>& aOriginal, int64_t K)
{
    const int N = aOriginal.size();

    if ((N % 2) == 0)
    {
        K = K % (N * 3);
        return solveBruteForce(aOriginal, K);
    }
    else
    {
        const bool kWasGreaterThanOrEqualToN = (K >= N);
        K = K % (N * 3);
        auto result = solveBruteForce(aOriginal, K);
        if (kWasGreaterThanOrEqualToN)
        {
            result[N / 2] = 0;
        }

        return result;
    }
}


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
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int64_t K = read<int64_t>();

        vector<int> a(N);
        for (auto& x : a)
            x = read<int>();

#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(a, K);
        cout << "solutionBruteForce: " << endl;
        for (const auto x : solutionBruteForce)
        {
            cout << x << " ";
        }
        cout << endl;
#if 1
        const auto solutionOptimised = solveOptimised(a, K);
        cout << "solutionOptimised:  " << endl;
        for (const auto x : solutionOptimised)
        {
            cout << x << " ";
        }
        cout << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
