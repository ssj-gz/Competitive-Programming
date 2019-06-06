// Simon St James (ssjgz) - 2017-12-03.  Framework for exploring "Optimal Play" Game Theory games, complete with example ("Move The Coins").
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <sys/time.h>
#include <cassert>

using namespace std;

//#define VERY_VERBOSE
#define PRINT_COMPUTER_MOVES

enum PlayState { Player1Win, Player1Lose };
enum Player { Player1, Player2 };

const string player1Name = "Laurel";
const string player2Name = "Hardy";

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
        vector<int> numInPile;
        bool hasWinningPlayerOverride(Player currentPlayer) const
        {
            // Assume that a GameState that has no moves that lead to a Lose for the
            // other player is a losing state (and vice-versa).
            return false;
        }
        Player winningPlayerOverride(Player currentPlayer) const
        {
            assert(false);
            return Player1;
        }
};

bool operator<(const GameState& lhs, const GameState& rhs)
{
    return lhs.numInPile < rhs.numInPile;
}

class Move
{
    public:
        int pileToTakeFrom = -1;
        int numToTake = -1;
};

ostream& operator<<(ostream& os, const GameState& gameState)
{
    for (const auto numInPile : gameState.numInPile)
    {
        os << numInPile << " ";
    }
    os << " (";
    os << gameState.numInPile[0] << " ";
    for (int i = 1; i < gameState.numInPile.size(); i++)
    {
        os << (gameState.numInPile[i] - gameState.numInPile[i - 1]) << " ";
    }
    os << ")";
    return os;
}

ostream& operator<<(ostream& os, const Move& move)
{
    os << "Take " << move.numToTake << " stones from pile # " << move.pileToTakeFrom;
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
        os << Player1;
    else
        os << Player2;

    os << " wins";

    return os;
}

map<pair<GameState, Player>, PlayState> playStateForLookup;

vector<Move> movesFor(Player currentPlayer, const GameState& gameState)
{
    vector<Move> moves;
    for (int pileNum = 0; pileNum < gameState.numInPile.size(); pileNum++)
    {
        for (int numToTake = 1; numToTake <= gameState.numInPile[pileNum] && (pileNum == 0 || gameState.numInPile[pileNum] - numToTake >= gameState.numInPile[pileNum - 1]); numToTake++)
        {
            Move move;
            move.pileToTakeFrom = pileNum;
            move.numToTake = numToTake;
            moves.push_back(move);
        }
    }
    return moves;
}

GameState gameStateAfterMove(const GameState& gameState, Player currentPlayer, const Move& move)
{
    GameState nextGameState(gameState);
    nextGameState.numInPile[move.pileToTakeFrom] -= move.numToTake;

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

    auto updatePlayStateFromMove = [&playState, &gameState, currentPlayer, player1Type, player2Type](const Move& move, bool isBruteForceMoveSearch)
    {
        const auto newGameState = gameStateAfterMove(gameState, currentPlayer, move);
        const auto result = findWinnerAux(otherPlayer(currentPlayer), newGameState, player1Type, player2Type, isBruteForceMoveSearch);
        if (result == winForPlayer(currentPlayer))
        {
            playState = winForPlayer(currentPlayer);
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
        playState = winForPlayer(gameState.winningPlayerOverride(currentPlayer));
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
            Move chosenMove;
            for (const auto& move : availableMoves)
            {
                const auto oldPlayState = playState;
                updatePlayStateFromMove(move, true);

                if (playState != winForPlayer(currentPlayer))
                {
                    // If we can't win, just play an arbitrary move; the last one, say.
                    chosenMove = move;
                }
                else if (oldPlayState != playState)
                {
                    // This is the first winning move we've discovered; store it and print it out (but not subsequent ones).
                    chosenMove = move;
#ifdef VERY_VERBOSE
                    cout << "The move " << move << " from state: " << gameState << " is a win for player " << currentPlayer << endl;
#endif
                }
            }
            if (!isBruteForceMoveSearch && !availableMoves.empty())
            {
#ifdef PRINT_COMPUTER_MOVES
                cout << "Computer (" << currentPlayer << ") played move: " << chosenMove << " and thinks it will " << (playState == winForPlayer(currentPlayer) ? "Win" : "Lose") << ". Game state now: " <<  gameStateAfterMove(gameState, currentPlayer, chosenMove) << endl;
#endif
                updatePlayStateFromMove(chosenMove, false);
            }
        }
    }

#ifdef VERY_VERBOSE
    cout << "At game state: " << gameState << " with player " << currentPlayer << ", the player " << (playState == winForPlayer(currentPlayer) ? "Wins" : "Loses") << endl;
#endif
    playStateForLookup[{gameState, currentPlayer}] = playState;

    return playState;
}

