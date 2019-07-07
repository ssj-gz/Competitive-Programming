#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

#include <cassert>

using namespace std;

struct Node
{
    int id = -1;
    vector<Node*> children;
    int color = -1;
};

void fixParentChild(Node* node, Node* parent)
{
    if (parent)
        node->children.erase(find(node->children.begin(), node->children.end(), parent));
    for (auto child : node->children)
        fixParentChild(child, node);
}

void findDistinctColors(const Node* node, set<int>& colors)
{
    cout << "findDistinctColors - node: " << node->id << endl;
    colors.insert(node->color);
    cout << " color: " << node->color << endl;
    for (auto child : node->children)
    {
        findDistinctColors(child, colors);
    }
}

vector<int> solveBruteForce(const vector<Node>& nodes, const vector<Node*>& queries)
{
    vector<int> queryResults;
    
    for (const auto root : queries)
    {
        set<int> colorsAtRootedSubtree;
        cout << "Query: start at " << root->id << endl;
        findDistinctColors(root, colorsAtRootedSubtree);
        queryResults.push_back(colorsAtRootedSubtree.size());
    }

    return queryResults;
}

int main()
{
    int numNodes;
    cin >> numNodes;

    int numQueries;
    cin >> numQueries;

    int rootNodeId;
    cin >> rootNodeId;
    rootNodeId--;

    vector<Node> nodes(numNodes);
    int nodeId = 1;
    for (auto& node : nodes)
    {
        node.id = nodeId;
        nodeId++;
    }

    for (int i = 0; i < numNodes - 1; i++)
    {
        int a;
        cin >> a;
        int b;
        cin >> b;

        a--;
        b--;

        nodes[a].children.push_back(&(nodes[b]));
        nodes[b].children.push_back(&(nodes[a]));
    }

    for (int i = 0; i < numNodes; i++)
    {
        cin >> nodes[i].color;
    }

    vector<Node*> queries(numQueries);
    for (int i = 0; i < numQueries; i++)
    {
        int nodeId;
        cin >> nodeId;
        nodeId--;


        queries[i] = &(nodes[nodeId]);
        cout << " query node id: " << nodeId << " blah: " << queries[i]->id << endl;
    }
    assert(cin);

    auto rootNode = &(nodes[rootNodeId]);
    cout << "rootNode: " << rootNode->id << endl;

    fixParentChild(rootNode, nullptr);

    const auto solutionBruteForce = solveBruteForce(nodes, queries);
    cout << "solutionBruteForce: " << endl;
    for (const auto x : solutionBruteForce)
    {
        cout << x << " ";
    }
    cout << endl;

}
