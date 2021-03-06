// Simon St James (ssjgz) - 2017-12-05
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr int unknownGrundyNumber = -1;

struct Location
{
    vector<Location*> neighbours;
    int grundyNumberForSoldier = unknownGrundyNumber;
};

int mex(const vector<int>& numbers)
{
    if (numbers.empty())
        return 0;
    auto numbersSorted = numbers;
    sort(numbersSorted.begin(), numbersSorted.end());

    int mex = 0;
    for (const auto number : numbersSorted)
    {
        if (number == mex)
            mex++;
        else if (number > mex)
            break;
    }

    return mex;
}

int findGrundyNumber(Location* location)
{
    if (location->grundyNumberForSoldier != unknownGrundyNumber)
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
    // Easy 70 points :) This is just a bunch of Nim-style games played on a graph - one 
    // game for each soldier.  We need to find the grundy number for each Soldier, which depends
    // solely on its start location in the graph i.e. we need to find the grundy number for 
    // each Location in the graph.  This is easy: it is defined recursively (via Sprague-Grundy) by:
    //
    //  If location is a leaf:
    //
    //   grundy(location) = 0
    //
    // else if location has l neighbours neighbour1, neighbour2, ... , neighbour_l (locations that can be 
    // reached directly by following one road):
    //
    //   grundy(location) = mex(grundy(neighbour1), grundy(neighbour2), .... , grundy(neighbour_l))
    //
    // It's then a simple matter to use Sprague Grundy to find the overall winner of the game by xor'ing the
    // grundy numbers of each soldier's starting location.

    ios::sync_with_stdio(false);

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

        // Bumi is the first player.
        if (nimSum != 0)
            cout << "Bumi";
        else
            cout << "Iroh";
        cout << endl;
    }
    assert(cin);
}
