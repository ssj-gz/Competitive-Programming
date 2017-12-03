#include <iostream>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

enum PlayState { Player1Win, Player1Lose };
enum Player { Player1, Player2 };

const string player1Name = "Bob";
const string player2Name = "Alice";

Player otherPlayer(Player player)
{
    if (player == Player1)
        return Player2;
    else
        return Player1;
}

PlayState winForPlayer(Player player)
{
    if (player == Player1)
        return Player1Win;
    else
        return Player1Lose;
}

PlayState loseForPlayer(Player player)
{
    if (player == Player1)
        return Player1Lose;
    else
        return Player1Win;
}

class GameState
{
    public:
};

bool operator<(const GameState& lhs, const GameState& rhs)
{
    assert(false && "Fill me in - GameState operator<");
    return false;
}

class Move
{
    public:
};

ostream& operator<<(ostream& os, const GameState& gameState)
{
    return os;
}

ostream& operator<<(ostream& os, const Move& move)
{
    return os;
}

map<pair<GameState, Player>, PlayState> playStateForLookup;

vector<Move> movesFor(Player currentPlayer, const GameState& gameState)
{
    assert(false && "Fill me in - movesFor");
    vector<Move> moves;
    return moves;
}

GameState gameStateAfterMove(const GameState& gameState, Player currentPlayer, const Move& move)
{
    assert(false && "Fill me in - gameStateAfterMove");
    return GameState();
}

PlayState findWinner(Player currentPlayer, const GameState& gameState)
{
    if (playStateForLookup.find({gameState, currentPlayer}) != playStateForLookup.end())
    {
        return playStateForLookup[{gameState, currentPlayer}];
    }
    const vector<Move> availableMoves = movesFor(currentPlayer, gameState);
    PlayState playState = loseForPlayer(currentPlayer);

    for (const auto& move : availableMoves)
    {
        const auto newGameState = gameStateAfterMove(gameState, currentPlayer, move);
        const auto result = findWinner(otherPlayer(currentPlayer), newGameState);
        if (result == winForPlayer(currentPlayer))
        {
            playState = winForPlayer(currentPlayer);
        }
    }

    playStateForLookup[{gameState, currentPlayer}] = playState;

    return playState;
}

int main()
{
}

