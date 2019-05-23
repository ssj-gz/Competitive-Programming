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

    vector<vector<int>> numInBuildingOnFloor(numBuildings, vector<int>(buildingHeight + 1, 0));

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

    vector<vector<int64_t>> maxScoreFromBuildingAndFloorLookup(numBuildings, vector<int64_t>(buildingHeight + 1, 0));

    for (int floorNum = 1; floorNum <= buildingHeight; floorNum++)
    {
        for (int buildingNum = 0; buildingNum < numBuildings; buildingNum++)
        {
            int64_t result = numInBuildingOnFloor[buildingNum][floorNum]; // Rescue these people.
            cout << "buildingNum: " << buildingNum << " floorNum: " << floorNum << " rescue: " << result << endl;

            int64_t bestResultAfterMove = 0;
            // Stay in same building; descend to next floor.
            bestResultAfterMove = max(bestResultAfterMove, maxScoreFromBuildingAndFloorLookup[buildingNum][floorNum - 1]);
            if (floorNum - buildingSwitchHeightLoss >= 0)
            {
                // Switch to other buildings.
                const int numBuildings = numInBuildingOnFloor.size();
                for (int otherBuildingNum = 0; otherBuildingNum < numBuildings; otherBuildingNum++)
                {
                    if (otherBuildingNum == buildingNum)
                        continue;

                    if (floorNum - buildingSwitchHeightLoss >= 0)
                    {
                        bestResultAfterMove = max(bestResultAfterMove, maxScoreFromBuildingAndFloorLookup[otherBuildingNum][floorNum - buildingSwitchHeightLoss]);
                    }
                }
            }

            result += bestResultAfterMove;
            maxScoreFromBuildingAndFloorLookup[buildingNum][floorNum] = result;
        }
    }

    int64_t result = 0;
    for (int buildingNum = 0; buildingNum < numBuildings; buildingNum++)
    {
        result = max(result, maxScoreFromBuildingAndFloorLookup[buildingNum][buildingHeight]);
    }

    cout << result << endl;
}

