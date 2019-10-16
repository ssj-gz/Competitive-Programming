// Simon St James (ssjgz) - 2019-10-16
// 
// Solution to: https://www.codechef.com/problems/S10E
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

int solveBruteForce(const vector<int>& priceForDay)
{
    int result = 0;
    for (int day = 0; day < priceForDay.size(); day++)
    {
        const int currentPrice = priceForDay[day];
        bool isGood = true;
        for (int previousDay = day - 1; day - previousDay <= 5 && previousDay >= 0; previousDay--)
        {
            if (priceForDay[previousDay] <= currentPrice)
            {
                isGood = false;
                break;
            }

        }

        if (isGood)
            result++;
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
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();

        vector<int> priceForDay(N);

        for (auto& price : priceForDay)
        {
            price = read<int>();
        }

#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(priceForDay);
        cout << solutionBruteForce << endl;
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
