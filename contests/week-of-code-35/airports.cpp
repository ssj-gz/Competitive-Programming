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
    //cout << "findResult - minDistance: " << minDistance << endl;
    vector<int> results;
    vector<int> airports;
    int leftEndpoint = numeric_limits<int>::max();
    int rightEndpoint = numeric_limits<int>::min();
    int leftEndPointIndex = -1;
    int rightEndPointIndex = -1;
    bool leftEndPointDuplicated = false;
    bool rightEndPointDuplicated = false;
    set<int> airportsNotCovered;
    for (int i = 0; i < airportAddedOnDay.size(); i++)
    {

        const int airportPos = airportAddedOnDay[i];
        airports.push_back(airportPos);
        sort(airports.begin(), airports.end());

        if (airportPos == leftEndpoint)
            leftEndPointDuplicated = true;
        if (airportPos == rightEndpoint)
            rightEndPointDuplicated = true;

        bool endpointChanged = false;
        if (airportPos < leftEndpoint)
        {
            leftEndpoint = airportPos;
            leftEndPointIndex = i;
            leftEndPointDuplicated = false;
            endpointChanged = true;
        }
        if (airportPos > rightEndpoint)
        {
            rightEndpoint = airportPos;
            rightEndPointIndex = i;
            rightEndPointDuplicated = false;
            endpointChanged = true;
        }

        //if (!endpointChanged)
        {
            if ((airportPos != leftEndpoint || leftEndPointDuplicated) && (airportPos != rightEndpoint || rightEndPointDuplicated) &&
                (airportPos < leftEndpoint + minDistance && airportPos > rightEndpoint - minDistance)
                    )
            {
                cout << "leftEndpoint: " << leftEndpoint << " rightEndpoint: " << rightEndpoint << " airportPos: " << airportPos << " leftEndPointDuplicated: " << leftEndPointDuplicated << " rightEndPointDuplicated: " <<rightEndPointDuplicated << " minDistance: " << minDistance << endl;
                airportsNotCovered.insert(airportPos);
            }
        }

#if 0
        cout << "day " << i << " airports: " << endl;
        for (const auto x : airports)
        {
            cout << x << " ";
        }
        cout << endl;
#endif

        int dbgLeftEndpoint = numeric_limits<int>::max();
        int dbgRightEndpoint = numeric_limits<int>::min();
        int dbgLeftEndPointIndex = -1;
        int dbgRightEndPointIndex = -1;
        for (int j = 0; j < airports.size(); j++)
        {
            const int airportPos = airports[j];

            if (airportPos < dbgLeftEndpoint)
            {
                dbgLeftEndpoint = airportPos;
                dbgLeftEndPointIndex = j;
            }
            if (airportPos > dbgRightEndpoint)
            {
                dbgRightEndpoint = airportPos;
                dbgRightEndPointIndex = j;
            }
        }
         const int leftEndpoint = dbgLeftEndpoint;
         const int rightEndpoint = dbgRightEndpoint;
         const int leftEndPointIndex = dbgLeftEndPointIndex;
         const int rightEndPointIndex = dbgRightEndPointIndex;

        assert(leftEndpoint == dbgLeftEndpoint && rightEndpoint == dbgRightEndpoint);
        assert((leftEndPointIndex == rightEndPointIndex) ==  (dbgLeftEndPointIndex == dbgRightEndPointIndex));
        //cout << " leftEndpoint: " << leftEndpoint << " rightEndpoint: " << rightEndpoint << endl;
        const int midpoint = (rightEndpoint + leftEndpoint) / 2;
        const bool oddMidpoint = (((rightEndpoint - leftEndpoint) % 2) == 0);

        bool hasAirportNotReachableByEndPoints = false;

        //int minAirportNotReachable = numeric_limits<int>::max();
        //int maxAirportNotReachable = numeric_limits<int>::min();

        //int adjustLeftBy = 0;
        //int adjustRightBy = 0;

        int bestAdjustedLeftBy = 0;
        int bestAdjustedRightBy = 0;
        int bestCost = numeric_limits<int>::max();

        vector<int> unreachableAirports;
        for (int j = 0; j < airports.size(); j++)
        {
            const int unreachableAirPort = airports[j];
            if (j == dbgLeftEndPointIndex || j == dbgRightEndPointIndex)
                continue;
            if (unreachableAirPort < leftEndpoint + minDistance && unreachableAirPort > rightEndpoint - minDistance)
            {
                unreachableAirports.push_back(unreachableAirPort);
            }
        }
        if (!endpointChanged)
        {
            set<int> dbgUnreachableAirports(unreachableAirports.begin(), unreachableAirports.end());
            cout << "airportsNotCovered: " << endl;
            for (const auto x : airportsNotCovered)
            {
                cout << x << " ";
            }
            cout << endl;
            cout << "dbgUnreachableAirports: " << endl;
            for (const auto x : dbgUnreachableAirports)
            {
                cout << x << " ";
            }
            cout << endl;
            assert(airportsNotCovered == dbgUnreachableAirports);
        }
        else
        {
            airportsNotCovered = set<int>(unreachableAirports.begin(), unreachableAirports.end());
        }
        for (int j = 0; j < unreachableAirports.size(); j++)
        {
            const int unreachableAirPort = unreachableAirports[j];
            //cout << " airport: " << unreachableAirPort << " is not reachable from or equal to either endpoint; midpoint " << midpoint << " oddMidpoint? " << oddMidpoint << endl;
            hasAirportNotReachableByEndPoints = true;
            //minAirportNotReachable = min(minAirportNotReachable, unreachableAirPort);
            //maxAirportNotReachable = max(maxAirportNotReachable, unreachableAirPort);
            int adjustLeftBy = (leftEndpoint + minDistance) - unreachableAirPort;
            int adjustRightBy = 0;
            if (j != 0)
            {
                const int previousUnreachable = unreachableAirports[j - 1];
                adjustRightBy = previousUnreachable - (rightEndpoint - minDistance);
            }
            //cout << " adjustLeftBy: " << adjustLeftBy << " adjustRightBy: " << adjustRightBy << " cost: " << (adjustLeftBy + adjustRightBy) << endl;
            if (adjustLeftBy + adjustRightBy <= bestCost)
            {
                bestCost = adjustLeftBy + adjustRightBy;
                bestAdjustedLeftBy = adjustLeftBy;
                bestAdjustedRightBy = adjustRightBy;
                //cout << "Updated best cost: " << bestCost << endl;
            }
            assert(bestAdjustedLeftBy >= 0 && bestAdjustedRightBy >= 0);
        }
        if (!unreachableAirports.empty())
        {
            int adjustRightBy = unreachableAirports.back() - (rightEndpoint - minDistance);
            int adjustLeftBy = 0;
            if (adjustLeftBy + adjustRightBy <= bestCost)
            {
                bestCost = adjustLeftBy + adjustRightBy;
                bestAdjustedLeftBy = adjustLeftBy;
                bestAdjustedRightBy = adjustRightBy;
                //cout << "Updated best cost: " << bestCost << endl;
            }
        }

#if 0
        if (!oddMidpoint)
        {
            if (unreachableAirPort < midpoint)
            {
                adjustRightBy = max(adjustRightBy, unreachableAirPort - (rightEndpoint - minDistance));
            }
            else
            {
                adjustLeftBy = max(adjustLeftBy, (leftEndpoint + minDistance) - unreachableAirPort);
            }
        }
        else
        {
            if (unreachableAirPort <= midpoint)
            {
                adjustRightBy = max(adjustRightBy, unreachableAirPort - (rightEndpoint - minDistance));
            }
            else 
            {
                adjustLeftBy = max(adjustLeftBy, (leftEndpoint + minDistance) - unreachableAirPort);
            }
        }
#endif
#if 0
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
#endif
        //cout << " bestAdjustedLeftBy: " << bestAdjustedLeftBy << " bestAdjustedRightBy: " << bestAdjustedRightBy << endl;
        const int newLeftEndpoint = leftEndpoint - bestAdjustedLeftBy;
        const int newRightEndpoint = rightEndpoint + bestAdjustedRightBy;
        assert(bestAdjustedLeftBy >= 0 && bestAdjustedRightBy >= 0);
        if (newRightEndpoint - newLeftEndpoint < minDistance && (leftEndPointIndex != rightEndPointIndex))
        {
            bestAdjustedRightBy += leftEndpoint - (rightEndpoint - minDistance);
            //cout << " further adjustment needed bestAdjustedRightBy now: " << bestAdjustedRightBy << endl;
        } 
        else
        {
            //cout << " no further adjustRightBy needed" << endl;
        }
        const int cost = bestAdjustedLeftBy + bestAdjustedRightBy;
        //cout << cost << endl;
        results.push_back(cost);
    }
    return results;
}

