// Simon St James (ssjgz) 2017-11-18
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <cassert>

using namespace std;

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

vector<int> findMinCostOfArrangementForDays(const vector<int>& airportAddedOnDay, int minDistance)
{
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
    // Some utils for dealing with set<> ugliness - though still just as ugly, I suppose!
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
        if (airportPos < leftEndpoint)
        {
            leftEndpoint = airportPos;
            leftEndPointIndex = i;
            leftEndPointDuplicated = false;
        }
        if (airportPos > rightEndpoint)
        {
            rightEndpoint = airportPos;
            rightEndPointIndex = i;
            rightEndPointDuplicated = false;
        }

        vector<int> newlyUncoveredAirportPositions;

        // Should we add this as an "uncovered" airport? There's a bit of finickiness with whether it's an endpoint: 
        // we should add it to uncovered if it is and there is a duplicate of it already in the set of airports.
        if ((airportPos != leftEndpoint || leftEndPointDuplicated) && (airportPos != rightEndpoint || rightEndPointDuplicated) &&
                (isUncovered(airportPos))
           )
        {
            newlyUncoveredAirportPositions.push_back(airportPos);
        }
        if (oldRightEndPoint != rightEndpoint)
        {
            // The right endpoint has been moved rightwards; erase the now-covered airports from the beginning of
            // airportsNotCovered (updating maxDiffOfSuccessiveUncoveredPairs as we go) ...
            while (!airportsNotCovered.empty() && *airportsNotCovered.begin() <= rightEndpoint - minDistance)
            {
                if (hasNext(airportsNotCovered, airportsNotCovered.begin()))
                {
                    const int diffToRemove = next(airportsNotCovered, airportsNotCovered.begin()) -  *airportsNotCovered.begin();
                    maxDiffOfSuccessiveUncoveredPairs.remove(diffToRemove);
                }
                airportsNotCovered.erase(airportsNotCovered.begin());
            }
            // ... and maybe add the previous endpoint as an uncovered airport.
            if ((i != 0) && (isUncovered(oldRightEndPoint) && oldRightEndPoint != leftEndpoint))
            {
                newlyUncoveredAirportPositions.push_back(oldRightEndPoint);
            }
        }
        if (oldLeftEndPoint != leftEndpoint)
        {
            // Left endpoint has been moved leftwards; erase the now-covered airports from the end of airportsNotCovered, etc.
            // Same as the right endpoint case, really, except dealing with iterators is more ugly!
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
                        if (hasPrevious(airportsNotCovered, highestIter))
                        {
                            const int diffToRemove = *highestIter - previous(airportsNotCovered, highestIter);
                            maxDiffOfSuccessiveUncoveredPairs.remove(diffToRemove);
                        }

                        highestIter = airportsNotCovered.erase(highestIter);
                        if (!airportsNotCovered.empty())
                            highestIter--;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            if ((i != 0) && (isUncovered(oldLeftEndPoint) && (oldLeftEndPoint != rightEndpoint)))
            {
                newlyUncoveredAirportPositions.push_back(oldLeftEndPoint);
            }
        }

        // Add the new uncovered airport positions to airportsNotCovered if they are not there already,
        // and keep maxDiffOfSuccessiveUncoveredPairs up-to-date.
        for (const auto uncoveredAirportPos : newlyUncoveredAirportPositions)
        {
            if (airportsNotCovered.find(uncoveredAirportPos) == airportsNotCovered.end())
            {
                airportsNotCovered.insert(uncoveredAirportPos);
                const auto newUncoveredIter = airportsNotCovered.find(uncoveredAirportPos);
                const bool hasLeft = hasPrevious(airportsNotCovered, newUncoveredIter);
                int left = -1;
                if (hasLeft)
                {
                    left = previous(airportsNotCovered, newUncoveredIter);
                }
                const bool hasRight = hasNext(airportsNotCovered, newUncoveredIter);
                int right = -1;
                if (hasRight)
                {
                    right = next(airportsNotCovered, newUncoveredIter);
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


        // Compute the bestCostForInner i.e. the cost required to arrange everything so that it is reachable, but *ignoring*
        // (for now) any adjustment necessary to make the gap between the endpoints large enough.
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

        // We've reduced the gap between the endpoints by bestCostForInner - is it enough?
        int cost = bestCostForInner;
        if (rightEndpoint - leftEndpoint + bestCostForInner < minDistance && (leftEndPointIndex != rightEndPointIndex))
        {
            // No, they're too close together - make the gap bigger!
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

        const auto result = findMinCostOfArrangementForDays(airportAddedOnDay, minDistance);
        for (const auto costForDay : result)
        {
            cout << costForDay << " ";
        }
        cout << endl;
    }
}
