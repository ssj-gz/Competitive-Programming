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

vector<int> findResult(const vector<int>& airportAddedOnDay, int minDistance)
{
    cout << "findResult - minDistance: " << minDistance << endl;
    vector<int> results;
    vector<int> airports;
    for (int i = 0; i < airportAddedOnDay.size(); i++)
    {
        int leftEndpoint = numeric_limits<int>::max();
        int rightEndpoint = numeric_limits<int>::min();
        int leftEndPointIndex = -1;
        int rightEndPointIndex = -1;

        const int airportPos = airportAddedOnDay[i];
        airports.push_back(airportPos);
        sort(airports.begin(), airports.end());

        cout << "day " << i << " airports: " << endl;
        for (const auto x : airports)
        {
            cout << x << " ";
        }
        cout << endl;

        for (int j = 0; j < airports.size(); j++)
        {
            const int airportPos = airports[j];

            if (airportPos < leftEndpoint)
            {
                leftEndpoint = airportPos;
                leftEndPointIndex = j;
            }
            if (airportPos > rightEndpoint)
            {
                rightEndpoint = airportPos;
                rightEndPointIndex = j;
            }
        }
        cout << " leftEndpoint: " << leftEndpoint << " rightEndpoint: " << rightEndpoint << endl;
        const int midpoint = (rightEndpoint + leftEndpoint) / 2;
        const bool oddMidpoint = (((rightEndpoint - leftEndpoint) % 2) == 0);
        
        bool hasAirportNotReachableByEndPoints = false;

        int minAirportNotReachable = numeric_limits<int>::max();
        int maxAirportNotReachable = numeric_limits<int>::min();

        for (int j = 0; j < airports.size(); j++)
        {
            const int unreachableAirPort = airports[j];
            if (j == leftEndPointIndex || j == rightEndPointIndex)
                continue;
            if (unreachableAirPort < leftEndpoint + minDistance && unreachableAirPort > rightEndpoint - minDistance)
            {
                cout << " airport: " << unreachableAirPort << " is not reachable from or equal to either endpoint" << endl;
                hasAirportNotReachableByEndPoints = true;
                minAirportNotReachable = min(minAirportNotReachable, unreachableAirPort);
                maxAirportNotReachable = max(maxAirportNotReachable, unreachableAirPort);
            }
        }
        int adjustLeftBy = 0;
        int adjustRightBy = 0;
        cout << " minAirportNotReachable: " << minAirportNotReachable << " maxAirportNotReachable: " << maxAirportNotReachable << endl;
        cout << "midpoint: " << midpoint << " oddMidpoint: " << oddMidpoint << endl;
        if (hasAirportNotReachableByEndPoints)
        {
            if (!oddMidpoint)
            {
                if (maxAirportNotReachable < midpoint)
                {
                    adjustRightBy = max(adjustRightBy, maxAirportNotReachable - (rightEndpoint - minDistance));
                }
                else if (minAirportNotReachable >= midpoint)
                {
                    adjustLeftBy = max(adjustLeftBy, (leftEndpoint + minDistance) - minAirportNotReachable);
                }
                else
                {
                    adjustLeftBy = max(adjustLeftBy, (leftEndpoint + minDistance) - maxAirportNotReachable);
                    adjustRightBy = max(adjustRightBy, minAirportNotReachable - (rightEndpoint - minDistance));
                }
            }
            else
            {
                if (minAirportNotReachable == maxAirportNotReachable)
                {
                    adjustRightBy = max(adjustRightBy, minAirportNotReachable - (rightEndpoint - minDistance));
                }
                else
                {
                    if (maxAirportNotReachable <= midpoint)
                    {
                        adjustRightBy = max(adjustRightBy, maxAirportNotReachable - (rightEndpoint - minDistance));
                    }
                    else if (minAirportNotReachable > midpoint)
                    {
                        adjustLeftBy = max(adjustLeftBy, (leftEndpoint + minDistance) - minAirportNotReachable);
                    }
                    else
                    {
                        adjustLeftBy = max(adjustLeftBy, (leftEndpoint + minDistance) - maxAirportNotReachable);
                        adjustRightBy = max(adjustRightBy, minAirportNotReachable - (rightEndpoint - minDistance));
                    }
                }
            }
        }
        cout << " adjustLeftBy: " << adjustLeftBy << " adjustRightBy: " << adjustRightBy << endl;
        const int newLeftEndpoint = leftEndpoint - adjustLeftBy;
        const int newRightEndpoint = rightEndpoint + adjustRightBy;
        if (newRightEndpoint - newLeftEndpoint < minDistance && (leftEndPointIndex != rightEndPointIndex))
        {
            adjustRightBy += leftEndpoint - (rightEndpoint - minDistance);
            cout << " further adjustment needed adjustRightBy now: " << adjustRightBy << endl;
        } 
        else
        {
            cout << " no further adjustRightBy needed" << endl;
        }
        const int cost = adjustRightBy + adjustLeftBy;
        cout << cost << endl;
        results.push_back(cost);
    }
    return results;
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
#if 0
                cout << "day: " << day << " minDistance: " << minDistance << " can arrange with cost: " << cost << endl;
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
#endif
                results.push_back(cost);
                break;
            }
            cost++;
        }
    }
    return results;
}

void compareBruteForce(const vector<int>& airportAddedOnDay, int minDistance)
{
    const auto result = findResult(airportAddedOnDay, minDistance);
    const auto resultsBruteForce = findResultBruteForce(airportAddedOnDay, minDistance);
    const int numDays = airportAddedOnDay.size();
    if (resultsBruteForce != result)
    {
        cout << "whoops:" << endl;
        for (int i = 0; i < numDays; i++)
        {
            cout << "i: " << i << " result[i]: " << result[i] << " resultsBruteForce[i]: " << resultsBruteForce[i] << endl;
        }
    }
    assert(result == resultsBruteForce);
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
        compareBruteForce(airportAddedOnDay, minDistance);
        //const auto resultsBruteForce = findResultBruteForce(airportAddedOnDay, minDistance);
        //cout << "testcaes with " << numAirports << " airports" << endl;
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


#ifdef BRUTE_FORCE
        compareBruteForce(airportAddedOnDay, minDistance);
#else
        const auto result = findResult(airportAddedOnDay, minDistance);
        cout << result << endl;
#endif
    }
}
