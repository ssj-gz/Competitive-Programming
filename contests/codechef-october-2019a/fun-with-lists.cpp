// Simon St James (ssjgz) - 2019-10-04
// 
// Solution to: https://www.codechef.com/OCT19A/problems/TANDON
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


int64_t solveBruteForce(int64_t N, int K)
{
    int64_t result = 0;
    int64_t powerOf10 = 1;
    for (int i = 0; i < N; i++)
    {
        powerOf10 *= 10;
    }
    int lastValid = 0;
    for (int i = 0; i <= powerOf10 - 1; i++)
    {
        if (i % K == 0)
        {
            auto iAsString = to_string(i);
            reverse(iAsString.begin(), iAsString.end());
            auto reverseI = stoi(iAsString);
            if (reverseI % K == 0)
            {
                cout << "i: " << i << endl;
                result++;
                const auto diffFromLast = (i - lastValid) / 7;
                cout << " diff from last: " << diffFromLast << " i: " << i << endl;

                if (diffFromLast > 19)
                {
                    cout << "i: " << i << " lastValid: " << lastValid << endl;
                }

                assert(diffFromLast <= 19);
                lastValid = i;
            }
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

        const auto N = read<int64_t>();
        const auto K = read<int>();
#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(N, K);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
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
