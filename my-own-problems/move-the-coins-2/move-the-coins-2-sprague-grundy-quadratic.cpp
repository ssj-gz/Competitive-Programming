#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

struct Node
{
    vector<Node*> children;
    bool hasCoin = false;
    Node* parent = nullptr;
};

struct Query
{
    Node* nodeToMove = nullptr;
    Node* newParent = nullptr;
};

void grundyNumberForTreeBruteForce(Node* node, const int depth, int& grundyNumber)
{
    if (node->hasCoin)
    {
        grundyNumber ^= depth;
    }
    for (Node* child : node->children)
    {
        grundyNumberForTreeBruteForce(child, depth + 1, grundyNumber);
    }
}

int grundyNumberForTreeBruteForce(Node* node)
{
    int grundyNumber = 0;
    grundyNumberForTreeBruteForce(node, 0, grundyNumber);
    return grundyNumber;
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

void fixParentChild(Node* node, Node* parent = nullptr)
{
    node->parent = parent;

    node->children.erase(remove(node->children.begin(), node->children.end(), parent), node->children.end());

    for (auto child : node->children)
    {
        fixParentChild(child, node);
    }
}

vector<int> grundyNumbersForQueriesBruteForce(vector<Node>& nodes, const vector<Query>& queries)
{
    auto rootNode = &(nodes.front());
    vector<int> grundyNumbersForQueries;
    int queryNum = 0;
    const auto originalTreeGrundyNumber = grundyNumberForTreeBruteForce(rootNode);
    for (const auto& query : queries)
    {
        auto originalParent = query.nodeToMove->parent;
        reparentNode(query.nodeToMove, query.newParent);
        grundyNumbersForQueries.push_back(grundyNumberForTreeBruteForce(rootNode));
        reparentNode(query.nodeToMove, originalParent);
        queryNum++;
        if (queryNum % 100 == 0)
            cerr << "Brute force queryNum: " << queryNum << " out of " << queries.size() << endl;
    }
    return grundyNumbersForQueries;
}

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main(int argc, char** argv)
{
    ios::sync_with_stdio(false);

    const auto numNodes = read<int>();

    vector<Node> nodes(numNodes);
    for (auto& node : nodes)
    {
        const auto numCoins = read<int>();
        node.hasCoin = ((numCoins % 2) == 1); // Only care about the parity of the number of coins.
    }
    for (int i = 0; i < numNodes - 1; i++)
    {
        const auto a = read<int>() - 1;
        const auto b = read<int>() - 1;

        nodes[a].children.push_back(&nodes[b]);
        nodes[b].children.push_back(&nodes[a]);
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

    const auto queryGrundyNumbers = grundyNumbersForQueriesBruteForce(nodes, queries);
    vector<int> queriesWhereBobWins;
    int queryNum = 1;
    for (const auto queryGrundyNumber : queryGrundyNumbers)
    {
        if (queryGrundyNumber == 0)
            queriesWhereBobWins.push_back(queryNum);
        queryNum++;
    }
    cout << queriesWhereBobWins.size() << endl;
    for (const auto queryNum : queriesWhereBobWins)
    {
        cout << queryNum << endl;
    }
}
