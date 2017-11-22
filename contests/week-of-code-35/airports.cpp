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

// Wrapper around std::map that gets rid of some of the ugliness of finding its min, max,
// next in sequence, previous in sequence, etc.
template <typename T>
class MinMaxTracker
{
    public:
        void add(const T& value)
        {
            m_countOf[value]++;
            if (!m_hasMaxValue || value > m_maxValue)
            {
                m_maxValue = value;
                m_hasMaxValue = true;
            }
            if (!m_hasMinValue || value < m_minValue)
            {
                m_minValue = value;
                m_hasMinValue = true;
            }
        }
        void remove(const T& value)
        {
            auto valueIter = m_countOf.find(value);
            assert(valueIter != m_countOf.end());
            m_countOf[value]--;
            if (m_countOf[value] == 0)
            {
                valueIter = m_countOf.erase(valueIter);
                if (m_countOf.empty())
                {
                    m_hasMinValue = false;
                    m_hasMaxValue = false;
                }
                else
                {
                    if (valueIter == m_countOf.end())
                    {
                        assert(value == m_maxValue);
                        assert(value != m_minValue);
                        valueIter--;
                        m_maxValue = valueIter->first;
                    }
                    else
                    {
                        assert(value != m_maxValue);
                        if (value == m_minValue)
                        {
                            m_minValue = valueIter->first;
                        }
                    }
                }
            }
        }
        bool contains(const T& value) const
        {
            return (m_countOf.find(value) != m_countOf.end());
        }
        typename map<T, int>::iterator find(const T& value)
        {
            return m_countOf.find(value);
        }
        bool empty() const
        {
            return m_countOf.empty();
        }
        T min() const
        {
            assert(!m_countOf.empty());
            return m_minValue;
        }
        T max() const
        {
            assert(!m_countOf.empty());
            return m_maxValue;
        };
        bool hasNext(typename map<T, int>::const_iterator valueIter) const
        {
            return !m_countOf.empty() && valueIter != m_countOf.end() && valueIter->first != m_maxValue;
        }
        T next(typename map<T, int>::const_iterator valueIter) const
        {
            assert(hasNext(valueIter));
            valueIter++;
            return valueIter->first;
        }
        bool hasPrevious(typename map<T, int>::const_iterator valueIter) const
        {
            return !m_countOf.empty() && valueIter != m_countOf.begin();
        }
        T previous(typename map<T, int>::const_iterator valueIter) const
        {
            assert(hasPrevious(valueIter));
            valueIter--;
            return valueIter->first;
        }
    private:
        map<T, int> m_countOf;
        T m_maxValue;
        bool m_hasMaxValue = false;
        T m_minValue;
        bool m_hasMinValue = false;
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

    MinMaxTracker<int> diffsOfSuccessiveUncoveredPairs;

