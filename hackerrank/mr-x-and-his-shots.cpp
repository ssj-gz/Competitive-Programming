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

bool rangesIntersect(const Range& lhs, const Range& rhs)
{
    if (lhs.end < rhs.begin || lhs.begin > rhs.end)
        return false;
    return true;
}

int64_t solveBruteForce(const vector<Range>& shots, const vector<Range>& players)
{
    int numShotsThatCanBeStopped = 0;
    for (const auto& playerRange : players)
    {
        for (const auto& shotRange : shots)
        {
            if (rangesIntersect(playerRange, shotRange))
                numShotsThatCanBeStopped++;
        }
    }
    return numShotsThatCanBeStopped;
}

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
            m_dbgRange = ranges;
        }
        int numRangesAroundX() const
        {
            // TODO - remove this.
            int dbgResult = 0;
            for (const auto& range : m_dbgRange)
            {
                if (m_x < range.begin || m_x > range.end)
                    continue;

                dbgResult++;

            }
            assert(dbgResult == m_activeRangesByEnd.size());
            return m_activeRangesByEnd.size();
        }
        void incrementX()
        {
            m_x++;
            while (!m_rangesByBegin.empty() && m_rangesByBegin.begin()->range.begin <= m_x)
            {
                cout << " adding new active range for x: " << m_x << endl;
                const auto newActiveRange = *(m_rangesByBegin.begin());

                m_activeRangesByEnd.insert(newActiveRange);

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
        struct RangeWithId
        {
            Range range;
            int id = -1;
        };
        int m_x = 0;
        vector<Range> m_dbgRange; // TODO - remove this.


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

int64_t solveOptimised(const vector<Range>& shots, const vector<Range>& players)
{
    int64_t result = 0;
    RangeTracker shotRangeTracker(shots);
    RangeTracker playerRangeTracker(players);

    int64_t numShotRangesActive = 0;
    int64_t numPlayerRangesActive = 0;
    int numIterations = 0;
    while (shotRangeTracker.hasRangesLeft() || playerRangeTracker.hasRangesLeft())
    {
        cout << "iteration: numIterations " << endl;
        const int64_t numShotRangesActiveAtStartOfIteration = numShotRangesActive;
        const int64_t numPlayerRangesActiveAtStartOfIteration = numPlayerRangesActive;

        shotRangeTracker.incrementX();
        playerRangeTracker.incrementX();

        numShotRangesActive = shotRangeTracker.numRangesAroundX();
        numPlayerRangesActive = playerRangeTracker.numRangesAroundX();

        const int64_t newShotRangesActive = numShotRangesActive - numShotRangesActiveAtStartOfIteration;
        const int64_t newPlayerRangesActive = numPlayerRangesActive - numPlayerRangesActiveAtStartOfIteration;

        cout << " newShotRangesActive: " << newShotRangesActive << " newPlayerRangesActive: " << newPlayerRangesActive << endl;

        if (newShotRangesActive > 0)
            result += newShotRangesActive * numPlayerRangesActive;
        if (newPlayerRangesActive > 0)
        result += newPlayerRangesActive * numShotRangesActiveAtStartOfIteration;

        numIterations++;
    }

    return result;
}

int main()
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


    const auto solutionBruteForce = solveBruteForce(shots, players);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
    const auto solutionOptimised = solveOptimised(shots, players);
    cout << "solutionOptimised: " << solutionOptimised << endl;
}
