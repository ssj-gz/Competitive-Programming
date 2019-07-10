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
                m_ranges.push_back({range, id});
                id++;
            }
        }
        int numRangesAroundX() const
        {
            return 0;
        }
        void incrementX()
        {
            m_x++;
            while (!m_rangesByBegin.empty() && m_rangesByBegin.begin()->range.begin == m_x)
            {
                const auto newActiveRange = *(m_rangesByBegin.begin());

                m_activeRangesByEnd.insert(newActiveRange);

                m_rangesByBegin.erase(m_rangesByBegin.end());
            }
            while (!m_activeRangesByEnd.empty() && m_activeRangesByEnd.begin()->range.end < m_x)
            {
                m_activeRangesByEnd.erase(m_activeRangesByEnd.begin());
            }

        }
    private:
        struct RangeWithId
        {
            Range range;
            int id = -1;
        };
        vector<RangeWithId> m_ranges;
        int m_x = 0;


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
    return 0;
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
