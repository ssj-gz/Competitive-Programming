// Simon St James (ssjgz) - 2019-XX-XX
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <map>

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
int64_t solveBruteForce(const string& s)
{
    int64_t result = 0;

    for (int i = 0; i < s.size(); i++)
    {
        for (int j = i; j < s.size(); j++)
        {
            const int numLetters = 26;
            int letterHistogram[numLetters] = {};
            for (int k = i; k <= j; k++)
            {
                letterHistogram[s[k] - 'a']++;
            }
            int numLettersWithOddOccurrence = 0;
            for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
            {
                if ((letterHistogram[letterIndex] % 2) == 1)
                    numLettersWithOddOccurrence++;
            }
            if (numLettersWithOddOccurrence == 0 || numLettersWithOddOccurrence == 1)
            {
                cout << "Palindromic: " << s.substr(i, j - i + 1) << endl;
                result++;
            }
        }
    }
    
    return result;
}
#endif

#if 1
int64_t solveOptimised(const string& s)
{
    int64_t result = 0;

    map<uint32_t, int> numPrefixesWithXorSum;
    numPrefixesWithXorSum[0] = 1; // Empty prefix.

    uint32_t prefixXorSum = 0;
    for (int i = 0; i < s.size(); i++)
    {
        const int letterIndex = s[i] - 'a';
        prefixXorSum = prefixXorSum ^ (1 << letterIndex); 

        result += numPrefixesWithXorSum[prefixXorSum];
        for (int i = 0; i < 26; i++)
        {
            result += numPrefixesWithXorSum[prefixXorSum ^ (1 << i)];
        }

        numPrefixesWithXorSum[prefixXorSum]++;
    }
    
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

        const int N = rand() % 100 + 1;
        const int maxLetter = rand() % 26 + 1;

        cout << 1 << endl;
        //cout << N << endl;
        for (int i = 0; i < N; i++)
        {
            cout << static_cast<char>('a' + rand() % maxLetter);
        }
        cout << endl;

        return 0;
    }
    
    // TODO - read in testcase.
    const int T = read<int>();
    
    for (int t = 0; t < T; t++)
    {
        const string s = read<string>();
#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(s);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(s);
        cout << "solutionOptimised: " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised(s);
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
