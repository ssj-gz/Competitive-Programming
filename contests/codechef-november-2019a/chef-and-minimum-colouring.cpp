// Simon St James (ssjgz) - 2019-11-06
// 
// Solution to: https://www.codechef.com/NOV19B/problems/CAMC
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
int solveBruteForce(int N, int M, const vector<int64_t>& a)
{
    int result = 0;

    vector<int> colouring(N, 0);

    while (true)
    {
        bool isValidColouring = true;
        for (int i = 0; i  + M <= a.size() && isValidColouring; i++)
        {
            vector<bool> isColouredUsed(M, false);
            for (int j = i; j < i + M; j++)
            {
                if (isColouredUsed[colouring[j]])
                {
                    isValidColouring = false;
                    break;
                }
                isColouredUsed[colouring[j]] = true;
            }
        }

        if (isValidColouring)
        {
            cout << "Valid colouring: " << endl;
            for (int i = 0; i < N; i++)
            {
                cout << " " << colouring[i];
            }
            cout << endl;
        }

        int index = 0;
        while (index < N && colouring[index] == M - 1)
        {
            colouring[index] = 0;
            index++;
        }
        if (index == N)
            break;

        colouring[index]++;
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
        const int M = read<int>();

        vector<int64_t> a(N);
        for (auto& x : a)
            x = read<int64_t>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(N, M, a);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
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
