// Very, very brute force implementation - worse than O(2^N) :)
// Expected to pass Subtask #1 only.
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <cassert>

using namespace std;

struct Node
{
    int index = -1;
    int numCounters = -1;
    Node* parent = nullptr;
    vector<Node*> children;
};

struct Query
{
    Node* nodeToMove = nullptr;
    Node* newParent = nullptr;
};

void fixParentChild(Node* node, Node* parent = nullptr)
{
    node->parent = parent;

    node->children.erase(remove(node->children.begin(), node->children.end(), parent), node->children.end());

    for (auto child : node->children)
    {
        fixParentChild(child, node);
    }
}

void reparentNode(Node* nodeToMove, Node* newParent)
{
    auto oldParent = nodeToMove->parent;
    if (oldParent)
    {
        assert(find(oldParent->children.begin(), oldParent->children.end(), nodeToMove) != oldParent->children.end());
        oldParent->children.erase(remove(oldParent->children.begin(), oldParent->children.end(), nodeToMove), oldParent->children.end());
        assert(find(oldParent->children.begin(), oldParent->children.end(), nodeToMove) == oldParent->children.end());
    }
    nodeToMove->parent = newParent;
    if (newParent)
    {
        assert(find(newParent->children.begin(), newParent->children.end(), nodeToMove) == newParent->children.end());
        newParent->children.push_back(nodeToMove);
    }
}

enum class Result { Unknown, CurrentPlayerWins, CurrentPlayerLoses };
bool currentPlayerWins(const vector<int>& state, Node* rootNode, map<vector<int>, Result>& resultLookup, const vector<vector<Node*>>& allowedMovesForNodeLookup)
{
    if (resultLookup[state] != Result::Unknown)
    {
        return resultLookup[state] == Result::CurrentPlayerWins;
    }

    bool currentPlayerHasWinningMove = false;
    const int numNodes = state.size();
    for (int nodeIndex = 0; nodeIndex < numNodes; nodeIndex++)
    {
        if (state[nodeIndex] > 0)
        {
            for (auto destNode : allowedMovesForNodeLookup[nodeIndex])
            {
                vector<int> nextState = state;
                nextState[nodeIndex]--;
                nextState[destNode->index]++;
                const auto isWinForPlayer = (!currentPlayerWins(nextState, rootNode, resultLookup, allowedMovesForNodeLookup));
                if (isWinForPlayer)
                    currentPlayerHasWinningMove = true;
            }

        }
    }

    resultLookup[state] = (currentPlayerHasWinningMove ? Result::CurrentPlayerWins : Result::CurrentPlayerLoses);

    return currentPlayerHasWinningMove;
};

bool firstPlayerWins(const vector<Node>& nodes, Node* rootNode)
{
    const int numNodes = nodes.size();
    vector<Node*> parentLookup(numNodes, nullptr);

    vector<vector<Node*>> allowedMovesForNodeLookup(numNodes);
    for (const auto& node : nodes)
    {
        vector<Node*> allowedMoves;
        Node* currentNode = node.parent;
        while (currentNode)
        {
            allowedMoves.push_back(currentNode);
            currentNode = currentNode->parent;
        }
        allowedMovesForNodeLookup[node.index] = allowedMoves;
    }

    vector<int> initialState;
    for (const auto& node : nodes)
    {
        initialState.push_back(node.numCounters);
    }

    map<vector<int>, Result> resultLookup;

    return currentPlayerWins(initialState, rootNode, resultLookup, allowedMovesForNodeLookup);
}

vector<bool> calcDoesBobWinQueryBruteForce(vector<Node>& nodes, const vector<Query>& queries)
{
    auto rootNode = &(nodes.front());
    int queryNum = 0;
    vector<bool> doesBobWinQuery;
    for (const auto& query : queries)
    {
        auto originalParent = query.nodeToMove->parent;
        reparentNode(query.nodeToMove, query.newParent);
        doesBobWinQuery.push_back(!firstPlayerWins(nodes, rootNode));
        reparentNode(query.nodeToMove, originalParent);
        queryNum++;
        if (queryNum % 100 == 0)
            cerr << "Brute force queryNum: " << queryNum << " out of " << queries.size() << endl;
    }
    return doesBobWinQuery;
}

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main()
{
    auto readInt = [](){ int x; cin >> x; return x; };

    const auto numTestcases = readInt();
    for (int t = 0; t < numTestcases; t++)
    {
        const auto numNodes = readInt();
        vector<Node> nodes(numNodes);

        for (auto edgeNum = 0; edgeNum < numNodes - 1; edgeNum++)
        {
            const auto node1Index = readInt() - 1;
            const auto node2Index = readInt() - 1;

            nodes[node1Index].children.push_back(&(nodes[node2Index]));
            nodes[node2Index].children.push_back(&(nodes[node1Index]));
        }

        int nodeIndex = 0;
        for (auto& node : nodes)
        {
            node.index = nodeIndex;
            node.numCounters = readInt();

            nodeIndex++;
        }

        const auto numQueries = read<int>();

        vector<Query> queries(numQueries);

        for (int i = 0; i < numQueries; i++)
        {
            const auto u = read<int>() - 1;
            const auto v = read<int>() - 1;

            queries[i].nodeToMove = &(nodes[u]);
            queries[i].newParent = &(nodes[v]);
        }

        auto rootNode = &(nodes.front());
        fixParentChild(rootNode);

        const auto doesBobWinQuery = calcDoesBobWinQueryBruteForce(nodes, queries);
        const int64_t MOD = 1'000'000'007;
        int64_t result = 0;
        int64_t powerOf2 = 2; // 2 to the power of 1.
        for (const auto isBobWinner : doesBobWinQuery)
        {
            if (isBobWinner)
            {
                result = (result + powerOf2) % MOD;
            }
            powerOf2 = (powerOf2 * 2) % MOD;

        }
        cout << result << endl;
    }

}
