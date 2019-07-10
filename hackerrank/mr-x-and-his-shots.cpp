#include <iostream>
#include <vector>

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
    cout << solutionBruteForce << endl;
}
