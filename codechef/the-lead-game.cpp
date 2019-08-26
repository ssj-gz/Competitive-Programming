// Simon St James (ssjgz) - 2019-08-26
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        while (true)
        {

            const int N = rand() % 10'000 + 1;
            const int maxPlayer1Score = rand() % 1000 + 1;
            const int maxPlayer2Score = rand() % 1000 + 1;

            vector<int> player1Scores;
            vector<int> player2Scores;

            int64_t player1CumulativeScore = 0;
            int64_t player2CumulativeScore = 0;
            int64_t greatestLead = 0;

            bool valid = true;

            for (int i = 0; i < N; i++)
            {
                const int player1RoundScore = rand() % maxPlayer1Score + 1;
                const int player2RoundScore = rand() % maxPlayer2Score + 1;

                player1Scores.push_back(player1RoundScore);
                player2Scores.push_back(player2RoundScore);

                assert(1 <= player1RoundScore && player1RoundScore <= 1000);
                assert(1 <= player2RoundScore && player2RoundScore <= 1000);

                player1CumulativeScore += player1RoundScore;
                player2CumulativeScore += player2RoundScore;

                const int64_t lead = abs(player1CumulativeScore - player2CumulativeScore);

                if (lead == greatestLead)
                {
                    valid = false;
                    break;
                }
                if (lead == 0)
                {
                    valid = false;
                    break;
                }
                greatestLead = max(greatestLead, lead);

            }
            if (valid)
            {
                cout << N << endl;
                for (int i = 0; i < N; i++)
                {
                    cout << player1Scores[i] << " " << player2Scores[i] << endl;
                }
                return 0;
            }

        }


        return 0;
    }

    const int N = read<int>();
    assert(1 <= N && N <= 10'000);

    bool winnerIsPlayer1 = true;

    int64_t player1CumulativeScore = 0;
    int64_t player2CumulativeScore = 0;
    int64_t greatestLead = 0;

    for (int i = 0; i < N; i++)
    {
        const int player1RoundScore = read<int>();
        const int player2RoundScore = read<int>();
        assert(1 <= player1RoundScore && player1RoundScore <= 1000);
        assert(1 <= player2RoundScore && player2RoundScore <= 1000);

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

    cout << (winnerIsPlayer1 ? 1 : 2);
    cout << " ";
    cout << greatestLead << endl;



    assert(cin);
}
