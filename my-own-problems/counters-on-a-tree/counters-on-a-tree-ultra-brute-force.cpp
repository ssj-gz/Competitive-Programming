#include <iostream>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

struct Node
{
    int index = -1;
    int numCoins = -1;
    vector<Node*> neighbours;

    bool bobWinsIfR = false;
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

enum Result { Unknown, CurrentPlayerWins, CurrentPlayerLoses };
bool currentPlayerWins(const vector<int>& state, Node* rootNode, map<vector<int>, Result>& resultLookup, const vector<Node*>& parentLookup)
{
    if (resultLookup[state] != Unknown)
    {
        return resultLookup[state] == CurrentPlayerWins;
    }

    bool currentPlayerHasWinningMove = false;
    const int numNodes = state.size();
    for (int nodeIndex = 0; nodeIndex < numNodes; nodeIndex++)
    {
        if (nodeIndex == rootNode->index)
            continue;

        if (state[nodeIndex] > 0)
        {
            vector<int> nextState = state;
            Node* parentNode = parentLookup[nodeIndex];
            assert(parentNode);
            nextState[nodeIndex]--;
            nextState[parentNode->index]++;

            const auto isWinForPlayer = (!currentPlayerWins(nextState, rootNode, resultLookup, parentLookup));
            if (isWinForPlayer)
                currentPlayerHasWinningMove = true;
        }
    }

    resultLookup[state] = (currentPlayerHasWinningMove ? CurrentPlayerWins : CurrentPlayerLoses);

    return currentPlayerHasWinningMove;
};

bool firstPlayerWins(const vector<Node>& nodes, Node* rootNode)
{
    cout << "firstPlayerWins? rootNode " << rootNode->index << endl;
    const int numNodes = nodes.size();
    vector<Node*> parentLookup(numNodes, nullptr);

    buildParentLookup(rootNode, nullptr, parentLookup);

    vector<int> initialState;
    for (const auto& node : nodes)
    {
        initialState.push_back(node.numCoins);
    }

    map<vector<int>, Result> resultLookup;

    return currentPlayerWins(initialState, rootNode, resultLookup, parentLookup);
}

int main()
{
    auto readInt = [](){ int x; cin >> x; return x; };

    const auto numTestcases = readInt();
    for (int t = 0; t < numTestcases; t++)
    {
        const auto numNodes = readInt();
        vector<Node> nodes(numNodes);

        int nodeIndex = 0;
        for (auto& node : nodes)
        {
            node.index = nodeIndex;
            node.numCoins = readInt();
        }

        for (auto edgeNum = 0; edgeNum < numNodes - 1; edgeNum++)
        {
            const auto node1Index = readInt() - 1;
            const auto node2Index = readInt() - 1;

            nodes[node1Index].neighbours.push_back(&(nodes[node2Index]));
            nodes[node2Index].neighbours.push_back(&(nodes[node1Index]));
        }

        int64_t MOD = 1'000'000'007;
        int64_t result = 0;
        int64_t powerOf2 = 1;
        for (auto& rootNode : nodes)
        {
            if (!firstPlayerWins(nodes, &rootNode))
            {
                result = (result + powerOf2) % MOD;
            }
            powerOf2 = (powerOf2 * 2) % MOD;
        }
        cout << result << endl;
    }

}
