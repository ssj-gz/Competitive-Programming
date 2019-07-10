// Simon St James (ssjgz) - 2019-07-10
#include <iostream>
#include <vector>
#include <set>

#include <cassert>

using namespace std;

struct Range
{
    int begin = -1;
    int end = -1;
};

class RangeTracker
{
    public:
        RangeTracker(const vector<Range>& ranges)
        {
            int id = 0;
            for (const auto range : ranges)
            {
                assert(range.begin >= 1);
                m_rangesByBegin.insert({range, id});
                id++;
            }
        }
        int numRangesAroundX() const
        {
            return m_activeRangesByEnd.size();
        }
        int numActiveRangesAddedAfterIncrement() const
        {
            return m_numActiveRangesAddedAfterIncrement;
        }
        void incrementX()
        {
            m_x++;
            m_numActiveRangesAddedAfterIncrement = 0;
            while (!m_rangesByBegin.empty() && m_rangesByBegin.begin()->range.begin <= m_x)
            {
                const auto newActiveRange = *(m_rangesByBegin.begin());
                m_activeRangesByEnd.insert(newActiveRange);
                
                m_numActiveRangesAddedAfterIncrement++;

                m_rangesByBegin.erase(m_rangesByBegin.begin());
            }
            while (!m_activeRangesByEnd.empty() && m_activeRangesByEnd.begin()->range.end < m_x)
            {
                m_activeRangesByEnd.erase(m_activeRangesByEnd.begin());
            }

        }
        bool hasRangesLeft() const
        {
            return !m_rangesByBegin.empty() || !m_activeRangesByEnd.empty();
        }
    private:
        // We add an "id" so that our sets may contain multiple copies of ranges
        // with the same "begin" and/ or "end"!
        struct RangeWithId
        {
            Range range;
            int id = -1;
        };

        int m_x = 0;
        int m_numActiveRangesAddedAfterIncrement = 0;

        static bool compareRangeBegins(const RangeWithId& lhs, const RangeWithId& rhs)
        {
            if (lhs.range.begin != rhs.range.begin)
                return lhs.range.begin < rhs.range.begin;
            return lhs.id < rhs.id;
        };
        static bool compareRangeEnds(const RangeWithId& lhs, const RangeWithId& rhs)
        {
            if (lhs.range.end != rhs.range.end)
                return lhs.range.end < rhs.range.end;
            return lhs.id < rhs.id;
        };

        set<RangeWithId, bool(*)(const RangeWithId&, const RangeWithId&)>  m_rangesByBegin{compareRangeBegins};
        set<RangeWithId, bool(*)(const RangeWithId&, const RangeWithId&)>  m_activeRangesByEnd{compareRangeEnds};
};

int64_t findNumShotsThatCanBeStopped(const vector<Range>& shots, const vector<Range>& players)
{
    int64_t numShotsThatCanBeStopped = 0;
    RangeTracker shotRangeTracker(shots);
    RangeTracker playerRangeTracker(players);

    while (shotRangeTracker.hasRangesLeft() || playerRangeTracker.hasRangesLeft())
    {
        shotRangeTracker.incrementX();
        playerRangeTracker.incrementX();

        const int64_t numShotRangesActive = shotRangeTracker.numRangesAroundX();
        const int64_t numPlayerRangesActive = playerRangeTracker.numRangesAroundX();

        const int64_t newShotRangesActive = shotRangeTracker.numActiveRangesAddedAfterIncrement();
        const int64_t newPlayerRangesActive = playerRangeTracker.numActiveRangesAddedAfterIncrement();

        // Update numShotsThatCanBeStopped based on the new shot ranges and new player ranges that
        // became active.
        // Note that this calculatin overcounts.
        numShotsThatCanBeStopped += newShotRangesActive * numPlayerRangesActive;
        numShotsThatCanBeStopped += newPlayerRangesActive * numShotRangesActive;
        // Remove the amount we overcounted by, above.
        numShotsThatCanBeStopped -= newPlayerRangesActive * newShotRangesActive;
    }

    return numShotsThatCanBeStopped;
}

int main(int argc, char* argv[])
{
    // Easy one, though there's a bit of subtlety with updating numShotsThatCanBeStopped
    // involving overcounting.
    //
    // This is basically a "how many ranges surround a given point?" problem, where we have
    // *two* lists of ranges.  RangeTracker handles these queries as we sweep from left to
    // right across the cricket field, with "moving right" taking O(log2 max(numShots, numPlayers)) per move.
    // Hopefully the code above is self-explanatory.
    int numShots;
    cin >> numShots;

    int numPlayers;
    cin >> numPlayers;

    vector<Range> shots(numShots);
    for (int i = 0; i < numShots; i++)
    {
        int begin, end;
        cin >> begin >> end;

        shots[i] = { begin, end };
    }

    vector<Range> players(numPlayers);
    for (int i = 0; i < numPlayers; i++)
    {
        int begin, end;
        cin >> begin >> end;

        players[i] = { begin, end };
    }

    const auto numShotsThatCanBeStopped = findNumShotsThatCanBeStopped(shots, players);
    cout << numShotsThatCanBeStopped << endl;
}
