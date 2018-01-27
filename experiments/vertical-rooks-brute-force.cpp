// Simon St James (ssjgz) - 2017-12-03.  Framework for exploring "Optimal Play" Game Theory games.
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <cassert>

using namespace std;

//#define VERY_VERBOSE
#define PRINT_COMPUTER_MOVES

enum PlayState { Player1Win, Player1Lose, Draw };
enum Player { Player1, Player2 };

const string player1Name = "player-1";
const string player2Name = "player-2";

enum PlayerType { CPU, Random, Human };

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

class GameState;
class Move;

vector<Move> movesFor(Player currentPlayer, const GameState& gameState);

class GameState
{
    public:
        int numColumns = -1;
        int numRows = -1;
        vector<int> player1Positions;
        vector<int> player2Positions;
        int numBackMovesAllowed = 0;
        void validate()
        {
            assert(numColumns > 0 && numRows > 0);
            assert(player1Positions.size() == numColumns && player2Positions.size() == numColumns);
            for (int column = 0; column < numColumns; column++)
            {
                assert(player1Positions[column] >= 1 && player1Positions[column] <= numRows);
                assert(player2Positions[column] >= 1 && player2Positions[column] <= numRows);
                assert(player1Positions[column] != player2Positions[column]);
            }
        }
        bool hasWinningPlayerOverride(Player currentPlayer) const
        {
            // Assume that a GameState that has no moves that lead to a Lose for the
            // other player is a losing state (and vice-versa).
            return false;
        }
        PlayState winningPlayerOverride(Player currentPlayer) const
        {
            assert(false);
            return Player1Win;
        }
};

bool operator<(const GameState& lhs, const GameState& rhs)
{
    if (lhs.numBackMovesAllowed != rhs.numBackMovesAllowed)
        return lhs.numBackMovesAllowed < rhs.numBackMovesAllowed;
    if (lhs.player1Positions != rhs.player1Positions)
        return lhs.player1Positions < rhs.player1Positions;

    return lhs.player2Positions < rhs.player2Positions;
}

class Move
{
    public:
        int column = -1;
        int dy = 0;
        bool isBackmove = false;
        static Move preferredMove(const vector<Move>& moves, PlayState moveOutcome, Player currentPlayer, const GameState& gameState)
        {
            if (moveOutcome == winForPlayer(currentPlayer))
            {
                // Pick a move where we don't backmove, for illustration purposes :)
                for (const auto& move : moves)
                {
                    if (!move.isBackmove)
                        return move;
                }
                assert(false);
            }
            // Pick the first one, arbitrarily - feel free to add your own preferences here :)
            return moves.front();
        }
};

ostream& operator<<(ostream& os, const GameState& gameState)
{
    os << endl;
    os << "Player 1 Positions: " << endl;
    for (const auto pos : gameState.player1Positions)
        os << " " << pos;
    os << endl;
    os << "Player 2 Positions: " << endl;
    for (const auto pos : gameState.player2Positions)
        os << " " << pos;
    os << endl;
    os << "Backmoves allowed: " << gameState.numBackMovesAllowed << endl;
    return os;
}

ostream& operator<<(ostream& os, const Move& move)
{
    os << "Move rook at column " << move.column << " " << abs(move.dy) << " spaces " << (move.dy < 0 ? "upwards" : "downwards") << " (" << (!move.isBackmove ? "not " : "") << "a backmove)";
    return os;
}

ostream& operator<<(ostream& os, Player player)
{
    if (player == Player1)
        os << player1Name;
    else
        os << player2Name;

    return os;
}

ostream& operator<<(ostream& os, PlayState playState)
{
    if (playState == Player1Win)
        os << Player1 << " wins";
    else if (playState == Player1Lose)
        os << Player2 << " wins";
    else
        os << "Draw";

    return os;
}


map<pair<GameState, Player>, PlayState> playStateForLookup;

