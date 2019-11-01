// Simon St James (ssjgz) - 2017-12-03.  Framework for exploring "Optimal Play" Game Theory games, complete with example ("Move The Coins").
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <sys/time.h>
#include <cassert>

using namespace std;

//#define VERY_VERBOSE
#define PRINT_COMPUTER_MOVES

enum PlayState { Player1Win, Player1Lose, Draw };
enum Player { Player1, Player2 };

const string player1Name = "Player 1";
const string player2Name = "Player 2";

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
        vector<int> numStonesInPile;
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
    return lhs.numStonesInPile < rhs.numStonesInPile;
}

struct StackAndNumStones
{
    int stackIndex = -1;
    int numStonesToTake = -1;
};

class Move
{
    public:
        vector<StackAndNumStones> numStonesToTakeFromStacks;
        static Move preferredMove(const vector<Move>& moves, PlayState moveOutcome, Player currentPlayer, const GameState& gameState)
        {
            assert(!moves.empty());
            // Pick the first one, arbitrarily - feel free to add your own preferences here :)
            return moves.front();
        }
};

ostream& operator<<(ostream& os, const GameState& gameState)
{
    for (const auto x : gameState.numStonesInPile)
    {
        os << x << " ";
    }
    return os;
}

ostream& operator<<(ostream& os, const Move& move)
{
    cout << "Choose " << move.numStonesToTakeFromStacks.size() << " stacks" << endl;
    for (const auto x : move.numStonesToTakeFromStacks)
    {
        os << "Take " << x.numStonesToTake << " from stack: " << x.stackIndex << endl;
    }
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

    // Choose one stack.
    for (int stackIndex = 0; stackIndex < gameState.numStonesInPile.size(); stackIndex++)
    {
        for (int numStonesToTake = 1; numStonesToTake <= gameState.numStonesInPile[stackIndex]; numStonesToTake++)
        {
            Move move;
            move.numStonesToTakeFromStacks.push_back({stackIndex, numStonesToTake});
            moves.push_back(move);
        }
    }
    // Choose two stacks.
    for (int stack1Index = 0; stack1Index < gameState.numStonesInPile.size(); stack1Index++)
    {
        if (gameState.numStonesInPile[stack1Index] == 0)
        {
            continue;
        }
        for (int stack2Index = stack1Index + 1; stack2Index < gameState.numStonesInPile.size(); stack2Index++)
        {
            if (gameState.numStonesInPile[stack2Index] == 0)
            {
                continue;
            }

            for (int numStonesToTake1 = 1; numStonesToTake1 <= gameState.numStonesInPile[stack1Index]; numStonesToTake1++)
            {
                for (int numStonesToTake2 = 1; numStonesToTake2 <= gameState.numStonesInPile[stack2Index]; numStonesToTake2++)
                {
                    Move move;
                    move.numStonesToTakeFromStacks.push_back({stack1Index, numStonesToTake1});
                    move.numStonesToTakeFromStacks.push_back({stack2Index, numStonesToTake2});
                    moves.push_back(move);


                }
            }
        }
    }

    return moves;
}

GameState gameStateAfterMove(const GameState& gameState, Player currentPlayer, const Move& move)
{
    GameState nextGameState;
    nextGameState.numStonesInPile = gameState.numStonesInPile;
    assert(move.numStonesToTakeFromStacks.size() == 1 || move.numStonesToTakeFromStacks.size() == 2);

    for (const auto x : move.numStonesToTakeFromStacks)
    {
        assert(x.stackIndex >= 0 && x.stackIndex < nextGameState.numStonesInPile.size());
        nextGameState.numStonesInPile[x.stackIndex] -= x.numStonesToTake;
        assert(nextGameState.numStonesInPile[x.stackIndex] >= 0);
    }

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
            if (!isBruteForceMoveSearch)
            {
#ifdef PRINT_COMPUTER_MOVES
                cout << "Computer's turn; thinking ..." << endl;
#endif
            }
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

PlayState findWinner(Player firstPlayer, const GameState& initialGameState, PlayerType player1Type = CPU, PlayerType player2Type = CPU)
{
    cout << "Initial game state: " << initialGameState << endl;
    playStateForLookup.clear();
    const auto result = findWinnerAux(firstPlayer, initialGameState, player1Type, player2Type, false);

    if (player1Type == Human || player2Type == Human)
    {
        const auto interactivePlayer = (player1Type == Human ? Player1 : Player2);
        cout << "Result of interactive game: you played as " << interactivePlayer << "; " << firstPlayer << " took the first turn; " << result << endl;
    }

    return result;
}


int main(int argc, char** argv)
{
    struct timeval time; 
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    ifstream testCaseFileIn;
    bool isTestcaseFromFile = false;
    if (argc == 2)
    {
        const auto testCaseFilename = argv[1];
        testCaseFileIn.open(testCaseFilename);
        assert(testCaseFileIn.is_open());
        isTestcaseFromFile = true;
    }
    istream& testCaseIn = (isTestcaseFromFile ? testCaseFileIn : cin);

    int numPiles;
    testCaseIn >> numPiles;

    vector<int> numStonesInPile(numPiles);
    for (auto& pile : numStonesInPile)
        testCaseIn >> pile;

    GameState initialState;
    initialState.numStonesInPile = numStonesInPile;
    const auto result = (findWinner(Player1, initialState, CPU, CPU));
    cout << result << endl;

    set<GameState> gameStates;
    for (const auto& blah : playStateForLookup)
    {
        GameState blee = blah.first.first;
        sort(blee.numStonesInPile.begin(), blee.numStonesInPile.end());
        gameStates.insert(blee);
    }

    for (const auto& gameState : gameStates)
    {
        auto printIsWinner = [](const auto& gameState, const auto player)
        {
            if (playStateForLookup.find({gameState, player}) == playStateForLookup.end())
                return false;
            
            const auto result = playStateForLookup[{gameState, player}];
            const bool isWinForPlayer = (result == winForPlayer(player));
            int numNonEmpty = 0;
            for (const auto x : gameState.numStonesInPile)
            {
                if (x != 0)
                    numNonEmpty++;
            }
            cout << "Game state: " << gameState << " with " << numNonEmpty << " piles is a " << (isWinForPlayer ? "win" : "loss") << " for current player" << endl;
            return true;
        };

        if (printIsWinner(gameState, Player1))
            continue;
        printIsWinner(gameState, Player2);
    }

}

