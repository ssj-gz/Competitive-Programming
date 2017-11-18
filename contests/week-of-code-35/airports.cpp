// Simon St James (ssjgz) 2017-11-18
// This is just a slow, brute-force approach to test correctness - it's much too slow to pass!
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <cassert>

using namespace std;

bool areAirportsArranged(const vector<int>& airports, int minDistance)
{
    set<int> reachedAirportIndices;
    vector<int> airportIndicesToExplore;
    airportIndicesToExplore.push_back(0);
    reachedAirportIndices.insert(0);

    while (!airportIndicesToExplore.empty())
    {
        vector<int> nextAirportIndicesToExplore;
        for (const auto& airportIndexToExplore : airportIndicesToExplore)
        {
            const auto airportPos = airports[airportIndexToExplore];
            for (int j = 0; j < airports.size(); j++)
            {
                if (abs(airports[j] - airportPos) >= minDistance)
                {
                    if (reachedAirportIndices.find(j) == reachedAirportIndices.end())
                    {
                        reachedAirportIndices.insert(j);
                        nextAirportIndicesToExplore.push_back(j);
                    }
                }
            }
        }
        airportIndicesToExplore = nextAirportIndicesToExplore;
    }

    const auto areArranged = reachedAirportIndices.size() == airports.size();

    return areArranged;
}

bool canArrangeAirportsWithCost(vector<int>& airports, int nextAirportIndex, int cost, int minDistance)
{
    if (cost <= 0)
    {
        return false;
    }
    if (nextAirportIndex == airports.size())
    {
        return areAirportsArranged(airports, minDistance);
    }
    const int originalPos = airports[nextAirportIndex];
    for (int newAirportPos = originalPos - cost; newAirportPos <= originalPos + cost; newAirportPos++)
    {
        const int costOfThisMove = abs(newAirportPos - originalPos);
        airports[nextAirportIndex] = newAirportPos;

        if (canArrangeAirportsWithCost(airports, nextAirportIndex + 1, cost - costOfThisMove, minDistance))
        {
            return true;
        }
    }

    return false;
}

bool canArrangeAirportsWithCost(const vector<int>& airports, int cost, int minDistance)
{
    vector<int> airportsCopy(airports);
    return canArrangeAirportsWithCost(airportsCopy, 0, cost, minDistance);
}

vector<int> findResultBruteForce(const vector<int>& airportAddedOnDay, int minDistance)
{
    const int numDays = airportAddedOnDay.size();

    vector<int> results;

    vector<int> airportsSorted;
    for (int day = 0; day < numDays; day++)
    {
        airportsSorted.push_back(airportAddedOnDay[day]);
        sort(airportsSorted.begin(), airportsSorted.end());

        int cost = 0;
        while (true)
        {
            if (canArrangeAirportsWithCost(airportsSorted, cost, minDistance))
            {
                results.push_back(cost);
                break;
            }
            cost++;
        }
    }
    return results;
}

int main()
{
    int Q;
    cin >> Q;
    for (int q = 0; q < Q; q++)
    { 
        int numDays, minDistance;

        cin >> numDays >> minDistance;

        vector<int> airportAddedOnDay(numDays);

        for (int i = 0; i < numDays; i++)
        {
            cin >> airportAddedOnDay[i];
        }

        const auto resultsBruteForce = findResultBruteForce(airportAddedOnDay, minDistance);
        for (const auto x : resultsBruteForce)
        {
            cout << x << " ";
        }
        cout << endl;
    }
}