PlayState findWinner(Player currentPlayer, const GameState& gameState, PlayerType player1Type = CPU, PlayerType player2Type = CPU)
{
    playStateForLookup.clear();
    const auto result = findWinnerAux(currentPlayer, gameState, player1Type, player2Type, false);

    if (player1Type == Human || player2Type == Human)
    {
        const auto interactivePlayer = (player1Type == Human ? Player1 : Player2);
        cout << "Result of interactive game: you played as " << interactivePlayer << "; " << currentPlayer << " took the first turn; " << result << endl;
    }

    return result;
}


int main(int argc, char** argv)
{
    struct timeval time; 
    gettimeofday(&time,NULL);

    // microsecond has 1 000 000
    // Assuming you did not need quite that accuracy
    // Also do not assume the system clock has that accuracy.
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
#if 0
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
#endif
    if (argc == 2)
    {
        const int n = rand() % 9 + 1;
        cout << n << endl;
        vector<int> nums;
        for (int i = 0; i < n; i++)
        {
            nums.push_back(rand() % 20 + 1);
        }
        sort(nums.begin(), nums.end());
        for (const auto x : nums)
        {
            cout << x << " ";
        }
        return 0;
    }


    istream& testCaseIn = cin; 
    int n;
    testCaseIn >> n;

    vector<int> numInPile(n);

    GameState initialGameState;
    initialGameState.numInPile.resize(n);
    for (int i = 0; i < n; i++)
    {
        testCaseIn >> numInPile[i];
        if (i > 0)
            assert(numInPile[i] >= numInPile[i - 1]);

        initialGameState.numInPile[i] = numInPile[i];
    }

    int xorSum = 0;
#if 0
    for (int i = n - 1; i >= 0; i -= 2)
    {
        if (i > 0)
        {
            const auto diff = (initialGameState.numInPile[i] - initialGameState.numInPile[i - 1]);
            xorSum ^= diff;
            cout << "i: " << i << " diff: " << diff << endl;
        }
    }
#endif
    vector<int> diffs;
    diffs.push_back(initialGameState.numInPile[0]);
    for (int i = 1; i < n; i++)
    {
        const auto diff = (initialGameState.numInPile[i] - initialGameState.numInPile[i - 1]);
        diffs.push_back(diff);
    }
    for (int i = diffs.size() - 1; i >= 0; i -= 2)
    {
        xorSum ^= diffs[i];
    }

    cout << "Initial game state: " << initialGameState << endl;
    cout << "xorSum: " << xorSum << endl;
#if 0
    cout << "Diffs: " << endl;
    int diffSum = 0;
    for (int i = 1; i < n; i++)
    {
        const int diff = initialGameState.numInPile[i] - initialGameState.numInPile[i - 1];
        cout << diff << " ";
        diffSum += diff;
    }
    cout << endl;

    cout << "diffSum % 2: " << (diffSum % 2) << endl;
#endif
    const auto winner = findWinner(Player1, initialGameState, CPU, CPU);

    cout << "Winner: " << winner << " xorSum: " << xorSum << endl;

    assert((winner == Player1Win) == (xorSum != 0));
}

