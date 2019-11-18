// Simon St James (ssjgz) - 2019-11-18
// 
// Solution to: https://www.codechef.com/problems/CHEFSQ
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
bool solveBruteForce(const vector<int>& sequence, const vector<int>& candiateSubsequence)
{
    int numOfCandidateSubsequenceFound = 0;
    for (const auto sequenceValue : sequence)
    {
        if (sequenceValue == candiateSubsequence[numOfCandidateSubsequenceFound])
        {
            numOfCandidateSubsequenceFound++;
            if (numOfCandidateSubsequenceFound == candiateSubsequence.size())
                return true;
        }
    }
    
    return false;
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
        vector<int> sequence(N);
        for (auto& value : sequence)
            value = read<int>();

        const int M = read<int>();
        vector<int> candiateSubsequence(M);
        for (auto& value : candiateSubsequence)
            value = read<int>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(sequence, candiateSubsequence);
        cout << (solutionBruteForce ? "Yes" : "No") << endl;
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
