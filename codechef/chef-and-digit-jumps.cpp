// Simon St James (ssjgz) - 2020-07-30
// 
// Solution to: https://www.codechef.com/LRNDSA08/problems/DIGJUMP
//
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#else
#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#define BRUTE_FORCE
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

#if 0
int solveBruteForce(const vector<int>& digitString)
{
    int result = -1;
    
    return result;
}
#endif

#if 1
int solveOptimised(const vector<int>& digits)
{
    const int digitsLength = digits.size();

    vector<int> minDistToDigitIndex(digitsLength, -1);
    vector<vector<int>> unvisitedIndicesWithDigit(10);
    for (int index = 0; index < digitsLength; index++)
    {
        unvisitedIndicesWithDigit[digits[index]].push_back(index);
    }

    vector<int> indicesToExplore;
    indicesToExplore.push_back(0);
    minDistToDigitIndex[0] = 0;

    int numSteps = 1;

    while (!indicesToExplore.empty())
    {
        vector<int> nextIndicesToExplore;

        auto visitIndex = [&](int index)
        {
            if (index < 0 || index >= digitsLength)
                return;
            if (minDistToDigitIndex[index] != -1)
                return;
            minDistToDigitIndex[index] = numSteps;
            nextIndicesToExplore.push_back(index);
        };

        for (const auto index : indicesToExplore)
        {
            visitIndex(index - 1);
            visitIndex(index + 1);
            const auto digitAtIndex = digits[index];
            for (const auto unvisitedIndexWithThisDigit : unvisitedIndicesWithDigit[digitAtIndex])
            {
                visitIndex(unvisitedIndexWithThisDigit);
            }
            unvisitedIndicesWithDigit[digitAtIndex].clear(); // We've just visited them all!
        }

        numSteps++;
        indicesToExplore = nextIndicesToExplore;
    }
    
    return minDistToDigitIndex[digitsLength - 1];
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
    
    const auto digitString = read<string>();
    vector<int> digits;
    for (const auto& digit : digitString)
    {
        digits.push_back(digit - '0');
    }
    const auto solutionOptimised = solveOptimised(digits);
    cout << solutionOptimised << endl;
    assert(cin);
}
