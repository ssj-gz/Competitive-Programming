#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Node
{
    vector<Node*> neighbours;
    vector<Node*> children;
    Node* parent = nullptr;
};


void fixParentChildAndHeights(Node* node, Node* parent = nullptr)
{
    node->parent = parent;

    node->children.erase(remove(node->children.begin(), node->children.end(), parent), node->children.end());

    for (auto child : node->children)
    {
        fixParentChildAndHeights(child, node);
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
