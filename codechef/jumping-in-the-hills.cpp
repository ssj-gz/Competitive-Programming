// Simon St James (ssjgz) - 2019-10-30
// 
// Solution to: https://www.codechef.com/problems/HILLS
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
int solveBruteForce(int N, int U, int D, const vector<int>& hillHeights)
{
    int result = 0;

    int hillPos = 0;

    bool haveParachute = true;

    for (hillPos = 0; hillPos + 1 < N; hillPos++)
    {
        if (hillHeights[hillPos + 1] - hillHeights[hillPos] > U)
            break;
        if (hillHeights[hillPos] - hillHeights[hillPos + 1] > D)
        {
            if (haveParachute)
                haveParachute = false;
            else
                break;
        }
    }
    
    return hillPos + 1;
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
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int N = 1 + rand() % 100;
            const int U = 1 + rand() % 100;
            const int D = 1 + rand() % 100;

            cout << N << " " << U << " " << D << endl;

            const int maxHeight = 1 + rand() % 100;

            for (int i = 0; i < N; i++)
            {
                cout << (1 + rand() % maxHeight) << " ";
            }
            cout << endl;
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int U = read<int>();
        const int D = read<int>();

        vector<int> hillHeights(N);
        for (auto& height : hillHeights)
        {
            cin >> height;
        }


#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(N, U, D, hillHeights);
        cout << solutionBruteForce << endl;
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
