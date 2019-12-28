// Simon St James (ssjgz) - 2019-12-28
// 
// Solution to: https://www.codechef.com/problems/WALKFAST
//
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
            const int numCities = 1 + rand() % 20;
            const int AIndex = 1 + rand() % numCities;
            const int BIndex = 1 + rand() % numCities;
            const int CIndex = 1 + rand() % numCities;
            const int DIndex = 1 + rand() % numCities;
            const int walkSecsPerMetre = 1 + rand() % 100;
            const int trainSecsPerMetre = 1 + rand() % 100;
            const int trainLeaveTime = 1 + rand() % 100;

            vector<int> cityPositions;
            for (int i = -1000; i <= 1000; i++)
            {
                cityPositions.push_back(i);
            }
            random_shuffle(cityPositions.begin(), cityPositions.end());
            cityPositions.erase(cityPositions.begin() + numCities, cityPositions.end());
            sort(cityPositions.begin(), cityPositions.end());

            cout << numCities << " " << AIndex << " " << BIndex << " " << CIndex << " " << DIndex << " " << walkSecsPerMetre << " " << trainSecsPerMetre << " " << trainLeaveTime << endl;
            for (int i = 0; i < numCities; i++)
            {
                cout << cityPositions[i];
                if (i != numCities - 1)
                    cout << " ";
            }
            cout << endl;
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int numCities = read<int>();
        const int AIndex = read<int>() - 1;
        const int BIndex = read<int>() - 1;
        const int CIndex = read<int>() - 1;
        const int DIndex = read<int>() - 1;
        const int walkSecsPerMetre = read<int>();
        const int trainSecsPerMetre = read<int>();
        const int trainLeaveTime = read<int>();

        vector<int> cityPositions(numCities);
        for (auto& cityPos : cityPositions)
        {
            cityPos = read<int>();
        }

        const int distAB = abs(cityPositions[AIndex] - cityPositions[BIndex]);
        const int distCD = abs(cityPositions[CIndex] - cityPositions[DIndex]);
        const int distAC = abs(cityPositions[AIndex] - cityPositions[CIndex]);
        const int distDB = abs(cityPositions[DIndex] - cityPositions[BIndex]);

        const int timeJustWalk = distAB * walkSecsPerMetre;
        int minTime = timeJustWalk;

        const int timeWalkToTrain = distAC * walkSecsPerMetre;
        if (timeWalkToTrain <= trainLeaveTime)
        {
            const int timeWalkThenTrain = max(trainLeaveTime, timeWalkToTrain) + distCD * trainSecsPerMetre + distDB * walkSecsPerMetre;
            minTime = min(minTime, timeWalkThenTrain);
        }

        cout << minTime << endl;
    }

    assert(cin);
}
