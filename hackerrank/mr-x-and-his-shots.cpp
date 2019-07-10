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
                //cout << this << " adding new active range for x: " << m_x << endl;
                const auto newActiveRange = *(m_rangesByBegin.begin());
                m_activeRangesByEnd.insert(newActiveRange);
                
                m_numActiveRangesAddedAfterIncrement++;

                m_rangesByBegin.erase(m_rangesByBegin.begin());
            }
            while (!m_activeRangesByEnd.empty() && m_activeRangesByEnd.begin()->range.end < m_x)
            {
                //cout << this << " removing active range for x: " << m_x << endl;
                m_activeRangesByEnd.erase(m_activeRangesByEnd.begin());
            }

        }
        bool hasRangesLeft() const
        {
            return !m_rangesByBegin.empty() || !m_activeRangesByEnd.empty();
        }
    private:
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

        // Update numShotsThatCanBeStopped based on the new shot ranges and player ranges that
        // became active.
        // Note that this overcounts.
        numShotsThatCanBeStopped += newShotRangesActive * numPlayerRangesActive;
        numShotsThatCanBeStopped += newPlayerRangesActive * numShotRangesActive;
        // Remove the amount we overcounted by, above.
        numShotsThatCanBeStopped -= newPlayerRangesActive * newShotRangesActive;

    }

    return numShotsThatCanBeStopped;
}

int main(int argc, char* argv[])
{
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
