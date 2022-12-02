#include <iostream>
#include <cassert>

using namespace std;

enum Handshape
{
    Rock,
    Paper,
    Scissors,
    Unknown
};

enum Outcome
{
    Player1Wins,
    Draw,
    Player2Wins
};

Outcome playRound(const Handshape player1Move, const Handshape player2Move)
{
    if (player1Move == Rock && player2Move == Scissors)
        return Player1Wins;
    if (player1Move == Scissors && player2Move == Rock)
        return Player2Wins;

    if (player1Move == Paper && player2Move == Rock)
        return Player1Wins;
    if (player1Move == Rock && player2Move == Paper)
        return Player2Wins;

    if (player1Move == Scissors && player2Move == Paper)
        return Player1Wins;
    if (player1Move == Paper && player2Move == Scissors)
        return Player2Wins;

    return Draw;
}

int main()
{
    int64_t score = 0;
    int numRounds = 0;

    while (true)
    {
        char opponentMoveChar;
        cin >> opponentMoveChar;
        char requiredOutcomeChar;
        cin >> requiredOutcomeChar;

        if (!cin)
            break;

        numRounds++;

        assert('A' <= opponentMoveChar && opponentMoveChar <= 'C');
        assert('X' <= requiredOutcomeChar && requiredOutcomeChar <= 'Z');
        const Handshape opponentMove = static_cast<Handshape>(opponentMoveChar - 'A');
        const Outcome requiredOutcome = static_cast<Outcome>(requiredOutcomeChar - 'X');

        Handshape requiredMoveToPlay = Unknown;
        for (const auto move : { Rock, Scissors, Paper })
        {
            if (playRound(opponentMove, move) == requiredOutcome)
            {
                requiredMoveToPlay = move;
            }
        }
        assert(requiredMoveToPlay != Unknown);
        const int myMoveScore = requiredMoveToPlay - Rock + 1;
        score+= myMoveScore;

        switch (requiredOutcome)
        {
            case Player1Wins:
                score += 0;
                break;
            case Player2Wins:
                score += 6;
                break;
            case Draw:
                score += 3;
                break;
            default:
                assert(false);
        };
    }

    cout << "Total score: " << score << " (" << numRounds << " rounds played)" << std::endl;
}
