#include <iostream>
#include <cassert>

using namespace std;

enum Handshape
{
    Rock,
    Paper,
    Scissors
};

enum Outcome
{
    Player1Wins,
    Player2Wins,
    Draw
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
        char myMoveChar;
        cin >> myMoveChar;

        if (!cin)
            break;

        numRounds++;

        assert('A' <= opponentMoveChar && opponentMoveChar <= 'C');
        assert('X' <= myMoveChar && myMoveChar <= 'Z');
        const Handshape opponentMove = static_cast<Handshape>(opponentMoveChar - 'A');
        const Handshape myMove = static_cast<Handshape>(myMoveChar - 'X');
        const int myMovePoints = (myMoveChar - 'X') + 1;

        score += myMovePoints;

        switch (playRound(opponentMove, myMove))
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
