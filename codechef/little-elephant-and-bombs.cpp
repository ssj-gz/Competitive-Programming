// Simon St James (ssjgz) - 2019-12-29
// 
// Solution to: https://www.codechef.com/problems/LEBOMBS
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
            const auto numBuildings = 1 + rand() % 10;
            cout << numBuildings << endl;
            string buildingString(numBuildings, '0');

            for (auto& building : buildingString)
                building = '0' + rand() % 2;

            cout << buildingString << endl;
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int numBuildings = read<int>();
        const auto buildingString = read<string>();
        auto buildingStringAfterExplosion = buildingString;

        for (int buildingIndex = 0; buildingIndex < numBuildings; buildingIndex++)
        {
            if (buildingString[buildingIndex] == '1')
            {
                if (buildingIndex > 0)
                    buildingStringAfterExplosion[buildingIndex - 1] = 'E';
                if (buildingIndex + 1 < numBuildings)
                    buildingStringAfterExplosion[buildingIndex + 1] = 'E';
                buildingStringAfterExplosion[buildingIndex] = 'E';
            }
        }

        cout << count(buildingStringAfterExplosion.begin(), buildingStringAfterExplosion.end(), '0') << endl;
    }

    assert(cin);
}
