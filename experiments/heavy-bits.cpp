// Simon St James (ssjgz) - 2019-09-30
// 
// Solution to: TODO - Heavy Bits link
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>

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

int weight(const string& binaryString)
{
    int num0s = 0;
    int num1s = 0;
    for (const auto& bit : binaryString)
    {
        assert(bit == '0' || bit == '1');
        if (bit == '0')
            num0s++;
        if (bit == '1')
            num1s++;
    }
    return max(num0s, num1s);
}

int64_t solveBruteForce(const string& s)
{
    int64_t result = 0;
    set<string> distinctSubstrings;
    for (int startPos = 0; startPos < s.size(); startPos++)
    {
        for (int length = 1; startPos + length <= s.size(); length++)
        {
            const string substring = s.substr(startPos, length);
            cout << "substring: " << substring << endl;
            distinctSubstrings.insert(substring);
        }
    }

    for (const auto substring : distinctSubstrings)
    {
        result += weight(substring);
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
            const int N = rand() % 100 + 1;
            cout << N << endl;
            for (int i = 0; i < N; i++)
            {
                cout << static_cast<char>('0' + (rand() % 2));
            }
            cout << endl;
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        read<int>();
        const auto s = read<string>();

#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(s);
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
