// Simon St James (ssjgz) - 2018-01-12.
#include <iostream>
#include <vector>
#include <map>
#include <array>
#include <sstream>
#include <fstream>
#include <limits>
#include <cassert>

using namespace std;

//#define VERY_VERBOSE
#define PRINT_COMPUTER_MOVES
enum PlayState { Player1Win, Player1Lose };
enum Player { Player1, Player2 };

enum PlayerType { CPU, Random, Human };

const string player1Name = "Balsa";
const string player2Name = "Koca";

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
        array<int, 3> numWithMod3 = {};
        int balsaScore = 0;
        int kocaScore = 0;
        bool hasWinningPlayerOverride(Player currentPlayer) const
        {
            return movesFor(currentPlayer, *this).empty();
        }
        Player winningPlayerOverride(Player currentPlayer) const
        {
            //cout << "state override: balsaScore: " << balsaScore << " kocaScore: " << kocaScore << endl;
            const int balsaScoreMod3 = balsaScore % 3;
            const int kocaScoreMod3 = kocaScore % 3;
            const int balsaMinusKocaMod3 = (3 + balsaScoreMod3 - kocaScoreMod3) % 3;
            //cout << "balsaMinusKocaMod3: " << balsaMinusKocaMod3 << endl;
            return (balsaMinusKocaMod3 == 0 ? Player2 : Player1);
        }
};

bool operator<(const GameState& lhs, const GameState& rhs)
{
    if (lhs.balsaScore != rhs.balsaScore)
        return lhs.balsaScore < rhs.balsaScore;
    if (lhs.kocaScore != rhs.kocaScore)
        return lhs.kocaScore < rhs.kocaScore;
    return lhs.numWithMod3 < rhs.numWithMod3;
}

class Move
{
    public:
        int takeFromWithMod3 = -1;
};

ostream& operator<<(ostream& os, const GameState& gameState)
{
    os << "piles: " << gameState.numWithMod3[0] << " "<< gameState.numWithMod3[1] << " " << gameState.numWithMod3[2] << " balsa score: " << gameState.balsaScore << " kocaScore: " << gameState.kocaScore << endl;
    return os;
}

ostream& operator<<(ostream& os, const Move& move)
{
    os << "Take from pile " << move.takeFromWithMod3;
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
    for (int i = 0; i < 3; i++)
    {
        if (gameState.numWithMod3[i] > 0)
        {
            Move move;
            move.takeFromWithMod3 = i;
            moves.push_back(move);
        }
    }

    //cout << "num moves for " << gameState << " : " << moves.size() << endl;

    return moves;
}

GameState gameStateAfterMove(const GameState& gameState, Player currentPlayer, const Move& move)
{
    GameState nextGameState(gameState);
    nextGameState.numWithMod3[move.takeFromWithMod3]--;
    if (currentPlayer == Player1)
    {
        nextGameState.balsaScore += move.takeFromWithMod3;
    }
    else
    {
        nextGameState.kocaScore += move.takeFromWithMod3;
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
        GameState initialState;
#if 1
        int n;
        testCaseIn >> n;

        vector<int> a(n);
        for (int i = 0; i < n; i++)
        {
            testCaseIn >> a[i];
        }

        for (const auto x : a)
        {
            //cout << "x: " << x << endl;
            initialState.numWithMod3[x % 3]++;
        }
        if (initialState.numWithMod3[1] % 2 == 0 && initialState.numWithMod3[2] % 2 == 0)
        {
            cout << "Koca";
        }
        else
        {
            cout << "Balsa";
        }
        cout << endl;
#else
        int x, y, z;
        testCaseIn >> x >> y >> z;
        initialState.numWithMod3[0] = x;
        initialState.numWithMod3[1] = y;
        initialState.numWithMod3[2] = z;
        cout << "0: " << initialState.numWithMod3[0] << " 1: " << initialState.numWithMod3[1] << " 2: " << initialState.numWithMod3[2] << endl;
#endif
#if 0
        //const auto result = findWinner(Player1, initialState);
        const auto result = findWinner(Player1, initialState, CPU, Human);
        if (result == Player1Win)
        {
            cout << "Balsa";
        }
        else
        {
            cout << "Koca";
        }
        cout << endl;
#endif
    }
}

