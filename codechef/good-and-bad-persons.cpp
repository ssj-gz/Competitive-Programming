// Simon St James (ssjgz) - 2019-10-31
// 
// Solution to: https://www.codechef.com/problems/GOODBAD
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
#include <algorithm> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}


string solveBruteForce(int N, const string& s, int K)
{
    int numLowerCase = 0;
    int numUpperCase = 0;

    for (const auto letter : s)
    {
        if (islower(letter))
            numLowerCase++;
        else
            numUpperCase++;
    }

    const bool couldBeChef = numUpperCase <= K;
    const bool couldBeBrother = numLowerCase <= K;

    if (couldBeChef && couldBeBrother)
        return "both";
    if (couldBeChef)
        return "chef";;
    if (couldBeBrother)
        return "brother";
    
    return "none";
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
            const int N = 1 + rand() % 100;
            const int K = rand() % (N + 1);
            cout << N << " " << K << endl;
            string s;
            const int numLowercase = rand() % (N + 1);
            vector<bool> useLowercase(N);
            for (int i = 0; i < numLowercase; i++)
                useLowercase[i] = true;

            random_shuffle(useLowercase.begin(), useLowercase.end());

            for (int i = 0; i < N; i++)
            {
                s += (useLowercase[i] ? 'a' : 'A') + rand() % 26;
            }
            cout << s << endl;
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int K = read<int>();
        const string s = read<string>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(N, s, K);
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
