// Simon St James (ssjgz) 2017-11-18
// This is just a slow, brute-force approach to test correctness - it's much too slow to pass!
#define RANDOM
#define BRUTE_FORCE
//#define SUBMISSION
#ifdef SUBMISSION
#undef RANDOM
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <cassert>

using namespace std;

bool areAirportsArranged(const vector<int>& airports, int minDistance)
{
    //cout << "areAirportsArranged; minDistance: " << minDistance << " airports:" << endl;
    //for (const auto x : airports)
    //{
        //cout << x << " ";
    //}
    //cout << endl;
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
    //cout << "areArranged? " << areArranged << endl;

    return areArranged;
}

bool canArrangeAirportsWithCost(vector<int>& airports, int nextAirportIndex, int cost, int minDistance)
{
    if (cost < 0)
    {
        return false;
    }
    if (nextAirportIndex == airports.size() || cost == 0)
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
            //cout << "Can arrange airports with cost: " << cost << endl;
            return true;
        }
    }
    airports[nextAirportIndex] = originalPos;

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
        //cout << "day: " << day << " airports: " <<  endl;
        //for (const auto x : airportsSorted)
        //{
            //cout << x << " ";
        //}
        //cout << endl;

        int cost = 0;
        while (true)
        {
            if (canArrangeAirportsWithCost(airportsSorted, cost, minDistance))
            {
                //cout << "day: " << day << " can arrange with cost: " << cost << endl;
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
#ifdef RANDOM
    while (true)
    {
        srand(time(0));
        const int maxAirports = 10;
        const int airportRange = 30;
        const int numAirports = (rand() % (maxAirports - 3)) + 3;
        const int minDistance = rand() % 10;
        vector<int> airportAddedOnDay;
        for (int i = 0; i < numAirports; i++)
        {
            airportAddedOnDay.push_back(rand() % (2 * airportRange + 1) - airportRange);
        }
        findResultBruteForce(airportAddedOnDay, minDistance);
    }
#endif
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
