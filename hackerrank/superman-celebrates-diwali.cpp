// Simon St James (ssjgz) - 2019-05-23
#include <iostream>
#include <vector>
#include <utility>
#include <cassert>

using namespace std;

int main(int argc, char* argv[])
{
    auto readInt = []() { int x; cin >> x; assert(cin); return x; };
    const int numBuildings = readInt();

    const int buildingHeight = readInt();

    const int buildingSwitchHeightLoss = readInt();

    vector<vector<int>> numInBuildingOnFloor(numBuildings + 1, vector<int>(buildingHeight + 1, 0));

    for (int buildingNum = 0; buildingNum < numBuildings; buildingNum++)
    {
        const int numInBuilding = readInt();

        for (int personNum = 0; personNum < numInBuilding; personNum++)
        {
            const int personsFloor = readInt();

            numInBuildingOnFloor[buildingNum][personsFloor]++;
        }
    }

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
                // Update some of the lookup tables needed for efficient calculation of max score if we switch buildings.
                maxScoreIfSwitchToBuildingGreaterThan[otherBuildingNum] = max(maxScoreIfSwitchToBuildingGreaterThan[otherBuildingNum + 1], maxScoreFromBuildingAndFloorLookup[otherBuildingNum + 1][floorNumIfSwitchBuildings]);
            }
        }

        int64_t maxScoreIfSwitchToBuildingLessThanCurrent = 0;
        for (int buildingNum = 0; buildingNum < numBuildings; buildingNum++)
        {
            // Stay in same building; descend to next floor.
            const int64_t bestResultAfterDescend = maxScoreFromBuildingAndFloorLookup[buildingNum][floorNum - 1];

            int64_t bestResultAfterSwitch = 0;
            if (canSwitchBuildings)
            {
                // Switch buildings.
                if (buildingNum > 0)
                {
                    // Update more of the lookup stuff needed for efficient calculation of max score if we switch buildings.
                    maxScoreIfSwitchToBuildingLessThanCurrent = max(maxScoreIfSwitchToBuildingLessThanCurrent, maxScoreFromBuildingAndFloorLookup[buildingNum - 1][floorNumIfSwitchBuildings]);
                }
                bestResultAfterSwitch = max(maxScoreIfSwitchToBuildingGreaterThan[buildingNum], maxScoreIfSwitchToBuildingLessThanCurrent);
            }


            const auto maxScoreForBuildingAndFloor = numInBuildingOnFloor[buildingNum][floorNum] + // Rescue these people.
                                                     max(bestResultAfterDescend, bestResultAfterSwitch); // Rescue the remaining people, by descending or switching buildings.
            maxScoreFromBuildingAndFloorLookup[buildingNum][floorNum] = maxScoreForBuildingAndFloor;
        }
    }

    int64_t maxRescued = 0;
    for (int buildingNum = 0; buildingNum < numBuildings; buildingNum++)
    {
        maxRescued = max(maxRescued, maxScoreFromBuildingAndFloorLookup[buildingNum][buildingHeight]);
    }

    cout << maxRescued << endl;
}

