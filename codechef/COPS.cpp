// Simon St James (ssjgz) - 2019-XX-XX
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

#include <sys/time.h>

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
        cout << 1 << endl;
        const int M = rand() % 10 + 1;
        const int x = rand() % 10 + 1;
        const int y = rand() % 10 + 1;
        cout << M << " " << x << " " << y << endl;
        vector<int> allHouseNumbers;
        for (int i = 1; i <= 100; i++)
        {
            allHouseNumbers.push_back(i);
        }
        random_shuffle(allHouseNumbers.begin(), allHouseNumbers.end());
        for (int i = 0; i < M; i++)
        {
            cout << allHouseNumbers[i] << " ";
        }
        cout << endl;
        return 0;
    }

    // TODO - read in testcase.
    const int numHouses = 100;
    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int M = read<int>();
        const int x = read<int>();
        const int y = read<int>();

        vector<int> copLocations(M);
        for (auto& location : copLocations)
            location = read<int>() - 1;


        const int houseSearchSpan = x * y;

        vector<bool> isHouseSafe(numHouses, true);

        for (const auto copLocation : copLocations)
        {
            for (int i = copLocation - houseSearchSpan; i <= copLocation + houseSearchSpan; i++)
            {
                if (0 <= i && i < isHouseSafe.size())
                    isHouseSafe[i] = false;
            }
        }

        int numSafeHouses = 0;
        for (const auto safe : isHouseSafe)
        {
            if (safe)
                numSafeHouses++;
        }

        cout << numSafeHouses << endl;
    }


    assert(cin);
#ifdef BRUTE_FORCE
#if 0
    const auto solutionBruteForce = solveBruteForce();
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 0
    const auto solutionOptimised = solveOptimised();
    cout << "solutionOptimised: " << solutionOptimised << endl;

    assert(solutionOptimised == solutionBruteForce);
#endif
#else
    const auto solutionOptimised = solveOptimised();
    cout << solutionOptimised << endl;
#endif

}


