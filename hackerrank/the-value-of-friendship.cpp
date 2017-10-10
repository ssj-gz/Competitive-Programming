#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    int Q;
    cin >> Q;

    struct Edge;
    struct Node
    {
        vector<Edge*> neighbours;
        bool isInComponent = false;
    };
    struct Edge
    {
        Node* node1 = nullptr;
        Node* node2 = nullptr;
        Node* otherNode(Node* node)
        {
            return (node == node1 ? node2 : node1);
        }
        bool isInComponent = false;
    };

    for (int q = 0; q < Q; q++)
    {
        int n, m;
        cin >> n >> m;

        vector<Node> nodes(n);
        vector<Edge> edges;
        edges.reserve(m);

        for (int i = 0; i < m; i++)
        {
            int x, y;
            cin >> x >> y;
            x--;
            y--;

            auto node1 = &(nodes[x]);
            auto node2 = &(nodes[y]);
            Edge edge;
            edge.node1 = node1;
            edge.node2 = node2;
            edges.push_back(edge);

            auto newEdge = &(edges.back());

            node1->neighbours.push_back(newEdge);
            node2->neighbours.push_back(newEdge);
        }

        struct ComponentInfo
        {
            int numNodes = 0;
            int numEdges = 0;
        };
        vector<ComponentInfo> componentsInfo;


        for (auto& node : nodes)
        {
            if (node.isInComponent)
                continue;

            ComponentInfo componentInfo;
            vector<Node*> nodesToExplore = { &node };

            while (!nodesToExplore.empty())
            {
                vector<Node*> nextNodesToExplore;
                for (auto& nodeToExplore : nodesToExplore)
                {
                    if (nodeToExplore->isInComponent)
                        continue;

                    nodeToExplore->isInComponent = true;
                    componentInfo.numNodes++;

                    for (auto edge : nodeToExplore->neighbours)
                    {
                        if (!edge->isInComponent)
                        {
                            edge->isInComponent = true;
                            componentInfo.numEdges++;
                        }
                        nextNodesToExplore.push_back(edge->otherNode(nodeToExplore));
                    }

                }

                nodesToExplore = nextNodesToExplore;
            }

            componentsInfo.push_back(componentInfo);
        }

        sort(componentsInfo.begin(), componentsInfo.end(), [](const auto& lhs, const auto& rhs)
                {
                return rhs.numNodes < lhs.numNodes;
                });
        const vector<ComponentInfo> componentInfoDecreasingNodes(componentsInfo.begin(), componentsInfo.end());

        int64_t value = 0;
        int64_t valuesOfProcessedComponents = 0;
        int64_t numReinforcingNodes = 0;
        for (const auto& componentInfo : componentInfoDecreasingNodes)
        {
            //cout << "componentInfo.numNodes: " << componentInfo.numNodes << " componentInfo.numEdges: " << componentInfo.numEdges << endl;
            numReinforcingNodes += (componentInfo.numEdges - componentInfo.numNodes + 1);
            //cout << "numReinforcingNodes: " << numReinforcingNodes<< endl;
            const auto numNodesInComponent = componentInfo.numNodes;
            int64_t valueOfThisComponent = 0;
            const int numEdgesInComponentMST = componentInfo.numNodes - 1;
            for (int i = 1; i <= numEdgesInComponentMST; i++)
            {
                valueOfThisComponent = (i + 1) * i;
                //cout << "i: " << i << " valueOfThisComponent: " << valueOfThisComponent << endl;
                value += valuesOfProcessedComponents + (i + 1) * i;
                //cout << "i: " << i << "  value: " << value << endl;
            }
            //cout << "valueOfThisComponent: " << valueOfThisComponent << endl;
            valuesOfProcessedComponents += valueOfThisComponent;
        }
        //cout << "Before reinforcing: " << value << endl;
        value += numReinforcingNodes * valuesOfProcessedComponents;

        cout << value << endl;
    }

}
