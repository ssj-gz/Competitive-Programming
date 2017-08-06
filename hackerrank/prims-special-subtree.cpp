#include <cmath>
#include <cstdio>
#include <vector>
#include <set>
#include <iostream>
#include <algorithm>
#include <cassert>
using namespace std;

struct Node;
struct Edge
{
    Edge() = default;
    Edge(Node* node1, Node *node2, int distance)
        : node1(node1), node2(node2), distance(distance)
    {

    }
    Node *node1 = nullptr;
    Node *node2 = nullptr;
    int distance = -1;
};
struct Node
{
    vector<Edge*> edges;
    bool isInTree = false;
};

int main() {
    int N, M;
    cin >> N >> M;
    vector<Node> allNodes(N);
    vector<Edge> allEdges(M);
    for (int i = 0; i < M; i++)
    {
        int x, y, r;
        cin >> x >> y >> r;

        Node *node1 = &(allNodes[x - 1]);
        Node *node2 = &(allNodes[y - 1]);

        allEdges[i] = Edge(node1, node2, r);

        node1->edges.push_back(&(allEdges[i]));
        node2->edges.push_back(&(allEdges[i]));
    }

    auto edgeCompare = [](const Edge* lhs, const Edge* rhs)
    {
        if (lhs->distance != rhs->distance)
            return lhs->distance < rhs->distance;

        // Allow us to store multiple edges with the same
        // distance.
        return lhs < rhs;
    };
    set<Edge*, decltype(edgeCompare)> unprocessedEdges(edgeCompare);

    auto addNodeEdges = [&unprocessedEdges](const Node* node)
    {
        for (const auto& edge : node->edges)
        {
            unprocessedEdges.insert(edge);
        }

    };


    Node* initialNode = &(allNodes[0]);
    initialNode->isInTree = true;
    addNodeEdges(initialNode);

    long treeWeight = 0;

    while (!unprocessedEdges.empty())
    {
        const Edge* edge = *unprocessedEdges.begin();
        unprocessedEdges.erase(unprocessedEdges.begin());

        assert(edge->node1->isInTree || edge->node2->isInTree);
        if (edge->node1->isInTree == edge->node2->isInTree)
            continue;

        Node *newlyAddedNode = (edge->node1->isInTree ? edge->node2 : edge->node1);
        assert(!newlyAddedNode->isInTree);

        newlyAddedNode->isInTree = true;
        treeWeight += edge->distance;

        addNodeEdges(newlyAddedNode);
    }

    cout << treeWeight << endl;


    return 0;
}