vector<Move> movesFor(Player currentPlayer, const GameState& gameState)
{
    vector<Move> moves;
    const auto numColumns = gameState.numColumns;
    const auto numRows = gameState.numRows;
    for (int column = 0; column < numColumns; column++)
    {
        int minRow = -1;
        int maxRow = -1;
        if (currentPlayer == Player1)
        {
            if (gameState.player1Positions[column] < gameState.player2Positions[column])
            {
                minRow = 1;
                maxRow = gameState.player2Positions[column] - 1;
            }
            else
            {
                minRow = gameState.player2Positions[column] + 1;
                maxRow = numRows;
            }
        }
        else
        {
            if (gameState.player2Positions[column] < gameState.player1Positions[column])
            {
                minRow = 1;
                maxRow = gameState.player1Positions[column] - 1;
            }
            else
            {
                minRow = gameState.player1Positions[column] + 1;
                maxRow = numRows;
            }
        }
        for (int row = minRow; row <= maxRow; row++)
        {
            GameState stateAfterMove = gameState;
            const int originalRookDistance = abs(gameState.player1Positions[column] - gameState.player2Positions[column]);
            int originalRow = -1;
            if (currentPlayer == Player1)
            {
                originalRow = gameState.player1Positions[column];
                stateAfterMove.player1Positions[column] = row;
            }
            else
            {
                originalRow = gameState.player2Positions[column];
                stateAfterMove.player2Positions[column] = row;
            }
            const int rookDistanceAfterMove = abs(stateAfterMove.player1Positions[column] - stateAfterMove.player2Positions[column]);
            const bool isBackmove = (rookDistanceAfterMove > originalRookDistance);
            if (isBackmove && gameState.numBackMovesAllowed == 0)
                continue;
            if (stateAfterMove.player1Positions == gameState.player1Positions && stateAfterMove.player2Positions == gameState.player2Positions)
                continue;

            Move move;
            move.column = column;
            move.dy = row - originalRow;
            move.isBackmove = isBackmove;

            moves.push_back(move);
        }

    }

    return moves;
}

GameState gameStateAfterMove(const GameState& gameState, Player currentPlayer, const Move& move)
{
    GameState nextGameState(gameState);
    if (currentPlayer == Player1)
        nextGameState.player1Positions[move.column] += move.dy;
    else
        nextGameState.player2Positions[move.column] += move.dy;

    if (move.isBackmove)
        nextGameState.numBackMovesAllowed--;
    assert(nextGameState.numBackMovesAllowed >= 0);

    return nextGameState;
}

