// Simon St James (ssjgz) - 2019-11-07
// 
// Solution to: https://www.codechef.com/NOV19A/problems/PBOXES
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
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
vector<int64_t> solveBruteForce(int N, const vector<int64_t>& SOrig, const vector<int64_t>& POrig)
{
    vector<int64_t> result;
    struct SAndP
    {
        int64_t s = -1;
        int64_t p = -1;
    };

    vector<SAndP> sAndP;
    for (int i = 0; i < N; i++)
    {
        sAndP.push_back({SOrig[i], POrig[i]});
    }
    sort(sAndP.begin(), sAndP.end(), [](const auto& lhs, const auto& rhs) { return lhs.s < rhs.s; });

    vector<int64_t> psByS;
    for (int i = 0; i < N; i++)
    {
        psByS.push_back(sAndP[i].p);
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

        return 0;
    }

    const int N = read<int>();

    vector<int64_t> S(N);
    vector<int64_t> P(N);

    for (int i = 0; i < N; i++)
    {
        S[i] = read<int64_t>();
        P[i] = read<int64_t>();
    }

#ifdef BRUTE_FORCE
#if 1
    const auto solutionBruteForce = solveBruteForce(N, S, P);
    cout << "solutionBruteForce--: " << endl;
    for (const auto x : solutionBruteForce)
    {
        cout << "solutionBruteForce: " << x << endl;
    }
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

    assert(cin);
}
