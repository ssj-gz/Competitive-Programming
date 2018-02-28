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
        if (cityHasTower[cityPos])
        {
            int nextCityWithTower = -1;
            // Finding the next city with a tower looks O(numCities) (which would make the whole algorithm O(numCities^2)),
            // but it gets amortised to O(1) when taken over all cityPos that have a tower.
            for (int i = cityPos + 1; i < numCities; i++)
            {
                if (cityHasTower[i])
                {
                    nextCityWithTower = i;
                    break;
                }
            }
            if (firstUncoveredCity < numCities && (nextCityWithTower == -1 || nextCityWithTower - towerRange > firstUncoveredCity))
            {
                // Need a tower here.
                numTowersSwitchedOn++;
                firstUncoveredCity = cityPos + towerRange + 1;
                //cout << "Switched on " << cityPos << " firstUncoveredCity is now: " << firstUncoveredCity << endl;
            }
        }
    }

    const auto coveredAllCities = (firstUncoveredCity >= numCities);
    if (coveredAllCities)
        cout << numTowersSwitchedOn << endl;
    else
        cout << -1 << endl;
}