vector<int> findAirportArrangementWithCostEndpointsOnly(const vector<int>& airports, int cost, int minDistance)
{
    vector<int> airportsCopy(airports);
    int leftEndpoint = airportsCopy.front();
    int rightEndpoint = airportsCopy.back();

    for (int adjustLeftBy = 0; adjustLeftBy <= cost; adjustLeftBy++)
    {
        airportsCopy[0] = airports[0] - adjustLeftBy;
        for (int adjustRightBy = 0; adjustLeftBy + adjustRightBy <= cost; adjustRightBy++)
        {
            const int totalCost = adjustLeftBy + adjustRightBy;
            assert(totalCost <= cost);
            airportsCopy.back() = airports.back() + adjustRightBy;
            if (areAirportsArranged(airportsCopy, minDistance))
            {
                return airportsCopy;
            }
        }
    }
    return vector<int>();
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
        //cout << "brute force day: " << day << " of " << numDays << endl;
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
            //const auto arrangedAirportsMovingOnlyEnds = findAirportArrangementWithCost(airportsSorted, cost, minDistance, true);
            //assert(findAirportArrangementWithCostEndpointsOnly(airportsSorted, cost, minDistance).size() == arrangedAirportsMovingOnlyEnds.size());
            const auto arrangedAirportsMovingOnlyEnds = findAirportArrangementWithCostEndpointsOnly(airportsSorted, cost, minDistance);
            //const auto arrangedAirportsMovingAny = findAirportArrangementWithCost(airportsSorted, cost, minDistance, false);
            //assert(arrangedAirportsMovingOnlyEnds.size() == arrangedAirportsMovingAny.size());
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
        assert(result == resultsBruteForce);
    }
    else
    {
        cout << "Success with " << airportAddedOnDay.size() << " days and minDistance: " << minDistance << endl;
    }
}

int main(int argc, char** argv)
{
    if (argc == 2)
    {
        srand(time(0));
        const int numQueries = rand() % 100;
        cout << numQueries << endl;
        for (int q = 0; q < numQueries; q++)
        {
            const int numAirports = rand() % 1000 + 3;
            const int minDistance = rand() % 100;
            const int airportRange = 1000;

            vector<int> airportAddedOnDay;
            cout << numAirports << " " << minDistance << endl;
            for (int i = 0; i < numAirports; i++)
            {
                cout << (rand() % (2 * airportRange + 1) - airportRange) << " ";
            }
        }
        return 0;
    }
#ifdef RANDOM
    srand(time(0));
    while (true)
    {
        const int maxAirports = 20;
        const int minAirports = 5;
        const int airportRange = 10;
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
        for (const auto costForDay : result)
        {
            cout << costForDay << " ";
        }
        cout << endl;
#endif
    }
}
