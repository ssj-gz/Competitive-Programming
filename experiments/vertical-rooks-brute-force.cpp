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
        vector<int> player1RowForColumn;
        vector<int> player2RowForColumn;
        static int64_t numGameStatesOverEstimate;
        bool isWinForCurrentPlayer() const
        {
            int xorSum = 0;
            for (int column = 0; column < numColumns; column++)
            {
                const auto rookDistance = abs(player1RowForColumn[column] - player2RowForColumn[column]) - 1;
                xorSum ^= rookDistance;
            }
            return (xorSum != 0);
        }
        void validate()
        {
            assert(numColumns > 0 && numRows > 0);
            assert(player1RowForColumn.size() == numColumns && player2RowForColumn.size() == numColumns);
            for (int column = 0; column < numColumns; column++)
            {
                assert(player1RowForColumn[column] >= 1 && player1RowForColumn[column] <= numRows);
                assert(player2RowForColumn[column] >= 1 && player2RowForColumn[column] <= numRows);
                assert(player1RowForColumn[column] != player2RowForColumn[column]);
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
int64_t GameState::numGameStatesOverEstimate = -1;

bool operator<(const GameState& lhs, const GameState& rhs)
{
    if (lhs.player1RowForColumn != rhs.player1RowForColumn)
        return lhs.player1RowForColumn < rhs.player1RowForColumn;

    return lhs.player2RowForColumn < rhs.player2RowForColumn;
}

class Move
{
    public:
        int column = -1;
        int dy = 0;
        bool isBackmove = false;
        static Move preferredMove(const vector<Move>& moves, PlayState moveOutcome, Player currentPlayer, const GameState& gameState)
        {
            // ** SPOILER SPOILER SPOILER **
            // ** SPOILER SPOILER SPOILER **
            // ** SPOILER SPOILER SPOILER **

            // There is a winning strategy for a Winning player that does not require backmoves, so prefer not to use them; 
            // if you want to incorporate them, uncomment the ALLOW_COMPUTER_BACKMOVES define below  - it's still quite rare that a computer will decide to use one, though! :)
            // If ALLOW_COMPUTER_BACKMOVES is set, we still restrict how often it will use them in order to avoid situations where Player 2 loses purely because he ran
            // out of backmoves.
            //#define ALLOW_COMPUTER_BACKMOVES 
            if (moveOutcome == winForPlayer(currentPlayer))
            {
#ifdef ALLOW_COMPUTER_BACKMOVES
                // Let's make a Backmove - just to show that's it's possible!
                for (const auto& move : moves)
                {
                    if (move.isBackmove)
                        return move;
                }
#else
                // Pick a move where we don't backmove, for illustration purposes :)
                for (const auto& move : moves)
                {
                    if (!move.isBackmove)
                        return move;
                }
                assert(false);
#endif
            }
            // Pick the first one, arbitrarily - feel free to add your own preferences here :)
            return moves.front();
        }
};

ostream& operator<<(ostream& os, const GameState& gameState)
{
    os << endl;
    os << "Player 1 RowForColumn: " << endl;
    for (const auto pos : gameState.player1RowForColumn)
        os << " " << pos;
    os << endl;
    os << "Player 2 RowForColumn: " << endl;
    for (const auto pos : gameState.player2RowForColumn)
        os << " " << pos;
    os << endl;
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
        int minRowToMoveTo = -1;
        int maxRowToMoveTo = -1;
        if (currentPlayer == Player1)
        {
            if (gameState.player1RowForColumn[column] < gameState.player2RowForColumn[column])
            {
                minRowToMoveTo = 1;
                maxRowToMoveTo = gameState.player2RowForColumn[column] - 1;
            }
            else
            {
                minRowToMoveTo = gameState.player2RowForColumn[column] + 1;
                maxRowToMoveTo = numRows;
            }
        }
        else
        {
            if (gameState.player2RowForColumn[column] < gameState.player1RowForColumn[column])
            {
                minRowToMoveTo = 1;
                maxRowToMoveTo = gameState.player1RowForColumn[column] - 1;
            }
            else
            {
                minRowToMoveTo = gameState.player1RowForColumn[column] + 1;
                maxRowToMoveTo = numRows;
            }
        }
        for (int rowToMoveTo = minRowToMoveTo; rowToMoveTo <= maxRowToMoveTo; rowToMoveTo++)
        {
            GameState stateAfterMove = gameState;
            const int originalRookDistance = abs(gameState.player1RowForColumn[column] - gameState.player2RowForColumn[column]);
            int originalRow = -1;
            if (currentPlayer == Player1)
            {
                originalRow = gameState.player1RowForColumn[column];
                stateAfterMove.player1RowForColumn[column] = rowToMoveTo;
            }
            else
            {
                originalRow = gameState.player2RowForColumn[column];
                stateAfterMove.player2RowForColumn[column] = rowToMoveTo;
            }
            const int rookDistanceAfterMove = abs(stateAfterMove.player1RowForColumn[column] - stateAfterMove.player2RowForColumn[column]);
            const bool isBackmove = (rookDistanceAfterMove > originalRookDistance);
            if (stateAfterMove.player1RowForColumn == gameState.player1RowForColumn && stateAfterMove.player2RowForColumn == gameState.player2RowForColumn)
                continue;

            Move move;
            move.column = column;
            move.dy = rowToMoveTo - originalRow;
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
        nextGameState.player1RowForColumn[move.column] += move.dy;
    else
        nextGameState.player2RowForColumn[move.column] += move.dy;


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
#ifdef PRINT_COMPUTER_MOVES
            if (!isBruteForceMoveSearch)
            {
                cout << "Computer's turn; thinking ..." << endl;
            }
#endif

            for (const auto& move : availableMoves)
            {
                const bool moveIsWinForCurrentPlayer = !gameStateAfterMove(gameState, currentPlayer, move).isWinForCurrentPlayer();
                if (moveIsWinForCurrentPlayer)
                {
                    winningMovesForCurrentPlayer.push_back(move);
                    playState = winForPlayer(currentPlayer);
                }
                else
                    losingMovesForCurrentPlayer.push_back(move);
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

    if (playStateForLookup.size() % 10'000 == 0)
    {
        cout << "Processed " << playStateForLookup.size() << " out of at most " << GameState::numGameStatesOverEstimate << endl;
    }

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
    srand(time(0));

    GameState initialGameState;
    // With this particular arrangement of Rooks, Player 1 is guaranteed to Win; you, poor human,
    // will be playing as Player 2 :)
    // Try to either make Player 1 lose or to prolong your defeat indefinitely, and see how the computer player
    // counters your strategy :)
    initialGameState.player1RowForColumn = { 1, 1, 1, 1, 1, 1};
    initialGameState.player2RowForColumn = { 6, 6, 2, 2, 2, 2};
    initialGameState.numRows = 6;
    initialGameState.numColumns = 6;
    initialGameState.validate();
    cout << "Bloop: " << initialGameState.isWinForCurrentPlayer() << endl;
    cout << "^^^" << endl;

    // Player 2 goes first!
    const auto result = findWinner(Player2, initialGameState, CPU, Human);

    cout << "Result: " << result << endl;
    assert(result == Player1Win);
}

