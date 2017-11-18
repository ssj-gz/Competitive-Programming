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

vector<int> findAirportArrangementWithCost(vector<int>& airports, int nextAirportIndex, int cost, int minDistance, bool moveOnlyFirstAndLast)
{
    if (cost < 0)
    {
        return vector<int>();
    }
    if (nextAirportIndex == airports.size())
    {
        if (areAirportsArranged(airports, minDistance))
        {
            return airports;
        }
        return vector<int>();
    }
    if (!moveOnlyFirstAndLast || nextAirportIndex == 0 || nextAirportIndex == airports.size() - 1)
    {
        const int originalPos = airports[nextAirportIndex];
        for (int newAirportPos = originalPos - cost; newAirportPos <= originalPos + cost; newAirportPos++)
        {
            const int costOfThisMove = abs(newAirportPos - originalPos);
            airports[nextAirportIndex] = newAirportPos;

            const auto arrangedAirports = (findAirportArrangementWithCost(airports, nextAirportIndex + 1, cost - costOfThisMove, minDistance, moveOnlyFirstAndLast));
            if ((!arrangedAirports.empty()))
            {
                //cout << "Can arrange airports with cost: " << cost << endl;
                return arrangedAirports;
            }
        }
        airports[nextAirportIndex] = originalPos;
    }
    else
    {
        return findAirportArrangementWithCost(airports, nextAirportIndex + 1, cost, minDistance, moveOnlyFirstAndLast);
    }

    return vector<int>();
}

vector<int> findAirportArrangementWithCost(const vector<int>& airports, int cost, int minDistance, bool moveOnlyFirstAndLast)
{
    vector<int> airportsCopy(airports);
    return findAirportArrangementWithCost(airportsCopy, 0, cost, minDistance, moveOnlyFirstAndLast);
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
#if 0
        cout << "day: " << day << " airports: " <<  endl;
        for (const auto x : airportsSorted)
        {
            cout << x << " ";
        }
        cout << endl;
#endif

        int cost = 0;
        while (true)
        {
            const auto arrangedAirportsMovingOnlyEnds = findAirportArrangementWithCost(airportsSorted, cost, minDistance, true);
            const auto arrangedAirportsMovingAny = findAirportArrangementWithCost(airportsSorted, cost, minDistance, false);
            assert(arrangedAirportsMovingOnlyEnds.size() == arrangedAirportsMovingAny.size());
            //cout << "same cost" << endl;
            const auto arrangedAirports = arrangedAirportsMovingOnlyEnds;
            if (!arrangedAirports.empty())
            {
                if (arrangedAirports.size() > 5 && cost > 0)
                {
                    int derangements = 0;
                    for (int i = 1; i < airportsSorted.size() - 1; i++)
                    {
                        if (arrangedAirports[i] != airportsSorted[i])
                        {
                            derangements++;
                            break;
                        }
                    }
                    if (derangements > 0)
                    {
                        cout << "day: " << day << " minDistance: " << minDistance << " can arrange with cost: " << cost << " derangements: " << derangements << endl;
                        cout << "Arrangement: " << endl;
                        for (const auto x : arrangedAirports)
                        {
                            cout << x << " ";
                        }
                        cout << endl;
                        cout << "original: "<< endl;
                        for (const auto x : airportsSorted)
                        {
                            cout << x << " ";
                        }
                        cout << endl;
                        assert(derangements <= 1);
                    }
                }
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
    srand(time(0));
    while (true)
    {
        const int maxAirports = 20;
        const int minAirports = 5;
        const int airportRange = 45;
        const int numAirports = (rand() % (maxAirports - minAirports)) + minAirports;
        const int minDistance = rand() % 25;
        vector<int> airportAddedOnDay;
        for (int i = 0; i < numAirports; i++)
        {
            airportAddedOnDay.push_back(rand() % (2 * airportRange + 1) - airportRange);
        }
        const auto resultsBruteForce = findResultBruteForce(airportAddedOnDay, minDistance);
        cout << "testcaes with " << numAirports << " airports" << endl;
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
