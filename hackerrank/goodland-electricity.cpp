// Simon St James (ssjgz) - 2018-02-28
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    int numCities;
    cin >> numCities;

    int towerRange;
    cin >> towerRange;
    towerRange--; // Make the range inclusive.

    vector<bool> cityHasTower(numCities);
    for (int i = 0; i < numCities; i++)
    {
        int numTowersInCity;
        cin >> numTowersInCity;

        cityHasTower[i] = (numTowersInCity > 0);
    }

    int firstUncoveredCity = 0;
    int numTowersSwitchedOn = 0;

    for (int cityPos = 0; cityPos < numCities; cityPos++)
    {
        if (firstUncoveredCity < numCities && (cityPos == numCities || (cityPos + 1) - towerRange > firstUncoveredCity))
        {
            // Need a tower here.
            numTowersSwitchedOn++;
            firstUncoveredCity = cityPos + towerRange + 1;
            //cout << "Switched on " << cityPos << " firstUncoveredCity is now: " << firstUncoveredCity << endl;
        }
    }

    cout << numTowersSwitchedOn << endl;
}
