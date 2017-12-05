// Simon St James (ssjgz) - 2017-12-05
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

struct Location
{
    vector<Location*> neighbours;
    int grundyNumberForSoldier = -1;
};

int mex(const vector<int>& numbers)
{
    if (numbers.empty())
        return 0;
    auto numbersSorted = numbers;
    sort(numbersSorted.begin(), numbersSorted.end());
    numbersSorted.erase(unique(numbersSorted.begin(), numbersSorted.end()), numbersSorted.end());

    int mex = 0;
    auto numberIter = numbersSorted.begin();
    while (numberIter != numbersSorted.end() && mex == *numberIter)
    {
        mex++;
        numberIter++;
    }

    return mex;

}

int findGrundyNumber(Location* location)
{
    if (location->grundyNumberForSoldier != -1)
        return location->grundyNumberForSoldier;

    vector<int> grundyNumbersForNeighbours;
    for (const auto& neighbour : location->neighbours)
    {
        grundyNumbersForNeighbours.push_back(findGrundyNumber(neighbour));
    }

    const int grundyNumber = mex(grundyNumbersForNeighbours);

    location->grundyNumberForSoldier = grundyNumber;

    return grundyNumber;
}

int main()
{
    int numLocations, numPaths;
    cin >> numLocations >> numPaths;

    vector<Location> locations(numLocations);

    for (int i = 0; i < numPaths; i++)
    {
        int startLocation, endLocation;
        cin >> startLocation >> endLocation;

        // Make 0-relative.
        startLocation--;
        endLocation--;

        locations[startLocation].neighbours.push_back(&(locations[endLocation]));
    }

    for (auto& location : locations)
    {
        findGrundyNumber(&location);
    }

    int numQueries;
    cin >> numQueries;

    for (int q = 0; q < numQueries; q++)
    {
        int numSoldiers;
        cin >> numSoldiers;

        int nimSum = 0;
        for (int i = 0; i < numSoldiers; i++)
        {
            int soldierLocation;
            cin >> soldierLocation;

            // Make 0-relative.
            soldierLocation--;

            nimSum ^= findGrundyNumber(&(locations[soldierLocation]));
        }

        if (nimSum != 0)
            cout << "Bumi";
        else
            cout << "Iroh";
        cout << endl;
    }
    assert(cin);

}
