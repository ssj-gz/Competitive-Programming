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
#include <map>
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

class MaxTracker
{
    public:
        void add(int n)
        {
            m_countOf[n]++;
        }
        void remove(int n)
        {
            assert(m_countOf.find(n) != m_countOf.end());
            m_countOf[n]--;
            if (m_countOf[n] == 0)
            {
                m_countOf.erase(m_countOf.find(n));
            }
        }
        bool empty() const
        {
            return m_countOf.empty();
        }
        int max() const
        {
            assert(!empty());
            auto highestIter = m_countOf.end();
            highestIter--;
            return highestIter->first;
        }
    private:
        map<int, int> m_countOf;

};

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

    int bestCostForInner = 0;

    MaxTracker maxDiffOfSuccessiveUncoveredPairs;

    auto isUncovered = [&leftEndpoint, &rightEndpoint, minDistance](const int airportPos)
    {
        return airportPos < leftEndpoint + minDistance && airportPos > rightEndpoint - minDistance;
    };
    auto hasNext = [](const set<int>& collection, auto setIter)
    {
        if (setIter == collection.end())
            return false;
        setIter++;
        if (setIter == collection.end())
            return false;
        return true;
    };
    auto hasPrevious = [](const set<int>& collection, auto setIter)
    {
        if (collection.empty())
            return false;
        if (setIter == collection.begin())
            return false;
        return true;
    };
    auto next = [&hasNext](const set<int>& collection, auto setIter)
    {
        assert(hasNext(collection, setIter));
        setIter++;
        return *setIter;
    };
    auto previous = [&hasPrevious](const set<int>& collection, auto setIter)
    {
        assert(hasPrevious(collection, setIter));
        setIter--;
        return *setIter;
    };
    set<int> airportsNotCovered;
    for (int i = 0; i < airportAddedOnDay.size(); i++)
    {

        const int airportPos = airportAddedOnDay[i];
#ifdef BRUTE_FORCE
        airports.push_back(airportPos);
        sort(airports.begin(), airports.end());

        cout << "airports: " << endl;
        for (const auto x : airports)
        {
            cout << x << " ";
        }
        cout << endl;
#endif

        if (airportPos == leftEndpoint)
            leftEndPointDuplicated = true;
        if (airportPos == rightEndpoint)
            rightEndPointDuplicated = true;

        const auto oldLeftEndPoint = leftEndpoint;
        const auto oldRightEndPoint = rightEndpoint;
        const auto oldLeftEndPointDuplicated = leftEndPointDuplicated;
        const auto oldRightEndPointDuplicated = rightEndPointDuplicated;
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

        vector<int> newlyUncoveredAirportPositions;

        cout << "leftEndpoint: " << leftEndpoint << " rightEndpoint: " << rightEndpoint << " airportPos: " << airportPos << " leftEndPointDuplicated: " << leftEndPointDuplicated << " rightEndPointDuplicated: " <<rightEndPointDuplicated << " minDistance: " << minDistance << endl;
        //if (!endpointChanged)
        {
            if ((airportPos != leftEndpoint || leftEndPointDuplicated) && (airportPos != rightEndpoint || rightEndPointDuplicated) &&
                (isUncovered(airportPos))
                    )
            {
                cout << "adding " << airportPos << " to airportsNotCovered" << endl;
                newlyUncoveredAirportPositions.push_back(airportPos);
            }
        }
        //else
        {
            cout << "end points changed - removing stuff from set!" << endl;
            if (oldRightEndPoint != rightEndpoint)
            {
                cout << "right end point changed" << endl;
                //assert(rightEndpoint == oldRightEndPoint);
                while (!airportsNotCovered.empty() && *airportsNotCovered.begin() <= rightEndpoint - minDistance)
                {
                    cout << "removing: " << *airportsNotCovered.begin() << " due to change right end point" << endl;
                    if (hasNext(airportsNotCovered, airportsNotCovered.begin()))
                    {
                        const int diffToRemove = next(airportsNotCovered, airportsNotCovered.begin()) -  *airportsNotCovered.begin();
                        cout << " removing diff: " << diffToRemove << " due to change right end point";
                        maxDiffOfSuccessiveUncoveredPairs.remove(diffToRemove);
                    }
                    airportsNotCovered.erase(airportsNotCovered.begin());
                }
                if ((i != 0) && (/*oldRightEndPointDuplicated && */ isUncovered(oldRightEndPoint) && oldRightEndPoint != leftEndpoint))
                {
                    cout << "adding oldRightEndPoint: " << oldRightEndPoint << " to airportsNotCovered oldRightEndPointDuplicated: " << oldRightEndPointDuplicated<< endl;
                    newlyUncoveredAirportPositions.push_back(oldRightEndPoint);
                }
                if (oldRightEndPoint != rightEndpoint && !airportsNotCovered.empty())
                {
                    const int reduceBy = rightEndpoint - oldRightEndPoint;
                    //bestCostForInner -= reduceBy;
                    //cout << " bestCostForInner reduced by " << reduceBy << " to " << bestCostForInner << " due to change in right endpoint" << endl;
                    //assert(bestCostForInner >= 0);
                }
            }
            if (oldLeftEndPoint != leftEndpoint)
            {
                cout << "left end point changed" << endl;
                if (!airportsNotCovered.empty())
                {
                    auto highestIter = airportsNotCovered.end();
                    highestIter--;
                    while (true)
                    {
                        if (airportsNotCovered.empty())
                            break;
                        if (*highestIter >= leftEndpoint + minDistance)
                        {
                            cout << "removing: " << *highestIter << " due to change left end point" << endl;
                            if (hasPrevious(airportsNotCovered, highestIter))
                            {
                                const int diffToRemove = *highestIter - previous(airportsNotCovered, highestIter);
                                cout << " removing diff: " << diffToRemove << " due to change left end point";
                                maxDiffOfSuccessiveUncoveredPairs.remove(diffToRemove);
                            }

                            highestIter = airportsNotCovered.erase(highestIter);
                            if (!airportsNotCovered.empty())
                                highestIter--;
                        }
                        else
                        {
                            cout << "Not removing " << *highestIter << " due to left end point change" << endl;
                            break;
                        }
                    }
                }
                //assert(leftEndpoint == oldLeftEndPoint);
                if ((i != 0) && (/*oldLeftEndPointDuplicated &&*/ isUncovered(oldLeftEndPoint) && (oldLeftEndPoint != rightEndpoint)))
                {
                    cout << "adding oldLeftEndPoint: " << oldLeftEndPoint << " to airportsNotCovered oldLeftEndPointDuplicated: " << oldLeftEndPointDuplicated  << endl;
                    newlyUncoveredAirportPositions.push_back(oldLeftEndPoint);
                }
                if (oldLeftEndPoint != leftEndpoint && !airportsNotCovered.empty())
                {
                    const int reduceBy = oldLeftEndPoint - leftEndpoint;
                    //bestCostForInner -= reduceBy;
                    //cout << " bestCostForInner reduced by " << reduceBy << " to " << bestCostForInner << " due to change in left endpoint" << endl;
                    //assert(bestCostForInner >= 0);
                }
            }

            for (const auto uncoveredAirportPos : newlyUncoveredAirportPositions)
            {
                if (airportsNotCovered.find(uncoveredAirportPos) == airportsNotCovered.end())
                {
                    airportsNotCovered.insert(uncoveredAirportPos);
                    auto newUncoveredIter = airportsNotCovered.find(uncoveredAirportPos);
                    const bool hasLeft = newUncoveredIter != airportsNotCovered.begin();
                    int left = -1;
                    if (hasLeft)
                    {
                        newUncoveredIter--;
                        left = *newUncoveredIter;
                        newUncoveredIter++;
                    }
                    newUncoveredIter++;
                    const bool hasRight = newUncoveredIter != airportsNotCovered.end();
                    int right = -1;
                    if (hasRight)
                    {
                        right = *newUncoveredIter;
                    }
                    cout << "** new uncoveredAirportPos: " << uncoveredAirportPos << " left: " << (hasLeft ? to_string(left) : "none") << " right: " << (hasRight ? to_string(right) : "none") << endl;
                    if (hasLeft && hasRight)
                    {
                        cout << " removing diff  " << (right - left) << " because of breaking stuff up!" << endl;
                        maxDiffOfSuccessiveUncoveredPairs.remove(right - left);
                    } 
                    if (hasLeft)
                    {
#if 0
                        const int adjustRightBy = left - (rightEndpoint - minDistance);
                        const int adjustLeftBy = (leftEndpoint + minDistance) - uncoveredAirportPos;
                        cout << "has left - adjustLeftBy: " << adjustLeftBy << " adjustRightBy: " << adjustRightBy << endl;
                        newBestCostForInner = min(newBestCostForInner, (adjustLeftBy + adjustRightBy));
#endif
                        maxDiffOfSuccessiveUncoveredPairs.add(uncoveredAirportPos - left);
                    }
#if 0
                    else
                    {
                        // uncoveredAirportPos is min.
                        const int adjustLeftBy = (leftEndpoint + minDistance) - uncoveredAirportPos;
                        newBestCostForInner = min(newBestCostForInner, adjustLeftBy);
                        cout << "is min - adjustLeftBy: " << adjustLeftBy << endl;
                    }
#endif
                    if (hasRight)
                    {
                        //const int adjustRightBy = uncoveredAirportPos - (rightEndpoint - minDistance);
                        //const int adjustLeftBy = (leftEndpoint + minDistance) - right;
                        //cout << "has right - adjustLeftBy: " << adjustLeftBy << " adjustRightBy: " << adjustRightBy << endl;
                        //newBestCostForInner = min(newBestCostForInner, (adjustLeftBy + adjustRightBy));
                        maxDiffOfSuccessiveUncoveredPairs.add(right - uncoveredAirportPos);
                    }
#if 0
                    else
                    {
                        // uncoveredAirportPos is largest.
                        const int adjustRightBy = uncoveredAirportPos - (rightEndpoint - minDistance);
                        newBestCostForInner = min(newBestCostForInner, adjustRightBy);
                        cout << "is max - adjustRightBy: " << adjustRightBy << endl;
                    }
#endif
                    //cout << "newBestCostForInner: " << newBestCostForInner << endl;
                    //bestCostForInner = min(bestCostForInner, newBestCostForInner);
                    //bestCostForInner = newBestCostForInner;
                }
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
        cout << "airportsNotCovered: " << endl;
        for (const auto x : airportsNotCovered)
        {
            cout << x << " ";
        }
        cout << endl;
        cout << "About to calculate bestCostForInner" << endl;
        bestCostForInner = 0;
        if (!airportsNotCovered.empty())
        {
            bestCostForInner = numeric_limits<int>::max();
            const int minThing = *airportsNotCovered.begin();
            auto blee = airportsNotCovered.end();
            blee--;
            const int maxThing = *blee;
            // If left endpoint extended to cover everything.
            bestCostForInner = min(bestCostForInner,  (leftEndpoint + minDistance) - minThing);
            // If right end point extended to cover everything.
            bestCostForInner = min(bestCostForInner,  maxThing - (rightEndpoint - minDistance));
            cout << " minThing: " << minThing << " maxThing: " << maxThing << endl;
            if (!maxDiffOfSuccessiveUncoveredPairs.empty())
            {
                bestCostForInner = min(bestCostForInner,  leftEndpoint - rightEndpoint + 2 * minDistance - maxDiffOfSuccessiveUncoveredPairs.max());
            }
        }

#ifdef BRUTE_FORCE
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
#endif
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

#ifdef BRUTE_FORCE
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
        if (true || !endpointChanged)
        {
            set<int> dbgUnreachableAirports(unreachableAirports.begin(), unreachableAirports.end());
#if 0
            cout << "airportsNotCovered: " << endl;
            for (const auto x : airportsNotCovered)
            {
                cout << x << " ";
            }
            cout << endl;
            cout << "dbgUnreachableAirports: " << endl;
            for (const auto x : unreachableAirports)
            {
                cout << x << " ";
            }
            cout << endl;
#endif
            assert(airportsNotCovered == dbgUnreachableAirports);
        }
        else
        {
            airportsNotCovered = set<int>(unreachableAirports.begin(), unreachableAirports.end());
        }
        unreachableAirports.erase(unique(unreachableAirports.begin(), unreachableAirports.end()), unreachableAirports.end());
#endif
#ifdef BRUTE_FORCE
        int numCostAdjustments = 0;
        int maxDiffOfThings = numeric_limits<int>::min();
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
                cout << "weee adjustLeftBy: " << adjustLeftBy << " adjustRightBy: " << adjustRightBy << " cost: " << (adjustLeftBy + adjustRightBy) << " previousUnreachable: " << previousUnreachable << " unreachable: " << unreachableAirPort << " leftEndpoint: " << leftEndpoint << " rightEndpoint: " << rightEndpoint << " minDistance: " << minDistance << " thing: " << (adjustRightBy + adjustLeftBy + (unreachableAirPort - previousUnreachable)) <<  endl;
                if (previousUnreachable == unreachableAirPort)
                {
                    assert(bestCost <= adjustLeftBy + adjustRightBy);
                }
                assert(adjustLeftBy + adjustRightBy == leftEndpoint - rightEndpoint + 2 * minDistance - (unreachableAirPort - previousUnreachable));
                maxDiffOfThings = max(maxDiffOfThings, unreachableAirPort - previousUnreachable);
                cout << "cost of this: " << (adjustLeftBy + adjustRightBy) << endl;
            }
            if (adjustLeftBy + adjustRightBy <= bestCost)
            {
                bestCost = adjustLeftBy + adjustRightBy;
                bestAdjustedLeftBy = adjustLeftBy;
                bestAdjustedRightBy = adjustRightBy;
                numCostAdjustments++;
                cout << "Updated best cost: " << bestCost << " numCostAdjustments: " << numCostAdjustments << endl;
            }
            assert(bestAdjustedLeftBy >= 0 && bestAdjustedRightBy >= 0);
        }
        cout << "maxDiffOfThings: " << maxDiffOfThings << " maxDiffOfSuccessiveUncoveredPairs: " << (maxDiffOfSuccessiveUncoveredPairs.empty() ? "none" : to_string(maxDiffOfSuccessiveUncoveredPairs.max())) << endl;
        assert((maxDiffOfThings == numeric_limits<int>::min() && maxDiffOfSuccessiveUncoveredPairs.empty()) || (maxDiffOfThings == maxDiffOfSuccessiveUncoveredPairs.max()));
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
#endif
        //cout << " bestAdjustedLeftBy: " << bestAdjustedLeftBy << " bestAdjustedRightBy: " << bestAdjustedRightBy << endl;
#ifdef BRUTE_FORCE
        const int newLeftEndpoint = leftEndpoint - bestAdjustedLeftBy;
        const int newRightEndpoint = rightEndpoint + bestAdjustedRightBy;
        const int dbgInnerCost = bestAdjustedRightBy + bestAdjustedLeftBy;
        cout << "dbgInnerCost: " << dbgInnerCost << " bestCostForInner: " << bestCostForInner << endl;
        assert(dbgInnerCost ==  bestCostForInner);
        assert(bestAdjustedLeftBy >= 0 && bestAdjustedRightBy >= 0);
#endif
#if 0
        if (newRightEndpoint - newLeftEndpoint < minDistance && (leftEndPointIndex != rightEndPointIndex))
        {
            bestAdjustedRightBy += leftEndpoint - (rightEndpoint - minDistance);
            //cout << " further adjustment needed bestAdjustedRightBy now: " << bestAdjustedRightBy << endl;
        } 
        else
        {
            //cout << " no further adjustRightBy needed" << endl;
        }
#endif
        int cost = bestCostForInner;
        if (rightEndpoint - leftEndpoint + bestCostForInner < minDistance && (leftEndPointIndex != rightEndPointIndex))
        {
            cost += leftEndpoint - (rightEndpoint - minDistance);
        }
        //const int cost = bestAdjustedLeftBy + bestAdjustedRightBy;
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
            const int numAirports = rand() % 100000 + 3;
            const int minDistance = rand() % 100000000;
            const int airportRange = 10000000;

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
        const int airportRange = 20;
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
