// Simon St James (ssjgz) - 2019-08-26
#include <iostream>
#include <vector>

#include <cassert>

//#define DIAGNOSTICS

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    const int N = read<int>();

    bool winnerIsPlayer1 = true;

    int64_t player1CumulativeScore = 0;
    int64_t player2CumulativeScore = 0;
    int64_t greatestLead = 0;

    for (int i = 0; i < N; i++)
    {
        const int player1RoundScore = read<int>();
        const int player2RoundScore = read<int>();

        player1CumulativeScore += player1RoundScore;
        player2CumulativeScore += player2RoundScore;

#ifdef DIAGNOSTICS
        cout << "In round " << (i + 1) << " player 1 scored: " << player1RoundScore << " and player 2 scored: " << player2RoundScore << endl;
        cout << "The cumulative scores are now - player 1: " << player1CumulativeScore << "; player 2: " << player2CumulativeScore << endl;
        cout << endl;
#endif

        const int64_t lead = abs(player1CumulativeScore - player2CumulativeScore);

        if (lead > greatestLead)
        {
            greatestLead = lead;

            if (player1CumulativeScore < player2CumulativeScore)
            {
#ifdef DIAGNOSTICS
                cout << "New best lead: player 2 has a lead of " << greatestLead << " over player 1" << endl << endl;
#endif
                winnerIsPlayer1 = false;
            }
            else
            {
#ifdef DIAGNOSTICS
                cout << "New best lead: player 2 has a lead of " << greatestLead << " over player 1" << endl << endl;
#endif
                winnerIsPlayer1 = true;
            }
        }
        assert(greatestLead != 0);
    }

#ifdef DIAGNOSTICS
    cout << "Final winner/ lead: " << endl;
#endif
    cout << (winnerIsPlayer1 ? 1 : 2) << " " << greatestLead << endl;

    assert(cin);
}
