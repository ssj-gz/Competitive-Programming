// Simon St James (ssjgz) - 2019-05-23
#include <iostream>
#include <vector>
#include <utility>
#include <cassert>

using namespace std;

int main()
{
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
                cout << "floorNum: " << floorNum <<" maxScoreIfSwitchToBuildingGreaterThan[" << otherBuildingNum << "] = " << maxScoreIfSwitchToBuildingGreaterThan[otherBuildingNum]<< endl;
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
                // Switch to other buildings.
                const int numBuildings = numInBuildingOnFloor.size();
                for (int otherBuildingNum = 0; otherBuildingNum < numBuildings; otherBuildingNum++)
                {
                    if (otherBuildingNum == buildingNum)
                        continue;

                    if (floorNum - buildingSwitchHeightLoss >= 0)
                    {
                        bestResultAfterSwitch = max(bestResultAfterSwitch, maxScoreFromBuildingAndFloorLookup[otherBuildingNum][floorNum - buildingSwitchHeightLoss]);
                    }
                }
                const auto bestResultAfterSwitchOptimised = max(maxScoreIfSwitchToBuildingGreaterThan[buildingNum], maxScoreIfSwitchToBuildingLessThanCurrent);
                cout << "buildingNum: " << buildingNum << " floorNum: " << floorNum << " bestResultAfterSwitch: " << bestResultAfterSwitch << " bestResultAfterSwitchOptimised: " << bestResultAfterSwitchOptimised << " maxScoreIfSwitchToBuildingGreaterThan[buildingNum]: " << maxScoreIfSwitchToBuildingGreaterThan[buildingNum] << " maxScoreIfSwitchToBuildingLessThanCurrent: " << maxScoreIfSwitchToBuildingLessThanCurrent << endl;
                assert(bestResultAfterSwitch == bestResultAfterSwitchOptimised);
            }


            const auto result = numInBuildingOnFloor[buildingNum][floorNum] + // Rescue these people.
                               max(bestResultAfterDescend, bestResultAfterSwitch);
            maxScoreFromBuildingAndFloorLookup[buildingNum][floorNum] = result;

            if (canSwitchBuildings && buildingNum > 0)
            {
                maxScoreIfSwitchToBuildingLessThanCurrent = max(maxScoreIfSwitchToBuildingLessThanCurrent, maxScoreFromBuildingAndFloorLookup[buildingNum - 1][floorNumIfSwitchBuildings]);
            }
            cout << "buildingNum: " << buildingNum << " floorNum: " << floorNum << " result: " << result << endl;
        }
    }

    int64_t result = 0;
    for (int buildingNum = 0; buildingNum < numBuildings; buildingNum++)
    {
        result = max(result, maxScoreFromBuildingAndFloorLookup[buildingNum][buildingHeight]);
    }

    cout << result << endl;
}

