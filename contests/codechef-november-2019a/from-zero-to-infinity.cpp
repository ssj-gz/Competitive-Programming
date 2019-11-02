// Simon St James (ssjgz) - 2019-11-02
// 
// Solution to: https://www.codechef.com/NOV19A/problems/WEIRDO
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

#if 0
SolutionType solveBruteForce()
{
    SolutionType result;
    
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
    {
        while (true)
        {
            const int len = 1 + rand() % 100;
            string s;
            for (int i = 0; i < len; i++)
            {
                s += '0' + rand() % 2;
            }
            bool hasAdjacentOnes = false;
            for (int i = 1; i < len; i++)
            {
                if (s[i] == '1' && s[i - 1] == '1')
                {
                    hasAdjacentOnes = true;
                    break;
                }
            }

            bool allSubstringsAtLeastAsMany0s = true;
            for (int l = 0; l < len; l++)
            {
                for (int r = l + 1; r < len; r++)
                {
                    string substring = s.substr(l, r - l + 1);
                    const int num0s = count(substring.begin(), substring.end(), '0');
                    const int num1s = count(substring.begin(), substring.end(), '1');
                    if (num1s > num0s)
                        allSubstringsAtLeastAsMany0s = false;
                }
            }
            cout << "s: " << s << endl;
            assert(allSubstringsAtLeastAsMany0s == !hasAdjacentOnes);
        }
    }
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

#ifdef BRUTE_FORCE
#if 0
        const auto solutionBruteForce = solveBruteForce();
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
