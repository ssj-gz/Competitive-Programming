// Simon St James (ssjgz) 2019-04-09
#include <iostream>
#include <vector>
#include <unordered_map>

#include <cassert>

using namespace std;

int main()
{
    // Trivially easy, though the case where the solution consists of two
    // icecreams with the same cost is a little fiddly!

    auto readInt = []() { int x; cin >> x; assert(cin); return x;};

    const int numTrips = readInt();

    for (int t = 0; t < numTrips; t++)
    {
        const int money = readInt();
        const int numCosts = readInt();

        unordered_map<int, vector<int>> indicesWithCost;
        for (int i = 0; i < numCosts; i++)
        {
            const int cost = readInt();
            indicesWithCost[cost].push_back(i + 1); // Store indices as 1-relative.
        }

        int iceCream1Index = -1;
        int iceCream2Index = -1;
        for (const auto [iceCream1Cost, unused] : indicesWithCost)
        {
            // NB: the assertions below assert the uniqueness of a solution
            // (as promised by the Challenge description).
            const auto iceCream2Cost = money - iceCream1Cost;
            if (indicesWithCost.find(iceCream2Cost) != indicesWithCost.end())
            {
                // We have an icecream that can be pooled with Icecream 1!
                if (iceCream1Cost == iceCream2Cost)
                {
                    // For this to be a solution, we need at least *two* icecreams with this cost.
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
        }
        assert(iceCream1Index != -1 && iceCream2Index != -1);
        if (iceCream2Index < iceCream1Index)
            swap(iceCream1Index, iceCream2Index); // Output should be in ascending order of index.

        cout << iceCream1Index << " " << iceCream2Index << endl;
    }
}
