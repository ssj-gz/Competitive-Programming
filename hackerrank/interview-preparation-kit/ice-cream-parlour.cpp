// Simon St James (ssjgz) 2019-04-09
#include <iostream>
#include <vector>
#include <unordered_map>

#include <cassert>

using namespace std;

int main()
{
    int numTrips;
    cin >> numTrips;

    for (int t = 0; t < numTrips; t++)
    {
        int money;
        cin >> money;

        int numCosts;
        cin >> numCosts;

        unordered_map<int, vector<int>> indicesWithCost;
        int index = 1;
        for (int i = 0; i < numCosts; i++)
        {
            int cost;
            cin >> cost;
            indicesWithCost[cost].push_back(index);

            index++;
        }

        int iceCream1Index = -1;
        int iceCream2Index = -1;
        for (const auto [iceCream1Cost, unused] : indicesWithCost)
        {
            const auto iceCream2Cost = money - iceCream1Cost;
            if (indicesWithCost.find(iceCream2Cost) != indicesWithCost.end())
            {
                if (iceCream1Cost == iceCream2Cost)
                {
                    // For this to be a solution, we need two icecreams with this cost.
                    if (indicesWithCost[iceCream1Cost].size() > 1)
                    {
                        assert(indicesWithCost[iceCream1Cost].size() == 2);
                        iceCream1Index = indicesWithCost[iceCream1Cost].front(); 
                        iceCream2Index = indicesWithCost[iceCream1Cost].back(); 
                    }
                }
                else
                {
                    assert(indicesWithCost[iceCream1Cost].size() == 1);
                    assert(indicesWithCost[iceCream2Cost].size() == 1);
                    iceCream1Index = indicesWithCost[iceCream1Cost].front(); 
                    iceCream2Index = indicesWithCost[iceCream2Cost].front(); 
                }
            }

            index++;
        }
        assert(iceCream1Index != -1 && iceCream2Index != -1);
        if (iceCream2Index < iceCream1Index)
            swap(iceCream1Index, iceCream2Index);

        cout << iceCream1Index << " " << iceCream2Index << endl;
    }
}
