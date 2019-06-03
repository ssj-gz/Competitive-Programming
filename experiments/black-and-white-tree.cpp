#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct Node
{
    vector<Node*> neighbours;
    int componentNum = -1;
    enum Color
    {
        Unknown,
        SameAsRoot,
        DifferentFromRoot
    };
    Color color = Unknown;
};

struct Component
{
    vector<Node*> nodes;
    Node* rootNode = nullptr;
    int numNodes = 0;
    int numNodesSameColorAsRoot = 0;
    
};

int main()
{
    int N, M;
    cin >> N >> M;

    vector<Node> nodes(N);
    for (int i = 0; i < N; i++)
    {
        int u, v;
        cin >> u >> v;
        u--;
        v--;

        nodes[u].neighbours.push_back(&nodes[v]);
        nodes[v].neighbours.push_back(&nodes[u]);
    }

    for (Node& node : nodes)
    {
        if (node.componentNum == -1)
        {
            Component newComponent;
            newComponent.rootNode = &node;

            vector<Node*> nodesToExplore = { &node };
            int depth = 0;
            while (!nodesToExplore.empty())
            {
                vector<Node*> nextNodesToExplore;
                Node::Color color = ((depth % 2) == 0) ? Node::SameAsRoot : Node::DifferentFromRoot;
                for (Node* nodeToExplore : nodesToExplore)
                {
                    if (nodeToExplore->color != Node::Unknown)
                    {
                        assert(nodeToExplore->color == color);
                        continue;
                    }
                    nodeToExplore->color = color;
                    if (color == Node::SameAsRoot)
                    {
                        newComponent.numNodesSameColorAsRoot++;
                    }
                    newComponent.numNodes++;
                    newComponent.nodes.push_back(nodeToExplore);

                    for (Node* neighbour : nodeToExplore->neighbours)
                    {
                        if (neighbour->color == Node::Unknown)
                        {
                            nextNodesToExplore.push_back(neighbour);
                        }
                    }
                }
                nodesToExplore = nextNodesToExplore;
            }
        }
    }

}
