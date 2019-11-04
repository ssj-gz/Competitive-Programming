// Simon St James (ssjgz) - 2019-11-04
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
#include <cmath>
#include <iomanip>
#include <limits>

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

    auto intpower = [](int64_t base, int64_t exponent)
    {
        int64_t result = 1;
        for (int i = 0; i < exponent; i++)
        {
            result *= base;
        }
        return result;
    };

    {
        cout <<  std::setprecision (std::numeric_limits<long double>::digits10 + 1);
        cout << "log 5: " << logl(5) << endl;
        cout << "Blee: " << static_cast<long double>(intpower(2, 12) * intpower(5, 8) * intpower(3, 20)) / (intpower(7, 4) * intpower(5, 7) * intpower(6, 5)) << endl;

        cout << "Bloo: " << expl(12 * logl(2) + 8 * logl(5) + 20 * logl(3) - (4 * logl(7) + 7 * logl(5) + 5 * logl(6))) << endl;

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
