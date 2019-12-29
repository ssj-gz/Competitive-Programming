// Simon St James (ssjgz) - 2019-12-21
// 
// Solution to: https://www.codechef.com/problems/FLOW005
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

#if 1
int solveBruteForce(const vector<int>& denomimationsDecreasing, int targetValue)
{
    int result = 0;

    for (const auto& denomimation : denomimationsDecreasing)
    {
        result += targetValue / denomimation;
        targetValue = targetValue % denomimation;
    }
    
    return result;
}
#endif

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
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
#if 0
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {

        }
#else
        const vector<int> denomimationsDecreasing = { 100, 50, 10, 5, 2, 1};
        for (int N = 1; N <= 1'000'000; N++)
        {

            cout << "Q: 2 lines" << endl;
            cout << 1 << endl;
            cout << N << endl;
            cout << "A: 1 lines" << endl;
            cout << solveBruteForce(denomimationsDecreasing, N) << endl;
        }
#endif

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    const vector<int> denomimationsDecreasing = { 100, 50, 10, 5, 2, 1};

    for (int t = 0; t < T; t++)
    {

        const int N = read<int>();

#ifdef BRUTE_FORCE
#if 1
        cout << solveBruteForce(denomimationsDecreasing, N) << endl;
#endif
#if 0
        const auto solutionOptimised = solveOptimised();
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
