// Simon St James (ssjgz) - 2017-12-03.  Framework for exploring "Optimal Play" Game Theory games, complete with example ("Move The Coins").
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <cassert>

using namespace std;

//#define VERBOSE

#define MOVE_COINS_EXAMPLE
//#define MOVE_COINS_EXAMPLE_RANDOM
#ifdef MOVE_COINS_EXAMPLE
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
#endif

enum PlayState { Player1Win, Player1Lose };
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
#ifdef MOVE_COINS_EXAMPLE
        vector<int> coins;
#endif
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
#ifdef MOVE_COINS_EXAMPLE
    return lhs.coins < rhs.coins;
#else
    assert(false && "Fill me in - GameState operator<");
#endif
}

class Move
{
    public:
#ifdef MOVE_COINS_EXAMPLE
        int numCoins = -1;
        int nodeIdToTakeFrom = -1;
        int nodeIdToAddTo = -1;
#endif
};

ostream& operator<<(ostream& os, const GameState& gameState)
{
#ifdef MOVE_COINS_EXAMPLE
    for (const auto x : gameState.coins)
    {
        os << x << " ";
    }
#endif
    return os;
}

ostream& operator<<(ostream& os, const Move& move)
{
#ifdef MOVE_COINS_EXAMPLE
    os << "Take " << move.numCoins << " from node: " << (move.nodeIdToTakeFrom + 1) << " and place in " << (move.nodeIdToAddTo + 1);
#endif
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
#ifdef MOVE_COINS_EXAMPLE
    for (int nodeId = 0; nodeId < nodes.size(); nodeId++)
    {
        if (nodes[nodeId].parent == nullptr)
            continue;

        const int parentNodeId = nodes[nodeId].parent->nodeId;
        if (gameState.coins[nodeId] > 0)
        {
            for (int coinsToMove = 1; coinsToMove <= gameState.coins[nodeId]; coinsToMove++)
            {
                Move move;
                move.numCoins = coinsToMove;
                move.nodeIdToTakeFrom = nodeId;
                move.nodeIdToAddTo = parentNodeId;

                moves.push_back(move);
            }
        }
    }
#else
    assert(false && "Fill me in - movesFor");
#endif

    return moves;
}

GameState gameStateAfterMove(const GameState& gameState, Player currentPlayer, const Move& move)
{
    GameState nextGameState;
#ifdef MOVE_COINS_EXAMPLE
    nextGameState.coins = gameState.coins;
    nextGameState.coins[move.nodeIdToAddTo] += move.numCoins;
    nextGameState.coins[move.nodeIdToTakeFrom] -= move.numCoins;
#else
    assert(false && "Fill me in - gameStateAfterMove");
#endif

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
#ifdef VERBOSE
                    cout << "The move " << move << " from state: " << gameState << " is a win for player " << currentPlayer << endl;
#endif
                }
            }
            if (!isBruteForceMoveSearch && !availableMoves.empty())
            {
                cout << "Computer (" << currentPlayer << ") played move: " << chosenMove << " and thinks it will " << (playState == winForPlayer(currentPlayer) ? "Win" : "Lose") << ". Game state now: " <<  gameStateAfterMove(gameState, currentPlayer, chosenMove) << endl;
                updatePlayStateFromMove(chosenMove, false);
            }
        }
    }

#ifdef VERBOSE
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
#ifdef MOVE_COINS_EXAMPLE
    int n;
    testCaseIn >> n;

    nodes.resize(n);
    for (int i = 0; i < n; i++)
    {
        int numCoinsOnNode = 0;
        testCaseIn >> numCoinsOnNode;

        nodes[i].numCoins = numCoinsOnNode;
        nodes[i].nodeId = i;
    }

    for (int i = 0; i < n - 1; i++)
    {
        int u, v;
        testCaseIn >> u >> v;
        u--;
        v--;

        nodes[u].neighbours.push_back(&(nodes[v]));
        nodes[v].neighbours.push_back(&(nodes[u]));
    }

    auto rootNode = &(nodes.front());
    fixTree(rootNode, 0, nullptr);

    printTree(nodes);

#ifdef MOVE_COINS_EXAMPLE_RANDOM
    while (true)
    {
        GameState initialState;
        int oddHeightXor = 0;
        for (int i = 0; i < n; i++)
        {
            initialState.coins.push_back(rand() % 6);
            if ((nodes[i].height % 2) == 1)
            {
                oddHeightXor ^= initialState.coins.back();
            }
        }
        const auto result = findWinner(Player1, initialState);
        //cout << "Win for first player: " << (firstPlayerWins ? "Yes" : "No") << " oddHeightXor: " << oddHeightXor << endl;
        cout << result << "  oddHeightXor: " << oddHeightXor << endl;
        assert((result == Player1Win) == (oddHeightXor != 0));
    }
#else
    GameState initialState;
    initialState.coins.resize(n);
    for (const auto node : nodes)
    {
        initialState.coins[node.nodeId] = node.numCoins;
    }
    const auto result = (findWinner(Player1, initialState, CPU, Random));
    cout << result << endl;
#endif
#endif
}

