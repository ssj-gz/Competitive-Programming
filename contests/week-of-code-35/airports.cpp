// Simon St James (ssjgz) 2017-11-18
#define SUBMISSION
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
    auto hasNext = [](const set<int>& collection, set<int>::iterator setIter)
    {
        if (setIter == collection.end())
            return false;
        setIter++;
        if (setIter == collection.end())
            return false;
        return true;
    };
    auto hasPrevious = [](const set<int>& collection, set<int>::iterator setIter)
    {
        if (collection.empty())
            return false;
        if (setIter == collection.begin())
            return false;
        return true;
    };
    auto next = [&hasNext](const set<int>& collection, set<int>::iterator setIter)
    {
        assert(hasNext(collection, setIter));
        setIter++;
        return *setIter;
    };
    auto previous = [&hasPrevious](const set<int>& collection, set<int>::iterator setIter)
    {
        assert(hasPrevious(collection, setIter));
        setIter--;
        return *setIter;
    };
    set<int> airportsNotCovered;
    for (int i = 0; i < airportAddedOnDay.size(); i++)
    {

        const int airportPos = airportAddedOnDay[i];

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

        if ((airportPos != leftEndpoint || leftEndPointDuplicated) && (airportPos != rightEndpoint || rightEndPointDuplicated) &&
            (isUncovered(airportPos))
                )
        {
            newlyUncoveredAirportPositions.push_back(airportPos);
        }
            //cout << "end points changed - removing stuff from set!" << endl;
            if (oldRightEndPoint != rightEndpoint)
            {
                //cout << "right end point changed" << endl;
                //assert(rightEndpoint == oldRightEndPoint);
                while (!airportsNotCovered.empty() && *airportsNotCovered.begin() <= rightEndpoint - minDistance)
                {
                    //cout << "removing: " << *airportsNotCovered.begin() << " due to change right end point" << endl;
                    if (hasNext(airportsNotCovered, airportsNotCovered.begin()))
                    {
                        const int diffToRemove = next(airportsNotCovered, airportsNotCovered.begin()) -  *airportsNotCovered.begin();
                        //cout << " removing diff: " << diffToRemove << " due to change right end point";
                        maxDiffOfSuccessiveUncoveredPairs.remove(diffToRemove);
                    }
                    airportsNotCovered.erase(airportsNotCovered.begin());
                }
                if ((i != 0) && (/*oldRightEndPointDuplicated && */ isUncovered(oldRightEndPoint) && oldRightEndPoint != leftEndpoint))
                {
                    //cout << "adding oldRightEndPoint: " << oldRightEndPoint << " to airportsNotCovered oldRightEndPointDuplicated: " << oldRightEndPointDuplicated<< endl;
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
                //cout << "left end point changed" << endl;
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
                            //cout << "removing: " << *highestIter << " due to change left end point" << endl;
                            if (hasPrevious(airportsNotCovered, highestIter))
                            {
                                const int diffToRemove = *highestIter - previous(airportsNotCovered, highestIter);
                                //cout << " removing diff: " << diffToRemove << " due to change left end point";
                                maxDiffOfSuccessiveUncoveredPairs.remove(diffToRemove);
                            }

                            highestIter = airportsNotCovered.erase(highestIter);
                            if (!airportsNotCovered.empty())
                                highestIter--;
                        }
                        else
                        {
                            //cout << "Not removing " << *highestIter << " due to left end point change" << endl;
                            break;
                        }
                    }
                }
                //assert(leftEndpoint == oldLeftEndPoint);
                if ((i != 0) && (/*oldLeftEndPointDuplicated &&*/ isUncovered(oldLeftEndPoint) && (oldLeftEndPoint != rightEndpoint)))
                {
                    //cout << "adding oldLeftEndPoint: " << oldLeftEndPoint << " to airportsNotCovered oldLeftEndPointDuplicated: " << oldLeftEndPointDuplicated  << endl;
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
                    if (hasLeft && hasRight)
                    {
                        // Broken up a previously successive pair; remove their contribution.
                        maxDiffOfSuccessiveUncoveredPairs.remove(right - left);
                    } 
                    if (hasLeft)
                    {
                        maxDiffOfSuccessiveUncoveredPairs.add(uncoveredAirportPos - left);
                    }
                    if (hasRight)
                    {
                        maxDiffOfSuccessiveUncoveredPairs.add(right - uncoveredAirportPos);
                    }
                }
            }


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
            if (!maxDiffOfSuccessiveUncoveredPairs.empty())
            {
                bestCostForInner = min(bestCostForInner,  leftEndpoint - rightEndpoint + 2 * minDistance - maxDiffOfSuccessiveUncoveredPairs.max());
            }
        }

        int cost = bestCostForInner;
        if (rightEndpoint - leftEndpoint + bestCostForInner < minDistance && (leftEndPointIndex != rightEndPointIndex))
        {
            cost += leftEndpoint - (rightEndpoint - minDistance);
        }
        results.push_back(cost);
    }
    return results;
}


int main(int argc, char** argv)
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


        const auto result = findResult(airportAddedOnDay, minDistance);
        for (const auto costForDay : result)
        {
            cout << costForDay << " ";
        }
        cout << endl;
    }
}
