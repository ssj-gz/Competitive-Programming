// Simon St James (ssjgz) - 2020-08-22
// 
// Solution to: https://www.codechef.com/problems/CARVANS
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

#if 1
int solveBruteForce(const vector<int>& carMaxSpeeds)
{
    int numCarsTravellingAtMaxSpeed = 0;
    int maxSpeedWithoutCollision = std::numeric_limits<int>::max();

    for (const auto carMaxSpeed : carMaxSpeeds)
    {
        if (carMaxSpeed <= maxSpeedWithoutCollision)
            numCarsTravellingAtMaxSpeed++;
        maxSpeedWithoutCollision = min(maxSpeedWithoutCollision, carMaxSpeed);
    }
    
    return numCarsTravellingAtMaxSpeed;
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
        const int T = rand() % 10 + 1;
        //const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int N = 1 + rand() % 10;
            cout << N << endl;
            for (int i = 0; i < N; i++)
            {
                cout << (std::numeric_limits<int>::max() - (rand() % 10)) << endl;
                //cout << (1 + rand() % 100) << endl;
            }
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto numCars = read<int>();
        vector<int> carMaxSpeeds(numCars);

        for (auto& carMaxSpeed : carMaxSpeeds)
            carMaxSpeed = read<int>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(carMaxSpeeds);
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
