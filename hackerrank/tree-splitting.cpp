#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Node
{
    vector<Node*> neighbours;
    vector<Node*> children;
    Node* parent = nullptr;
    int originalNumDescendants = 0;
};


int fixParentChildAndCountDescendants(Node* node, Node* parentNode)
{
    node->originalNumDescendants = 1;
    if (parentNode)
        node->children.erase(find(node->children.begin(), node->children.end(), parentNode));

    for (auto child : node->children)
        node->originalNumDescendants += fixParentChildAndCountDescendants(child, node);

    return node->originalNumDescendants;
}


vector<vector<Node*>> heavyChains;

// Build up heavyChains; move the heavy child of each node to the front of that node's children.
void doHeavyLightDecomposition(Node* node, bool followedHeavyEdge)
{
    if (followedHeavyEdge)
    {
        // Continue this chain.
        heavyChains.back().push_back(node);
    }
    else
    {
        // Start a new chain.
        heavyChains.push_back({node});
    }
    if (!node->children.empty())
    {
        auto heaviestChildIter = max_element(node->children.begin(), node->children.end(), [](const Node* lhs, const Node* rhs)
                {
                return lhs->originalNumDescendants < rhs->originalNumDescendants;
                });
        iter_swap(node->children.begin(), heaviestChildIter);
        auto heavyChild = node->children.front();
        doHeavyLightDecomposition(heavyChild, true);

        for (auto lightChildIter = node->children.begin() + 1; lightChildIter != node->children.end(); lightChildIter++)
            doHeavyLightDecomposition(*lightChildIter, false);
    }
}


int numInComponent(Node* nodeInComponent, Node* previousNode = nullptr)
{
    int num = 1; // This node.
    for (Node* neighbour : nodeInComponent->neighbours)
    {
        if ( neighbour == previousNode)
            continue;

        num += numInComponent(neighbour, nodeInComponent);
    }
    return num;
}

vector<int> bruteForce(vector<Node>& nodes, const vector<int>& queries)
{
    vector<int> queryResults;
    int previousAnswer = 0;
    for (int encryptedNodeIndex : queries)
    {
        const int nodeIndex = (encryptedNodeIndex ^ previousAnswer) - 1;

        Node* nodeToRemove = &(nodes[nodeIndex]);
        const int thisAnswer = numInComponent(nodeToRemove);
        queryResults.push_back(thisAnswer);

        const auto neighbours = nodeToRemove->neighbours;
        nodeToRemove->neighbours.clear();

        for (const auto neighbour : neighbours)
        {
            neighbour->neighbours.erase(remove(neighbour->neighbours.begin(), neighbour->neighbours.end(), nodeToRemove), neighbour->neighbours.end());
        }

        previousAnswer = thisAnswer;
    }
    return queryResults;
}

vector<int> findSolutionOptimised(vector<Node>& nodes, const vector<int>& queries)
{
    Node* rootNode = &(nodes.front());
    fixParentChildAndCountDescendants(rootNode, nullptr);

    doHeavyLightDecomposition(rootNode, false);
    vector<int> queryResults;
    int previousAnswer = 0;
    for (int encryptedNodeIndex : queries)
    {
        const int nodeIndex = (encryptedNodeIndex ^ previousAnswer) - 1;

        Node* nodeToRemove = &(nodes[nodeIndex]);
        const int thisAnswer = -1; // TODO
        queryResults.push_back(thisAnswer);

        previousAnswer = thisAnswer;
    }
    return queryResults;

}

int main()
{
    auto readInt = []() { int x; cin >> x; return x; };

    const int n = readInt();

    vector<Node> nodes(n);

    for (int edge = 0; edge < n - 1; edge++)
    {
        const int u = readInt() - 1;
        const int v = readInt() - 1;

        nodes[u].neighbours.push_back(&(nodes[v]));
        nodes[v].neighbours.push_back(&(nodes[u]));

        nodes[u].children.push_back(&(nodes[v]));
        nodes[v].children.push_back(&(nodes[u]));
    }

    const int numQueries = readInt();

    vector<int> queries;
    for (int queryNum = 0; queryNum < numQueries; queryNum++)
    {
        queries.push_back(readInt());
    }

    const auto bruteForceResults = bruteForce(nodes, queries);

    for (const auto result : bruteForceResults)
    {
        std::cout << result << std::endl;
    }

}
