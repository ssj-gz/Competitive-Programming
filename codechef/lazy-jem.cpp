// Simon St James (ssjgz) - 2019-10-29
// 
// Solution to: https://www.codechef.com/problems/TALAZY
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

int64_t solveBruteForce(int64_t N, int64_t B, int64_t M)
{
    int64_t timeTaken = 0;

    int64_t timeToSolveOneProblem = M;
    while (true)
    {
        const auto numToSolveBeforeBreak = ((N % 2) == 0) ? N / 2 : N / 2 + 1;
        timeTaken += numToSolveBeforeBreak * timeToSolveOneProblem;
        N -= numToSolveBeforeBreak;

        if (N == 0)
            break;

        timeTaken += B;

        timeToSolveOneProblem *= 2;

    }
     
    return timeTaken;
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
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto N = read<int64_t>();
        const auto B = read<int64_t>();
        const auto M = read<int64_t>();

        const auto solutionBruteForce = solveBruteForce(N, B, M);
        cout << solutionBruteForce << endl;
    }

    assert(cin);
}
