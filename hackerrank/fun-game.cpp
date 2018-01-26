// Simon St James (ssjgz) - 2018-01-26
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
//#define PRINT_COMPUTER_MOVES

struct Node
{
    vector<Node*> children;
    Node* parent = nullptr;
    int numCoins = 0;
    int nodeId = -1;
    int height = -1;

    vector<Node*> neighbours;
};

void fixTree(Node* rootNode, int height, Node* parent)
{
    rootNode->parent = parent;
    rootNode->height = height;
    for (Node* child : rootNode->neighbours)
    {
        if (child == parent)
            continue;

        rootNode->children.push_back(child);
        fixTree(child, height + 1, rootNode);
    }
    rootNode->neighbours.clear();
}

void printTree(const vector<Node>& nodes)
{
    for (const auto& node : nodes)
    {
        cout << "Node: " << (node.nodeId + 1) << " coins: " << node.numCoins << " parent: " << (node.parent == nullptr ? -1 : node.parent->nodeId + 1) << " children: " ;
        for (const auto& child : node.children)
        {
            cout << (child->nodeId + 1) << " ";

        }
        cout << endl;
    }
}

vector<Node> nodes;

enum PlayState { Player1Win, Player1Lose, Draw };
enum Player { Player1, Player2 };

const string player1Name = "First";
const string player2Name = "Second";

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
        vector<int> A;
        vector<int> B;
        vector<bool> isAvailable;
        int player1Score = 0;
        int player2Score = 0;
        bool hasWinningPlayerOverride(Player currentPlayer) const
        {
            return movesFor(currentPlayer, *this).empty();
        }
        PlayState winningPlayerOverride(Player currentPlayer) const
        {
            if (player1Score > player2Score)
                return Player1Win;
            else if (player1Score < player2Score)
                return Player1Lose;

            assert(player1Score == player2Score);
            return Draw;
        }
};

bool operator<(const GameState& lhs, const GameState& rhs)
{
    if (lhs.player1Score != rhs.player1Score)
        return lhs.player1Score < rhs.player1Score;
    if (lhs.player2Score != rhs.player2Score)
        return lhs.player2Score < rhs.player2Score;
    if (lhs.A != lhs.A)
        return lhs.A < rhs.A;
    if (lhs.B != lhs.B)
        return lhs.B < rhs.B;
    return lhs.isAvailable < rhs.isAvailable;
}

class Move
{
    public:
        int indexToTake = -1;
        bool takeFromA = false;
};

ostream& operator<<(ostream& os, const GameState& gameState)
{
    os << endl;
    os << "Player 1 Score: " << gameState.player1Score << " Player 2 Score: " << gameState.player2Score << endl;
    const int n = gameState.A.size();
    for (int i = 0; i < n; i++)
    {
        if (gameState.isAvailable[i])
            os << gameState.A[i];
        else
            os << "X";
        os << " ";
    }
    os << endl;
    for (int i = 0; i < n; i++)
    {
        if (gameState.isAvailable[i])
            os << gameState.B[i];
        else
            os << "X";
        os << " ";
    }
    os << endl;
    return os;
}

ostream& operator<<(ostream& os, const Move& move)
{
    os << "Take index " << move.indexToTake << " from pile " << (move.takeFromA ? "A" : "B");
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
    for (int i = 0; i < gameState.isAvailable.size(); i++)
    {
        if (gameState.isAvailable[i])
        {
            Move move;
            move.indexToTake = i;
            move.takeFromA = (currentPlayer == Player1);
            moves.push_back(move);
        }
    }

    return moves;
}

GameState gameStateAfterMove(const GameState& gameState, Player currentPlayer, const Move& move)
{
    GameState nextGameState(gameState);
    nextGameState.isAvailable[move.indexToTake] = false;
    if (move.takeFromA)
    {
        nextGameState.player1Score += gameState.A[move.indexToTake];
    }
    else
    {
        nextGameState.player2Score += gameState.B[move.indexToTake];
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
            Move chosenMove;
            for (const auto& move : availableMoves)
            {
                const auto oldPlayState = playState;
                updatePlayStateFromMove(move, true);

                if (playState != winForPlayer(currentPlayer) && playState != Draw)
                {
                    // If we can't win, just play an arbitrary move; the last one, say.
                    chosenMove = move;
                }
                else if ((playState == winForPlayer(currentPlayer) && oldPlayState != playState) || (playState == Draw && oldPlayState != playState) )
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

PlayState findWinner(Player currentPlayer, const GameState& gameState, PlayerType player1Type = CPU, PlayerType player2Type = CPU)
{
    cout << "Initial state: " << gameState << endl;
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
#if 0
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
#else

    srand(time(0));
    if (argc == 2)
    {
        const int n = rand() % 10 + 1;
        cout << 1 << " " << n << endl;
        for (int row = 0; row < 2; row++)
        {
            for (int i = 0; i < n; i++)
            {
                cout << (rand() % 20 + 1) << " ";
            }
            cout << endl;
        }
        return 0;
    }
#endif
    istream& testCaseIn = cin;

    int T;
    testCaseIn >> T;

    for (int t = 0; t < T; t++)
    {
        cout << "** t: " << t << endl;
        int n;
        testCaseIn >> n;

        GameState initialGameState;

        initialGameState.isAvailable.resize(n, true);
        initialGameState.A.resize(n);
        initialGameState.B.resize(n);
        for (int i = 0; i < n; i++)
            testCaseIn >> initialGameState.A[i];
        for (int i = 0; i < n; i++)
            testCaseIn >> initialGameState.B[i];

        const auto result = findWinner(Player1, initialGameState, CPU, CPU);

        struct Sum
        {
            int sum = 0;
            int originalIndex = -1;
        };
        vector<Sum> sums;
        for (int i = 0; i < n; i++)
        {
            Sum sum;
            sum.sum = initialGameState.A[i] + initialGameState.B[i];
            sum.originalIndex = i;
            sums.push_back(sum);
        }
        sort(sums.begin(), sums.end(), [](const auto& lhs, const auto& rhs) { return lhs.sum < rhs.sum;});

        bool isPlayer1 = true;
        int player1Score = 0;
        int player2Score = 0;
        while (!sums.empty())
        {
            if (isPlayer1)
            {
                player1Score += initialGameState.A[sums.back().originalIndex];
            }
            else
            {
                player2Score += initialGameState.B[sums.back().originalIndex];
            }
            sums.pop_back();
            isPlayer1 = !isPlayer1;
        }
        cout << "Estimate: player1Score: " << player1Score << " player2Score: " << player2Score << endl;

        if (result == Draw)
            assert(player1Score == player2Score);
        else if (result == Player1Win)
            assert(player1Score > player2Score);
        else if (result == Player1Lose)
            assert(player1Score < player2Score);


        cout << "Result: " << result << endl;
    }
}

