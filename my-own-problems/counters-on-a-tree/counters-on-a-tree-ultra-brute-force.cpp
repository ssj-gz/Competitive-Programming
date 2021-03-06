// Very, very brute force implementation - worse than O(2^N) :)
// Expected to pass Subtask #1 only.
#include <iostream>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

struct Node
{
    int index = -1;
    int numCounters = -1;
    vector<Node*> neighbours;
};

void buildParentLookup(Node* currentNode, Node* parentNode, vector<Node*>& parentLookup)
{
    parentLookup[currentNode->index] = parentNode;
    for (auto child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;
        buildParentLookup(child, currentNode, parentLookup);
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

    buildParentLookup(rootNode, nullptr, parentLookup);

    vector<vector<Node*>> allowedMovesForNodeLookup(numNodes);
    for (const auto& node : nodes)
    {
        vector<Node*> allowedMoves;
        Node* currentNode = parentLookup[node.index];
        while (currentNode)
        {
            allowedMoves.push_back(currentNode);
            currentNode = parentLookup[currentNode->index];
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

            nodes[node1Index].neighbours.push_back(&(nodes[node2Index]));
            nodes[node2Index].neighbours.push_back(&(nodes[node1Index]));
        }

        int nodeIndex = 0;
        for (auto& node : nodes)
        {
            node.index = nodeIndex;
            node.numCounters = readInt();

            nodeIndex++;
        }

        int64_t MOD = 1'000'000'007;
        int64_t result = 0;
        int64_t powerOf2 = 2;
        for (auto& rootNode : nodes)
        {
            const bool bobWins = !firstPlayerWins(nodes, &rootNode);
            if (bobWins)
            {
                result = (result + powerOf2) % MOD;
            }
            powerOf2 = (powerOf2 * 2) % MOD;
        }
        cout << result << endl;
    }

}