    auto isUncovered = [&leftEndpoint, &rightEndpoint, minDistance](const int airportPos)
    {
        return airportPos < leftEndpoint + minDistance && airportPos > rightEndpoint - minDistance;
    };
    MinMaxTracker<int> uncoveredAirportPositions;
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
            // uncoveredAirportPositions (updating diffsOfSuccessiveUncoveredPairs as we go) ...
            while (!uncoveredAirportPositions.empty() && uncoveredAirportPositions.min() <= rightEndpoint - minDistance)
            {
                const auto oldMin = uncoveredAirportPositions.min();
                uncoveredAirportPositions.remove(oldMin);
                if (!uncoveredAirportPositions.empty())
                {
                    const int diffToRemove = uncoveredAirportPositions.min() - oldMin;

                    assert(diffToRemove > 0);
                    diffsOfSuccessiveUncoveredPairs.remove(diffToRemove);

                }
            }
            // ... and maybe add the previous endpoint as an uncovered airport.
            if ((i != 0) && (isUncovered(oldRightEndPoint) && oldRightEndPoint != leftEndpoint))
            {
                newlyUncoveredAirportPositions.push_back(oldRightEndPoint);
            }
        }
        if (oldLeftEndPoint != leftEndpoint)
        {
            // Left endpoint has been moved leftwards; erase the now-covered airports from the end of uncoveredAirportPositions, etc.
            // Same as the right endpoint case, really, except dealing with iterators is more ugly!
            while (!uncoveredAirportPositions.empty() && uncoveredAirportPositions.max() >= leftEndpoint + minDistance)
            {
                const auto oldMax = uncoveredAirportPositions.max();

                uncoveredAirportPositions.remove(oldMax);
                if (!uncoveredAirportPositions.empty())
                {
                    const int diffToRemove = oldMax - uncoveredAirportPositions.max();
                    assert(diffToRemove > 0);
                    diffsOfSuccessiveUncoveredPairs.remove(diffToRemove);

                }
            }
            if ((i != 0) && (isUncovered(oldLeftEndPoint) && (oldLeftEndPoint != rightEndpoint)))
            {
                newlyUncoveredAirportPositions.push_back(oldLeftEndPoint);
            }
        }

        // Add the new uncovered airport positions to uncoveredAirportPositions if they are not there already,
        // and keep diffsOfSuccessiveUncoveredPairs up-to-date.
        for (const auto uncoveredAirportPos : newlyUncoveredAirportPositions)
        {
            if (!uncoveredAirportPositions.contains(uncoveredAirportPos))
            {
                uncoveredAirportPositions.add(uncoveredAirportPos);
                const auto newUncoveredIter = uncoveredAirportPositions.find(uncoveredAirportPos);
                const bool hasLeft = uncoveredAirportPositions.hasPrevious(newUncoveredIter);
                const int left = (hasLeft ? uncoveredAirportPositions.previous(newUncoveredIter) : -1);
                const bool hasRight = uncoveredAirportPositions.hasNext(newUncoveredIter);
                const int right = (hasRight ? uncoveredAirportPositions.next(newUncoveredIter) : -1);

                if (hasLeft && hasRight)
                {
                    // Broken up a previously successive pair; remove their contribution.
                    diffsOfSuccessiveUncoveredPairs.remove(right - left);
                } 
                if (hasLeft)
                {
                    diffsOfSuccessiveUncoveredPairs.add(uncoveredAirportPos - left);
                }
                if (hasRight)
                {
                    diffsOfSuccessiveUncoveredPairs.add(right - uncoveredAirportPos);
                }
            }
        }


        // Compute the bestCostForInner i.e. the cost required to arrange everything so that it is reachable, but *ignoring*
        // (for now) any adjustment necessary to make the gap between the endpoints large enough.
        bestCostForInner = 0;
        if (!uncoveredAirportPositions.empty())
        {
            bestCostForInner = numeric_limits<int>::max();
            const int minUncoveredAirport = uncoveredAirportPositions.min();
            const int maxUncoveredAirport = uncoveredAirportPositions.max();

            // If left endpoint extended to cover everything.
            bestCostForInner = min(bestCostForInner,  (leftEndpoint + minDistance) - minUncoveredAirport);
            // If right end point extended to cover everything.
            bestCostForInner = min(bestCostForInner,  maxUncoveredAirport - (rightEndpoint - minDistance));
            // The other case: left endpoint and right endpoint both extended to cover a successive pair
            // of uncovered airports so that all airports become covered.
            if (!diffsOfSuccessiveUncoveredPairs.empty())
            {
                bestCostForInner = min(bestCostForInner,  leftEndpoint - rightEndpoint + 2 * minDistance - diffsOfSuccessiveUncoveredPairs.max());
            }
        }

        int bestCost = bestCostForInner;
        // We've reduced the gap between the endpoints by bestCostForInner (bestCostForInner represents the sum of the amount we
        // adjusted the left endpoint leftwards plus the right endpoint rightwards) - is it enough?
        if (rightEndpoint - leftEndpoint + bestCostForInner < minDistance && (leftEndPointIndex != rightEndPointIndex))
        {
            // No, they're too close together - make the gap bigger!
            bestCost += leftEndpoint - (rightEndpoint - minDistance);
        }
        results.push_back(bestCost);
    }
    return results;
}


int main(int argc, char** argv)
{
    ios::sync_with_stdio(false);
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
