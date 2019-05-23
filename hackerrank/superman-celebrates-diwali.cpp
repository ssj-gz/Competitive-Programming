// Simon St James (ssjgz) - 2019-05-23
#include <iostream>
#include <vector>
#include <utility>
#include <cassert>

using namespace std;

int main(int argc, char* argv[])
{
    // "Hard"? "80 Points"?? Well, I can't really complain as I bungled it the first time
    // I suppose (schoolboy error - the original way I calculated bestResultAfterSwitch
    // added an extra factor of numBuildings to the computational complexity, which I missed
    // completely XD).  It really *is* fundamentally easy, though!
    //
    // The recurrence relation for building maxScoreFromBuildingAndFloorLookup is very very simple
    // - rescue the people on this floor and building; add the amount rescued if we either
    // a) descend to the next floor of the current building or b) switch to a different building
    // (with the resulting height loss), whichever is larger.
    //
    // The only "difficult" bit comes from calculating this latter (bestResultAfterSwitch) efficiently
    // i.e. in O(1) instead of the O(numBuildings) it would be if we did it naively, but this is 
    // easy with a bit of incremental updating of lookup helpers (maxScoreIfSwitchToBuildingLessThanCurrent;
    // maxScoreIfSwitchToBuildingGreaterThan) - hopefully the code for this is self-explanatory.
    //
    // And that's it!
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
                // i.e. bestResultAfterSwitch = max[otherBuilding != buildingNum] {maxScoreFromBuildingAndFloorLookup[otherBuilding][floorNumIfSwitchBuildings]}.
                bestResultAfterSwitch = max(maxScoreIfSwitchToBuildingGreaterThan[buildingNum], maxScoreIfSwitchToBuildingLessThanCurrent);
            }


            const auto maxScoreForBuildingAndFloor = numInBuildingOnFloor[buildingNum][floorNum] + // Rescue these people.
                                                     max(bestResultAfterDescend, bestResultAfterSwitch); // Rescue the remaining people, by descending or switching buildings.
            maxScoreFromBuildingAndFloorLookup[buildingNum][floorNum] = maxScoreForBuildingAndFloor;
        }
    }

    // Max rescued = max over all buildings of max rescued if we started on top floor of that building.
    int64_t maxRescued = 0;
    for (int buildingNum = 0; buildingNum < numBuildings; buildingNum++)
    {
        maxRescued = max(maxRescued, maxScoreFromBuildingAndFloorLookup[buildingNum][buildingHeight]);
    }

    cout << maxRescued << endl;
}

