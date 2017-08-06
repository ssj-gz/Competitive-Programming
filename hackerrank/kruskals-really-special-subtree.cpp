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
    Edge(Node* node1, Node *node2, int weight)
        : node1(node1), node2(node2), weight(weight)
    {

    }
    Node *node1 = nullptr;
    Node *node2 = nullptr;
    int weight = -1;
};
struct Node
{
    vector<Edge*> edges;
    int componentNum = -1;
};

int main() {
    int N, M;
    cin >> N >> M;
    vector<Node> allNodes(N, Node{});
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
        if (lhs->weight != rhs->weight)
            return lhs->weight < rhs->weight;

        // Allow us to store multiple edges with the same
        // weight.
        return lhs < rhs;
    };
    // A "proper" implementation of Kruskal's Algorithm (to make up for my "cheat-y" version
    // where I just copy-n-pasted my Prim's Algorithm implementation XD) using a very simple 
    // (but reasonably efficient) "disjoint-set" implementation.
    set<Edge*, decltype(edgeCompare)> unprocessedEdges(edgeCompare);

    vector<vector<Node*>> nodesInComponent;

    for (auto& edge : allEdges)
    {
        unprocessedEdges.insert(&edge);
    }

    long treeWeight = 0;
    while (!unprocessedEdges.empty())
    {
        const Edge* edge = *unprocessedEdges.begin();
        unprocessedEdges.erase(unprocessedEdges.begin());

        Node *node1 = edge->node1;
        Node *node2 = edge->node2;

        if (node1->componentNum == -1 && node2->componentNum == -1)
        {
            // Join nodes, and create brand-new component.
            nodesInComponent.push_back({node1, node2});
            node1->componentNum = nodesInComponent.size() - 1;
            node2->componentNum = nodesInComponent.size() - 1;
            treeWeight += edge->weight;
            continue;
        }
        if (node1->componentNum == node2->componentNum)
        {
            // Don't create a cycle.
            continue;
        }
        // Have two nodes, at least one of which is in a component, and they are
        // not in the same component.
        int absorbingComponent = -1;
        int absorbedComponent = -1;
        Node *absorbedNode = nullptr;
        // If either node has no component, then it is absorbed into the other nodes' component; otherwise, the larger
        // component absorbs the smaller.
        if (node1->componentNum == -1 || (node2->componentNum != -1 && nodesInComponent[node1->componentNum].size() < nodesInComponent[node2->componentNum].size() ))
        {
            absorbingComponent = node2->componentNum;
            absorbedComponent = node1->componentNum;
            absorbedNode = node1;
        }
        else
        {
            absorbingComponent = node1->componentNum;
            absorbedComponent = node2->componentNum;
            absorbedNode = node2;
        }
        assert(absorbingComponent != -1);
        if (absorbedComponent != -1)
        {
            for (auto nodeToAbsorb : nodesInComponent[absorbedComponent])
            {
                nodeToAbsorb->componentNum = absorbingComponent;
                nodesInComponent[absorbingComponent].push_back(nodeToAbsorb);
            }
        }
        else
        {
            absorbedNode->componentNum = absorbingComponent;
            nodesInComponent[absorbingComponent].push_back(absorbedNode);
        }
        treeWeight += edge->weight;
    }

    cout << treeWeight << endl;

    return 0;
}

