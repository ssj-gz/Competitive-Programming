// Simon St James (ssjgz) - 2019-05-23
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <utility>
#include <cassert>

#include <sys/time.h>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));


        const int numBuildings = rand() % 100 + 1;
        const int buildingHeight = rand() % 100 + 1;
        const int buildingSwitchHeightLoss = rand() % 100 + 1;

        cout << numBuildings << " " << buildingHeight << " " << buildingSwitchHeightLoss << endl; 
        for (int buildingNum = 0; buildingNum < numBuildings; buildingNum++)
        {
            const int numInBuilding = rand() % 100;
            cout << numInBuilding;
            for (int personNum = 0; personNum < numInBuilding; personNum++)
            {
                cout << " " << (rand() % buildingHeight + 1);
            }
            cout << endl;
        }
        return 0;
    }
    int numBuildings;
    cin >> numBuildings;

    int buildingHeight;
    cin >> buildingHeight;

    int buildingSwitchHeightLoss;
    cin >> buildingSwitchHeightLoss;

    vector<vector<int>> numInBuildingOnFloor(numBuildings + 1, vector<int>(buildingHeight + 1, 0));

    for (int buildingNum = 0; buildingNum < numBuildings; buildingNum++)
    {
        int numInBuilding;
        cin >> numInBuilding;
        //cout << "numInBuilding: " << numInBuilding << endl;

        for (int personNum = 0; personNum < numInBuilding; personNum++)
        {
            int personsFloor;
            cin >> personsFloor;

            numInBuildingOnFloor[buildingNum][personsFloor]++;
            //cout << "numInBuildingOnFloor[" << buildingNum << "][" << personsFloor << "] = " << numInBuildingOnFloor[buildingNum][personsFloor] << endl;
        }
    }
    assert(cin);

    vector<vector<int64_t>> maxScoreFromBuildingAndFloorLookup(numBuildings + 1, vector<int64_t>(buildingHeight + 1, 0));

    for (int floorNum = 1; floorNum <= buildingHeight; floorNum++)
    {
        const int floorNumIfSwitchBuildings = floorNum - buildingSwitchHeightLoss;
        const bool canSwitchBuildings = (floorNumIfSwitchBuildings >= 0);
        vector<int64_t> maxScoreIfSwitchToBuildingGreaterThan(numBuildings + 1, 0);
        if (canSwitchBuildings)
        {
            for (int otherBuildingNum = numBuildings - 1; otherBuildingNum >= 0; otherBuildingNum--)
            {
                maxScoreIfSwitchToBuildingGreaterThan[otherBuildingNum] = max(maxScoreIfSwitchToBuildingGreaterThan[otherBuildingNum + 1], maxScoreFromBuildingAndFloorLookup[otherBuildingNum + 1][floorNumIfSwitchBuildings]);
                //cout << "floorNum: " << floorNum <<" maxScoreIfSwitchToBuildingGreaterThan[" << otherBuildingNum << "] = " << maxScoreIfSwitchToBuildingGreaterThan[otherBuildingNum]<< endl;
            }
        }

        int64_t maxScoreIfSwitchToBuildingLessThanCurrent = 0;
        for (int buildingNum = 0; buildingNum < numBuildings; buildingNum++)
        {
            //int64_t result = numInBuildingOnFloor[buildingNum][floorNum]; // Rescue these people.
            //cout << "buildingNum: " << buildingNum << " floorNum: " << floorNum << " rescue: " << result << endl;

            // Stay in same building; descend to next floor.
            const int64_t bestResultAfterDescend = maxScoreFromBuildingAndFloorLookup[buildingNum][floorNum - 1];
            int64_t bestResultAfterSwitch = 0;
            if (canSwitchBuildings)
            {
#ifdef BRUTE_FORCE
                // Switch to other buildings.
                const int numBuildings = numInBuildingOnFloor.size();
                for (int otherBuildingNum = 0; otherBuildingNum < numBuildings; otherBuildingNum++)
                {
                    if (otherBuildingNum == buildingNum)
                        continue;

                    if (floorNum - buildingSwitchHeightLoss >= 0)
                    {
                        bestResultAfterSwitch = max(bestResultAfterSwitch, maxScoreFromBuildingAndFloorLookup[otherBuildingNum][floorNumIfSwitchBuildings]);
                    }
                }
#endif
                if (buildingNum > 0)
                {
                    maxScoreIfSwitchToBuildingLessThanCurrent = max(maxScoreIfSwitchToBuildingLessThanCurrent, maxScoreFromBuildingAndFloorLookup[buildingNum - 1][floorNumIfSwitchBuildings]);
                }
                const auto bestResultAfterSwitchOptimised = max(maxScoreIfSwitchToBuildingGreaterThan[buildingNum], maxScoreIfSwitchToBuildingLessThanCurrent);
                //cout << "buildingNum: " << buildingNum << " floorNum: " << floorNum << " bestResultAfterSwitch: " << bestResultAfterSwitch << " bestResultAfterSwitchOptimised: " << bestResultAfterSwitchOptimised << " maxScoreIfSwitchToBuildingGreaterThan[buildingNum]: " << maxScoreIfSwitchToBuildingGreaterThan[buildingNum] << " maxScoreIfSwitchToBuildingLessThanCurrent: " << maxScoreIfSwitchToBuildingLessThanCurrent << endl;
                assert(bestResultAfterSwitch == bestResultAfterSwitchOptimised);
                bestResultAfterSwitch = bestResultAfterSwitchOptimised;
            }


            const auto result = numInBuildingOnFloor[buildingNum][floorNum] + // Rescue these people.
                               max(bestResultAfterDescend, bestResultAfterSwitch);
            maxScoreFromBuildingAndFloorLookup[buildingNum][floorNum] = result;

            //cout << "buildingNum: " << buildingNum << " floorNum: " << floorNum << " result: " << result << endl;
        }
    }

    int64_t result = 0;
    for (int buildingNum = 0; buildingNum < numBuildings; buildingNum++)
    {
        result = max(result, maxScoreFromBuildingAndFloorLookup[buildingNum][buildingHeight]);
    }

    cout << result << endl;
}

