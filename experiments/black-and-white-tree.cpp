#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct Node
{
    int id = 0;
    vector<Node*> neighbours;
    int componentNum = -1;
    enum Color
    {
        Unknown,
        SameAsRoot,
        DifferentFromRoot
    };
    Color color = Unknown;
    bool visitScheduled = false;
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
    for (int i = 0; i < M; i++)
    {
        int u, v;
        cin >> u >> v;
        u--;
        v--;

        nodes[u].neighbours.push_back(&nodes[v]);
        nodes[v].neighbours.push_back(&nodes[u]);
    }
    for (int nodeId = 1; nodeId <= N; nodeId++)
    {
        nodes[nodeId - 1].id = nodeId;
    }

    vector<Component> components;
    for (Node& node : nodes)
    {
        if (node.componentNum == -1)
        {
            Component newComponent;
            newComponent.rootNode = &node;
            const int componentNum = components.size();
            cout << "found new component beginning at : " << node.id << endl;


            vector<Node*> nodesToExplore = { &node };
            node.visitScheduled = true;
            int depth = 0;
            while (!nodesToExplore.empty())
            {
                cout << "Iteration - depth: " << depth << endl;
                Node::Color color = ((depth % 2) == 0) ? Node::SameAsRoot : Node::DifferentFromRoot;
                for (Node* nodeToExplore : nodesToExplore)
                {
                    cout << "nodeToExplore: " << nodeToExplore->id << " color: " << nodeToExplore->color << endl;
                    if (nodeToExplore->color != Node::Unknown)
                    {
                        assert(nodeToExplore->color == color);
                        continue;
                    }
                    nodeToExplore->color = color;
                    nodeToExplore->componentNum = componentNum;
                    cout << " set node: " << nodeToExplore->id << " to color: " << color << endl;
                    if (color == Node::SameAsRoot)
                    {
                        newComponent.numNodesSameColorAsRoot++;
                    }
                    newComponent.numNodes++;
                    newComponent.nodes.push_back(nodeToExplore);

                }
                vector<Node*> nextNodesToExplore;
                for (Node* nodeToExplore : nodesToExplore)
                {
                    for (Node* neighbour : nodeToExplore->neighbours)
                    {
                        cout << " adding neighbour: " << neighbour->id << " of " << nodeToExplore->id << endl;
                        if (!neighbour->visitScheduled)
                        {
                            nextNodesToExplore.push_back(neighbour);
                            neighbour->visitScheduled = true;
                        }
                    }
                }
                nodesToExplore = nextNodesToExplore;
                depth++;
            }

            components.push_back(newComponent);
        }
    }

    for (const auto& component : components)
    {
        cout << "component: " << endl;
        for (const auto& node : component.nodes)
        {
            assert(node->color != Node::Unknown);
            cout << node->id << " " << (node->color == Node::SameAsRoot ? "Same color as root" : "Different color from root") << endl;
        }
    }

}
