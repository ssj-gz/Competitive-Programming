// Simon St James (ssjgz) - 2019-08-24
#define SUBMISSION
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

int64_t solveOptimised(const string& s)
{
    // Let the "bit value" of a letter a, b, c ... be defined as
    //
    //   2 ** (distance of letter from 'a')
    //
    // i.e. 
    //
    //  bit_value(a) == 2 ** 0 = 1
    //  bit_value(b) == 2 ** 1 = 2
    //  bit_value(c) == 2 ** 2 = 4
    //
    // etc.
    //
    // Let the "xor sum" of a substring
    //
    //   a_i a_(i+1) ... a_j 
    //
    // be defined as 
    //
    //   xorSum (a_i a_(i+1) ... a_j) = bit_value(a_i) ^ bit_value(a_(i+1)) ^ ... ^ bit_value(a_j)
    //
    // Then a substring s will be a palindrome if and only if the binary representation
    // of xorSum(s) has *at most* one bit set.
    //
    // Let prefixXorSum(k) = xorSum(a1 a2 ... a_k); then note that
    //
    //   xorSum(a_i a_(i+1) ... a_k) = prefixXorSum(i - 1) ^ prefixXorSum(k)
    //
    // Thus, to find the number of palindromic-substrings ending at a_k, we need to calculate prefixXorSum(k)
    // and then find the number of i's such that prefixXorSum(i) ^ prefixXorSum(k) has at most one bit set
    // i.e. the number of i's such that prefixXorSum(i) ^ prefixXorSum(k) is either 0, or a power of 2.
    // This is pretty easy.
    int64_t result = 0;

    // We can probably use a large (2 ** 26 * sizeof(int) ~ 256MB) lookup table instead of a std::map, here,
    // giving us asymptotically better runtime performance at the expense of memory usage.
    map<uint32_t, int> numPrefixesWithXorSum;
    numPrefixesWithXorSum[0] = 1; // Empty prefix.

    uint32_t prefixXorSum = 0;
    for (int i = 0; i < s.size(); i++)
    {
        const int letterIndex = s[i] - 'a';
        prefixXorSum = prefixXorSum ^ (1 << letterIndex); 

        result += numPrefixesWithXorSum[prefixXorSum]; // Num substrings ending at k with xorSum == 0.
        for (int i = 0; i < 26; i++)
        {
            result += numPrefixesWithXorSum[prefixXorSum ^ (1 << i)]; // Num substrings ending at k with the ith bit of their xorSum set.
        }

        numPrefixesWithXorSum[prefixXorSum]++;
    }
    
    return result;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int N = rand() % 1'000 + 1;
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
    
    const int T = read<int>();
    
    for (int t = 0; t < T; t++)
    {
        const string s = read<string>();
#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(s);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        const auto solutionOptimised = solveOptimised(s);
        cout << "solutionOptimised: " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#else
        const auto solutionOptimised = solveOptimised(s);
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
