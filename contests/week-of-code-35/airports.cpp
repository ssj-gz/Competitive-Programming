// Simon St James (ssjgz) 2017-11-18 (Week of Code 35).
// Tidied up and re-submitted to the Practice track on 2019-05-22.
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
        if (isUncovered(airportPos) &&
            (airportPos != leftEndpoint || leftEndPointDuplicated) && (airportPos != rightEndpoint || rightEndPointDuplicated)
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
            // Same as the right endpoint case, really!
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
    // A fairly tough one, but I really liked it - it's a nice, multi-layered one with plenty of interesting
    // sub-problems to solve.  Plus, it's the first Expert-rated question I solved in a live contest ;)
    //
    // So, where to start? Let's introduce the notion of "coverage" as a shorthand for "reachability in one
    // move" - we say that airport A *covers* airport B if |B - A| >= d.  Coverage is symmetric, but not
    // necessarily transitive, and is anti-reflexive.
    //
    // Let L and R be the leftmost and rightmost airports (respectively) before we start moving things around on a given Day.
    // Note that if A covers B, then at least one L or R also covers B - this is easily seen: assume wlog that 
    // A <= B, so B >= A + D since A covers B.  A >= L by definition of L, so B - L >= A + D - L >= D,
    // so L covers B.  Similarly, if B <= A, then R covers B.
    //
    // So it looks like endpoints are key, here, as they seem to cover "at least" as many airports as other airports.
    // In fact:
    //
    // Theorem
    //
    // All airports are mutually reachable from each other if and only if all airports are covered by at least one of L or R.
    //
    // Proof
    //
    // (=>) Assume all airports are mutually reachable.  Pick any airport A: then since A is reachable from any airport X we care to name,
    // take a path from X to A, and let B be the penultimate airport it in: then B covers A, as it can reach A in one
    // move.  If B is an endpoint, then we're done.
    //
    // If B is not an endpoint, then, if B >= A, then B - A >= d as it covers it.  Since B is not an endpoint, then R > B.
    //
    // So
    //    R > B
    // therefore
    //    R - A > B - A
    //          >= d
    // i.e. R - A > d, so R covers A.  Similarly, if B <= A, then L covers A.  So in any case, every airport A is covered
    // by at least one of L or R.
    //
    // (<=) Assume all airports are mutually covered by at least one of L or R.
    //
    // As a corollary, since an airport cannot cover itself, we must have that L is covered by R and R is covered by L.
    // Let A and B be any pair of airports - it suffices to show that we can form a path from A to B.
    //
    // There are four cases:
    //   i) A is covered by L, and B is covered by L.
    //
    //      Then A -> L -> B is a path from A to B.
    //
    //   ii) A is covered by L, and B is covered by R.
    //
    //       Then, since L and R cover each other, A -> L -> R -> B is a path from A to B.
    //
    //   iii) and iv) - obvious variants of i) and ii); proof omitted.
    //
    // QED.
    //
    // Let's introduce some terminology.  Call an arrangement of airports such that all are reachable from each other
    // (equivalent, by the Theorem, to "all airports are covered by at least one of L and R") a Fully Reachable
    // Arrangement, or FRA.  If the number of moves required to construct this FRA is minimal out of all FRAs, then
    // call it an Optimal FRA or OFRA.  The goal of this problem is to find the number of moves in an OFRA.
    //
    // What observations can we make about forming an OFRA?
    //
    // Firstly, an airport only ever moves in one direction when forming the OFRA - if it "doubled-back", as it where,
    // then we could eliminate the move where it moved left then right (or right then left) for strictly fewer moves
    // so couldn't have been optimal.
    //
    // Secondly, note that moving R rightwards or L leftwards covers at least the original airports, so such a move
    // never makes things worse.
    //
    // The sequence of moves necessary to form the OFRA can be made in any order - only the final resting place of airports
    // and the number of moves we took to get there are of any importance.
    //
    // The second point hints that we might be able to get an OFRA by moving *only* the L and R airports, and this is in fact
    // the case, as we will shortly prove.  First, though, a quick Lemma.
    //
    // Lemma
    //
    // We can assume that forming an OFRA does not involve any pair of airports (A and B, say), initially at different locations,
    // passing through each other.
    //
    // Proof
    //
    // Let E_A and E_B be the final resting places of A and B (respectively) after forming the OFRA, and that A and B were initially
    // at different locations and that they passed through each other when forming the OFRA.
    // Then we can form an OFRA that looks exactly the same except that the final resting places of A and B are now E_B and E_A.
    // It's hopefully clear that such an arrangement is still FRA, and that it takes at most as many moves as does forming
    // the original FRA.  The latter case is fiddly to prove but simple: proof omitted.
    //
    // Theorem
    //
    // We can assume that an OFRA moves only its endpoints.
    //
    // Proof
    //
    // Take any OFRA where an airport other than one of L and R is moved, and call that airport A.  We know that A must move in only one
    // direction, so assume wlog that it moves rightwards (the case where it moves leftwards is almost identical and is omitted).
    //
    // As noted earlier, we can re-arrange the movements in any order we please, so re-order so that A makes all of its moves last.
    //
    // Then after the penultimate move of A, we cannot have an FRA (else, we could stop here with one less move than in the original OFRA,
    // contradicting the original's optimality). Thus, after the last move, A must not have been covered by either of L or R, but after
    // the final move, it was.
    //
    // Since A is moving rightwards, it must have been that A was not covered by L (not R), and then after moving A its final step to E_A, it was:
    // if it had been R that A's final move made it covered by, then moving rightwards would have moved it further from R, 
    // and so A and R must have crossed paths, contradicting optimality of original OFRA. 
    //
    // However, instead of making this final move of A, we could instead have moved L leftwards: since moving L leftwards is never a "bad move",
    // this gives us an OFRA which has the same number of total moves; where A makes exactly one less move; and which does not make
    // any thing worse.
    //
    // Thus, by repeated application, we can eventually end up with an OFRA with the same amount of moves where A makes 0 moves.
    //
    // If some other airport, B, say (B != L nor R), then we can repeat the procedure for B, etc.
    //
    // Thus, we can always turn an OFRA where a non-endpoint airport moves into one of equivalent cost where no non-endpoint moves.
    //
    // QED.
    //
    // So, to recap: we need only find the minimum number of moves for L and R, where L never moves rightwards and R never moves rightwards,
    // so that L and R (collectively) cover every airport (including each other).
    //
    // How can L and R cover every airport? Well, R could cover all airports to its left.  We can easily work out the cost of this: find the 
    // rightmost airport other than R, and move R so that it is at this rightmost airport's position + d.  Similarly, we could have
    // L cover all airports to its right, and the cost of this is also easily computed.
    //
    // Or, L and R could cover all airports collaboratively.  Assume for the moment that no airports occupy the same position, except for 
    // the positions occupied by L and R: it's hopefully clear that this doesn't affect the cost of anything, as two airports at 
    // the same position are covered by one of L and R iff the other is.  It's also hopefully clear that, if L and R collaborate to
    // cover all airports, then there are two *consecutive* airports X and Y (such that X <= Y, and there is no airport Z with X <= Z <= Y)
    // such that X and all airports to its left are covered by R, and and Y and all airports to its right are covered by L.
    //
    // For this to hold, R would have to be at X + d and L would have to be at Y - d.  The cost of this arrangement (we'll assume that 
    // L doesn't already cover Y and R doesn't already cover X!) is:
    //   
    //   (X + d) - R + (moving R into place)
    //   L - (Y - d)   (moving L into place) 
    //   = X + d - R + L - Y + d
    //   = (X - Y) + L - R + 2 * d
    //   = L - R + 2 * d - (Y - X)
    //   = L - R + 2 * d - (distance between X and Y)
    //
    // This is interesting, as the cost doesn't depend on the locations of X and Y - only on the difference between them! (L, R and d are
    // all constants for this Day), and to minimise the cost, we need to pick the successive pairs of airports that have the *largest* gap
    // between them.
    //
    // So it seems that all we need to do is maintain (incrementally, for each day):
    //
    //   The leftmost and rightmost endpoints, L and R;
    //   The list of airports not covered by this L and R;
    //   The rightmost and leftmost such uncovered airports other than L and R (in case it's cheaper for one of L or R to cover all airports); and
    //   The maximum distance between successive pairs of such airports (in case it's cheaper for L and R to cover all airports collaboratively).
    //
    // And that's basically it - maintaining all this information per day is pretty easy.  There are some edge-cases where two or most
    // airports occupy the same position of one of L and R, but these are "simple" to deal with.
    //
    // Hopefully the comments in the code fill in any blanks :)
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
