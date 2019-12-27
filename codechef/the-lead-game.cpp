// Simon St James (ssjgz) - 2019-08-26
#include <iostream>
#include <vector>

#include <cassert>

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

        const int64_t lead = abs(player1CumulativeScore - player2CumulativeScore);

        if (lead > greatestLead)
        {
            greatestLead = lead;

            if (player1CumulativeScore < player2CumulativeScore)
                winnerIsPlayer1 = false;
            else
                winnerIsPlayer1 = true;
        }
        assert(greatestLead != 0);
    }

    cout << (winnerIsPlayer1 ? 1 : 2) << " " << greatestLead << endl;

    assert(cin);
}
