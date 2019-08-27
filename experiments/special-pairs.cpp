// Simon St James (ssjgz) - 2019-XX-XX
//
// Solution for "Special Pairs" - https://www.hackerearth.com/practice/algorithms/dynamic-programming/bit-masking/practice-problems/algorithm/special-pairs-7/
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

int64_t solveBruteForce(const vector<int>& a)
{
    int64_t result = 0;

    for (int i = 0; i < a.size(); i++)
    {
        for (int j = 0; j < a.size(); j++)
        {
            if ((a[i] & a[j]) == 0)
                result++;
        }
    }

    return result;
}

#if 0
SolutionType solveOptimised()
{
    SolutionType result;
    
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
        cout << 1 << endl;
        const int N = rand() % 1000 + 1;
        const int maxA = rand() % 1'000'000 + 1;
        cout << N << endl;
        for (int i = 0; i < N; i++)
        {
            cout << (rand() % maxA) << " ";
        }
        cout << endl;
        return 0;
    }
    
    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        vector<int> a(N);
        for (auto& aElement : a)
        {
            aElement = read<int>();
        }

#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(a);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#if 0
        const auto solutionOptimised = solveOptimised();
        cout << "solutionOptimised: " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
