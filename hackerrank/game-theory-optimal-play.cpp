#include <iostream>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

#define VERBOSE

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

PlayState findWinnerAux(Player currentPlayer, const GameState& gameState)
{
    if (playStateForLookup.find({gameState, currentPlayer}) != playStateForLookup.end())
    {
        return playStateForLookup[{gameState, currentPlayer}];
    }

    PlayState playState = loseForPlayer(currentPlayer);
    if (gameState.hasWinningPlayerOverride(currentPlayer))
    {
        playState = winForPlayer(gameState.winningPlayerOverride(currentPlayer));
    }
    else
    {
        const vector<Move> availableMoves = movesFor(currentPlayer, gameState);

        for (const auto& move : availableMoves)
        {
            const auto newGameState = gameStateAfterMove(gameState, currentPlayer, move);
            const auto result = findWinnerAux(otherPlayer(currentPlayer), newGameState);
            if (result == winForPlayer(currentPlayer))
            {
#ifdef VERBOSE
                if (playState != winForPlayer(currentPlayer))
                {
                    // Print out the winning move, but only once.
                    cout << "The move " << move << " from state: " << gameState << " is a win for player " << currentPlayer << endl;
                }
#endif
                playState = winForPlayer(currentPlayer);
            }
        }
    }

#ifdef VERBOSE
    cout << "At game state: " << gameState << " with player " << currentPlayer << ", the player " << (playState == winForPlayer(currentPlayer) ? "Wins" : "Loses") << endl;
#endif
    playStateForLookup[{gameState, currentPlayer}] = playState;

    return playState;
}

PlayState findWinner(Player currentPlayer, const GameState& gameState)
{
    return findWinnerAux(currentPlayer, gameState);
}

int main()
{
#ifdef MOVE_COINS_EXAMPLE
    int n;
    cin >> n;

    nodes.resize(n);
    for (int i = 0; i < n; i++)
    {
        int numCoinsOnNode = 0;
        cin >> numCoinsOnNode;

        nodes[i].numCoins = numCoinsOnNode;
        nodes[i].nodeId = i;
    }

    for (int i = 0; i < n - 1; i++)
    {
        int u, v;
        cin >> u >> v;
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
    const auto result = (findWinner(Player1, initialState));
    cout << result << endl;
#endif
#endif
}

