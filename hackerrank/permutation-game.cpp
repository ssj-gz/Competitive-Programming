// Simon St James (ssjgz) - 2017-12-03
#define BRUTE_FORCE
#define SUBMISSION
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <cassert>

using namespace std;

//#define VERY_VERBOSE
//#define PRINT_PLAYOUTS

enum PlayState { Player1Win, Player1Lose };
enum Player { Player1, Player2 };

const string player1Name = "Alice";
const string player2Name = "Bob";

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
        vector<int> numbers;
        bool hasWinningPlayerOverride(Player currentPlayer) const
        {
            if (is_sorted(numbers.begin(), numbers.end()))
                return true;
            return false;
        }
        Player winningPlayerOverride(Player currentPlayer) const
        {
            return otherPlayer(currentPlayer);
        }
};

bool operator<(const GameState& lhs, const GameState& rhs)
{
    return lhs.numbers < rhs.numbers;
}

class Move
{
    public:
        int takeAtIndex;
};

ostream& operator<<(ostream& os, const GameState& gameState)
{
    for (const auto x : gameState.numbers)
    {
        os << x << " ";
    }
    return os;
}

ostream& operator<<(ostream& os, const Move& move)
{
    os << "Take from " << move.takeAtIndex;
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
    for (int i = 0; i < gameState.numbers.size(); i++)
    {
        Move move;
        move.takeAtIndex = i;
        moves.push_back(move);
    }

    return moves;
}

GameState gameStateAfterMove(const GameState& gameState, Player currentPlayer, const Move& move)
{
    GameState nextGameState(gameState);
    assert(move.takeAtIndex >= 0 && move.takeAtIndex < nextGameState.numbers.size());
    nextGameState.numbers.erase(nextGameState.numbers.begin() + move.takeAtIndex);

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
#ifdef PRINT_PLAYOUTS
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
    //playStateForLookup.clear();
    const auto result = findWinnerAux(currentPlayer, gameState, player1Type, player2Type, false);

    if (player1Type == Human || player2Type == Human)
    {
        const auto interactivePlayer = (player1Type == Human ? Player1 : Player2);
        cout << "Result of interactive game: you played as " << interactivePlayer << "; " << currentPlayer << " took the first turn; " << result << endl;
    }

    return result;
}

bool isWinnerWithRemainingBitset(int remainingNumberBitset, const vector<int>& a, vector<char>& currentPlayerWinsForRemainingBitsetLookup)
{
    assert(remainingNumberBitset != 0);
    if (currentPlayerWinsForRemainingBitsetLookup[remainingNumberBitset] != -1)
        return currentPlayerWinsForRemainingBitsetLookup[remainingNumberBitset];

    bool isWin = false;

    const int n = a.size();

    const int maxPowerOf2 = (1 << (n - 1));
    int last = -1;
    bool inOrder = true;
    auto powerOf2 = maxPowerOf2;
    for (int i = 0; i < n; i++)
    {
        if ((remainingNumberBitset & powerOf2) != 0)
        {
            if (last != -1 && a[i] < last)
            {
                inOrder = false;
                break;
            }
            last = a[i];

        }
        powerOf2 >>= 1;
    }

    if (!inOrder)
    {
        powerOf2 = maxPowerOf2;
        for (int i = 0; i < n; i++)
        {
            if ((remainingNumberBitset & powerOf2) != 0)
            {
                const int nextRemainingNumberBitset = remainingNumberBitset - (remainingNumberBitset & powerOf2);
                if (!isWinnerWithRemainingBitset(nextRemainingNumberBitset, a, currentPlayerWinsForRemainingBitsetLookup))
                {
                    isWin =  true;
                }
            }
            powerOf2 >>= 1;
        }
    }

    currentPlayerWinsForRemainingBitsetLookup[remainingNumberBitset] = isWin;
    return isWin;
}


int main(int argc, char** argv)
{
    srand(time(0));
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
    
    int T;
    testCaseIn >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        testCaseIn >> n;

        vector<int> a(n);

        vector<char> currentPlayerWinsForRemainingBitsetLookup(1 << n, -1);
        for (int i = 0; i < n; i++)
        {
            testCaseIn >> a[i];
        }

        const int allRemainingBitset = (1 << n) - 1;
        const auto aliceWins = isWinnerWithRemainingBitset(allRemainingBitset, a, currentPlayerWinsForRemainingBitsetLookup);
        if (aliceWins)
            cout << "Alice";
        else
            cout << "Bob";
        cout << endl;
#ifdef BRUTE_FORCE
        GameState initialState;
        initialState.numbers = a;
        const auto resultBruteForce = findWinner(Player1, initialState, CPU, CPU);
        if (resultBruteForce == Player1Win)
            cout << "Brute Force - Alice";
        else
            cout << "Brute Force - Bob";
        cout << endl;
        assert(aliceWins == (resultBruteForce == Player1Win));
#endif
    }

}

