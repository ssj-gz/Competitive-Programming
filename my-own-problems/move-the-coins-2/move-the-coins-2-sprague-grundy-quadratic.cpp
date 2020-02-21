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

void grundyNumberForTreeBruteForce(Node* node, const int depth, int& grundyNumber, int& numNodes)
{
    numNodes++;
    if (node->hasCoin)
    {
        grundyNumber ^= depth;
    }
    for (Node* child : node->children)
    {
        grundyNumberForTreeBruteForce(child, depth + 1, grundyNumber, numNodes);
    }
}

int grundyNumberForTreeBruteForce(Node* node, int& numNodes)
{
    int grundyNumber = 0;
    grundyNumberForTreeBruteForce(node, 0, grundyNumber, numNodes);
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

void fixParentChildAndHeights(Node* node, Node* parent = nullptr, int height = 0)
{
    node->parent = parent;

    node->children.erase(remove(node->children.begin(), node->children.end(), parent), node->children.end());

    for (auto child : node->children)
    {
        fixParentChildAndHeights(child, node, height + 1);
    }
}

vector<int> grundyNumbersForQueriesBruteForce(vector<Node>& nodes, const vector<Query>& queries)
{
    auto rootNode = &(nodes.front());
    vector<int> grundyNumbersForQueries;
    int queryNum = 0;
    int unused = 0;
    const auto originalTreeGrundyNumber = grundyNumberForTreeBruteForce(rootNode, unused);
    for (const auto& query : queries)
    {
        auto originalParent = query.nodeToMove->parent;
        reparentNode(query.nodeToMove, query.newParent);
        int numNodesInModifiedTree = 0;
        grundyNumbersForQueries.push_back(grundyNumberForTreeBruteForce(rootNode, numNodesInModifiedTree));
        assert(numNodesInModifiedTree == nodes.size());
        reparentNode(query.nodeToMove, originalParent);
        queryNum++;
        if (queryNum % 100 == 0)
            cerr << "Brute force queryNum: " << queryNum << " out of " << queries.size() << endl;
    }
    return grundyNumbersForQueries;
}

int main(int argc, char** argv)
{
    ios::sync_with_stdio(false);

    int numNodes;
    cin >> numNodes;

    vector<Node> nodes(numNodes);
    for (int i = 0; i < numNodes; i++)
    {
        int numCoins;
        cin >> numCoins;
        nodes[i].hasCoin = ((numCoins % 2) == 1); // Only care about the parity of the number of coins.
    }
    for (int i = 0; i < numNodes - 1; i++)
    {
        int a, b;
        cin >> a >> b;
        a--;
        b--;

        nodes[a].children.push_back(&nodes[b]);
        nodes[b].children.push_back(&nodes[a]);
    }

    int numQueries;
    cin >> numQueries;

    vector<Query> queries(numQueries);

    for (int i = 0; i < numQueries; i++)
    {
        int u, v;
        cin >> u >> v;
        u--;
        v--;

        queries[i].nodeToMove = &(nodes[u]);
        queries[i].newParent = &(nodes[v]);
    }

    auto rootNode = &(nodes.front());
    fixParentChildAndHeights(rootNode);

    const auto resultBruteForce = grundyNumbersForQueriesBruteForce(nodes, queries);
    vector<int> queriesWhereBobWins;
    int queryNum = 1;
    for (const auto queryResult : resultBruteForce)
    {
        if (queryResult == 0)
            queriesWhereBobWins.push_back(queryNum);
        queryNum++;
    }
    cout << queriesWhereBobWins.size() << endl;
    for (const auto queryNum : queriesWhereBobWins)
    {
        cout << queryNum << endl;
    }
}