PlayState findWinnerAux(Player currentPlayer, const GameState& gameState, PlayerType player1Type, PlayerType player2Type, bool isBruteForceMoveSearch)
{
    const auto playThisMoveAsType = (currentPlayer == Player1 ? player1Type : player2Type);
    const bool playThisMoveInteractively = !isBruteForceMoveSearch && (playThisMoveAsType == Human);
    const bool playThisMoveRandomly = !isBruteForceMoveSearch && (playThisMoveAsType == Random);
    if (isBruteForceMoveSearch && playStateForLookup.find({gameState, currentPlayer}) != playStateForLookup.end())
    {
        // Don't use the cache if we're interactive/ random: it will know all losing states from earlier dry-runs,
        // and if the human player is in a losing state, won't give them a chance to make a move!
        return playStateForLookup[{gameState, currentPlayer}];
    }

    // Assume a loss by default.
    PlayState playState = loseForPlayer(currentPlayer);

    vector<Move> winningMovesForCurrentPlayer;
    vector<Move> losingMovesForCurrentPlayer;
    vector<Move> drawingMovesForCurrentPlayer;


    auto updatePlayStateFromMove = [&playState, &gameState, currentPlayer, player1Type, player2Type, &winningMovesForCurrentPlayer, &drawingMovesForCurrentPlayer, &losingMovesForCurrentPlayer]
        (const Move& move, bool isBruteForceMoveSearch)
        {
            const auto newGameState = gameStateAfterMove(gameState, currentPlayer, move);
            const auto result = findWinnerAux(otherPlayer(currentPlayer), newGameState, player1Type, player2Type, isBruteForceMoveSearch);
            if (result == winForPlayer(currentPlayer))
            {
                winningMovesForCurrentPlayer.push_back(move);
            }
            else if (result == Draw)
            {
                drawingMovesForCurrentPlayer.push_back(move);
            }
            else
            {
                losingMovesForCurrentPlayer.push_back(move);
            }
            if (result == winForPlayer(currentPlayer))
            {
                playState = winForPlayer(currentPlayer);
            }
            else if (playState != winForPlayer(currentPlayer) && result == Draw)
            {
                playState = Draw;
            }

        };

    auto readInt = [](const string& message)
    {
        while (true)
        {
            cout << message << endl;
            int value;
            cin >> value;

            if (!cin)
            {
                cout << "Invalid input; please try again" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            else
                return value;
        }
    };

    if (gameState.hasWinningPlayerOverride(currentPlayer))
    {
        playState = gameState.winningPlayerOverride(currentPlayer);

    }
    else
    {
        const vector<Move> availableMoves = movesFor(currentPlayer, gameState);

        if (playThisMoveInteractively)
        {
            if (!availableMoves.empty())
            {
                stringstream messagePromptStream;
                messagePromptStream << "Player " << currentPlayer << ", it is your move; game state is " << gameState << endl;
                for (int i = 0; i < availableMoves.size(); i++)
                {
                    messagePromptStream << i << ":  " << availableMoves[i] << endl;
                }
                int moveIndex = -1;
                while (true)
                {
                    moveIndex = readInt(messagePromptStream.str());
                    if (moveIndex < 0 || moveIndex >= availableMoves.size())
                    {
                        cout << "Invalid input" << endl;
                    }
                    else
                        break;
                }
                const auto chosenMove = availableMoves[moveIndex];
                cout << "You chose move " << chosenMove << " game state is now: " <<  gameStateAfterMove(gameState, currentPlayer, chosenMove) << endl;
                updatePlayStateFromMove(chosenMove, false);
            }
            else
            {
                cout << "Player " << currentPlayer << " - you have no moves available.  You lose; good-day, Sir!" << endl;
            }
        }
        else if (playThisMoveRandomly)
        {
            if (!availableMoves.empty())
            {
                const int moveIndex = rand() % availableMoves.size();
                const auto chosenMove = availableMoves[moveIndex];
                cout << "Randomly chose move " << chosenMove << " game state is now: " <<  gameStateAfterMove(gameState, currentPlayer, chosenMove) << endl;
                updatePlayStateFromMove(chosenMove, false);
            }
        }
        else
        {
            for (const auto& move : availableMoves)
            {
                const auto oldPlayState = playState;
                updatePlayStateFromMove(move, true);

#ifdef VERY_VERBOSE
                cout << "The move " << move << " from state: " << gameState << " is a " << (playState == winForPlayer(currentPlayer) ? "Win" : (playState == Draw ? "Draw" : "Lose")) << " for player " << currentPlayer << endl;
#endif

            }

            if (!isBruteForceMoveSearch && !availableMoves.empty())
            {
                Move chosenMove;
                if (playState == winForPlayer(currentPlayer))
                {
                    assert(!winningMovesForCurrentPlayer.empty());
                    chosenMove = Move::preferredMove(winningMovesForCurrentPlayer, playState, currentPlayer, gameState);
                }
                else if (playState == Draw)
                {
                    assert(!drawingMovesForCurrentPlayer.empty());
                    chosenMove = Move::preferredMove(drawingMovesForCurrentPlayer, playState, currentPlayer, gameState);
                }
                else
                {
                    assert(!losingMovesForCurrentPlayer.empty());
                    chosenMove = Move::preferredMove(losingMovesForCurrentPlayer, playState, currentPlayer, gameState);
                }
#ifdef PRINT_COMPUTER_MOVES
                cout << "Computer (" << currentPlayer << ") played move: " << chosenMove << " and thinks it will " << (playState == winForPlayer(currentPlayer) ? "Win" : (playState == Draw ? "Draw" : "Lose")) << ". Game state now: " <<  gameStateAfterMove(gameState, currentPlayer, chosenMove) << endl;
#endif
                updatePlayStateFromMove(chosenMove, false);
            }
        }
    }

#ifdef VERY_VERBOSE
    cout << "At game state: " << gameState << " with player " << currentPlayer << ", the player " << (playState == winForPlayer(currentPlayer) ? "Wins" : playState == winForPlayer(otherPlayer(currentPlayer)) ? "Loses" : "Draws") << endl;

#endif
    playStateForLookup[{gameState, currentPlayer}] = playState;

    return playState;
}


PlayState findWinner(Player currentPlayer, const GameState& initialGameState, PlayerType player1Type = CPU, PlayerType player2Type = CPU)
{
    cout << "Initial game state: " << initialGameState << endl;
    playStateForLookup.clear();
    const auto result = findWinnerAux(currentPlayer, initialGameState, player1Type, player2Type, false);

    if (player1Type == Human || player2Type == Human)
    {
        const auto interactivePlayer = (player1Type == Human ? Player1 : Player2);
        cout << "Result of interactive game: you played as " << interactivePlayer << "; " << currentPlayer << " took the first turn; " << result << endl;
    }

    return result;
}


int main(int argc, char** argv)
{
    const vector<int> player1Positions = { 1, 1, 1, 1};
    const vector<int> player2Positions = { 4, 3, 2, 2};

    GameState initialGameState;
    // A 4x4 grid - unfortunately, any larger than this takes ages to compute :/ Although 4 columns x 5 rows works and is a bit more interesting.
    // With this particular arrangement of Rooks, Player 1 is guaranteed to Win; you, poor human,
    // will be playing as Player 2 :)
    initialGameState.player1Positions = { 1, 1, 1, 1};
    initialGameState.player2Positions = { 4, 3, 2, 2};
    initialGameState.numRows = 4;
    initialGameState.numColumns = 4;
    // Stop the brute-force calculation from continuing forever - you won't be able to use up all these backmoves, though, so the limit
    // makes no difference as to whether you win or lose.
    initialGameState.numBackMovesAllowed = 15; 
    initialGameState.validate();

    const auto result = findWinner(Player1, initialGameState, CPU, Human);

    cout << "Result: " << result << endl;
}

