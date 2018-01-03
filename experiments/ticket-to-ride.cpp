#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct Node
{
    vector<Node*> neighbours;
    int index = -1;
    bool doNotExplore = false;
};

vector<Node*> getDescendants(Node* node, Node* parentNode)
{
    vector<Node*> descendants;
    if (node->doNotExplore)
        return descendants;

    descendants.push_back(node);

    for (const auto& child : node->neighbours)
    {
        if (child == parentNode)
            continue;
        if (child->doNotExplore)
            continue;

        //descendants.push_back(child);
        const auto& childDescendants = getDescendants(child, node);
        descendants.insert(descendants.end(), childDescendants.begin(), childDescendants.end());
    }

#if 0
    cout << "Node: " << node->index << " descendants: ";
    for (const auto& node : descendants)
    {
        cout << node->index << " ";
    }
    cout << endl;
#endif

    return descendants;
}

Node* findMedian(Node* startNode)
{
    int numInGraph = 1;
    vector<Node*> allNodes = getDescendants(startNode, nullptr);
    const auto numNodes = allNodes.size();

    if (numNodes == 1)
        return startNode;

    for (auto& node : allNodes)
    {
        bool isMedian = true;
        for (auto& neighbour : node->neighbours)
        {
            if (getDescendants(neighbour, node).size() > numNodes / 2)
            {
                isMedian = false;
                break;
            }
        }
        if (isMedian)
            return node;

    }

    assert(false);
    return nullptr;
}

void decompose(Node* startNode, vector<vector<int>>& blee, int indentLevel = 0)
{
    if (startNode->doNotExplore)
        return;
    auto countPair = [&blee](Node* node1, Node* node2)
    {
        assert(node1 != node2);
        if (node1->index > node2->index)
            swap(node1, node2);
        cout << "Counted " << node1->index << "," << node2->index << endl;
        blee[node1->index][node2->index]++;
        blee[node2->index][node1->index]++;
    };
    const string indent(indentLevel, ' ');
    cout << indent << "Decomposing graph containing " << startNode->index << endl;
    Node* centroid = findMedian(startNode);
    const auto numNodes = getDescendants(centroid, nullptr).size();
    cout << indent << " centroid: " << centroid->index << " num nodes: " << numNodes << endl;
    cout << " indentLevel: " << indentLevel << " numNodes: " << numNodes << endl;

    if (centroid->doNotExplore)
        return;
    centroid->doNotExplore = true;

    vector<Node*> descendantsSoFar;
    descendantsSoFar.push_back(centroid);
    for (auto& neighbour : centroid->neighbours)
    {
        auto newDescendants = getDescendants(neighbour, centroid);
        assert(newDescendants.size() <= numNodes / 2);
#if 1
        for (const auto& descendant : newDescendants)
        {
            for (const auto& oldDescendant : descendantsSoFar)
            {
                countPair(descendant, oldDescendant);
            }
        }
        descendantsSoFar.insert(descendantsSoFar.end(), newDescendants.begin(), newDescendants.end());
#endif
        decompose(neighbour, blee, indentLevel + 1);
    }

}

int main()
{
    srand(time(0));
    const int maxNodes = 10;
    const int numNodes = (rand() % maxNodes) + 1;
    vector<Node> nodes(numNodes);

    int nodeIndex = 0;
    for (auto& node : nodes)
    {
        node.index = nodeIndex;
        if (nodeIndex != 0)
        {
            const int neighbourNodeIndex = rand() % nodeIndex;
            auto neighbourNode = &(nodes[neighbourNodeIndex]);
            auto newNode = &(nodes[nodeIndex]);

            newNode->neighbours.push_back(neighbourNode);
            neighbourNode->neighbours.push_back(newNode);

        }

        nodeIndex++;

    }

    cout << "Tree: " << endl;
    for (const auto& node : nodes)
    {
        cout << "Node id: " << node.index << " neighbour ids: ";
        for (const auto& neighbour : node.neighbours)
        {
            cout << neighbour->index << " ";
        }
        cout << endl;
    }

    const auto median = findMedian(&(nodes.front()));
    cout << "median: " << median->index << endl;

    vector<vector<int>> blee(numNodes, vector<int>(numNodes, 0));

    decompose(&(nodes.front()), blee);

    for (int i = 0; i < numNodes; i++)
    {
        for (int j = 0; j < numNodes; j++)
        {
            if (i == j)
                continue;
            cout << "i: " << i << " j: " << j << " blee: " << blee[i][j] << endl;
        }
    }
    
}



