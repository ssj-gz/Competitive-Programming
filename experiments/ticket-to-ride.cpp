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

int countDescendants(Node* node, Node* parentNode)
{
    if (node->doNotExplore)
        return 0;
    int numDescendants = 1; // Current node.

    for (const auto& child : node->neighbours)
    {
        if (child == parentNode)
            continue;
        if (child->doNotExplore)
            continue;

        numDescendants += countDescendants(child, node);
    }

    return numDescendants;
}

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

        const auto& childDescendants = getDescendants(child, node);
        descendants.insert(descendants.end(), childDescendants.begin(), childDescendants.end());
    }

    return descendants;
}

int findCentroidAux(Node* currentNode, Node* parentNode, const int totalNodes, Node** centroid)
{
    if (currentNode->doNotExplore)
        return 0;
    int numDescendents = 1;

    bool childHasTooManyDescendants = false;

    for (const auto& child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;
        if (child->doNotExplore)
            continue;

        const auto numChildDescendants = findCentroidAux(child, currentNode, totalNodes, centroid);
        if (numChildDescendants > totalNodes / 2)
        {
            // Not the centroid, but can't break here - must continue processing children.
            childHasTooManyDescendants = true;
        }

        numDescendents += numChildDescendants;
    }

    if (!childHasTooManyDescendants)
    {
        // No child has more than totalNodes/2 descendants, but what about the remainder of the graph?
        const auto nonChildDescendants = totalNodes - numDescendents;
        if (nonChildDescendants <= totalNodes / 2)
        {
            assert(centroid);
            *centroid = currentNode;
        }
    }

    return numDescendents;
}

Node* findCentroid(Node* startNode)
{
    const auto totalNumNodes = countDescendants(startNode, nullptr);
    Node* centroid = nullptr;
    findCentroidAux(startNode, nullptr, totalNumNodes, &centroid);
    assert(centroid);
    return centroid;
}


#if 0
Node* findCentroidBruteForce(Node* startNode, int numNodes)
{
    int numInGraph = 1;
    //vector<Node*> allNodes = getDescendants(startNode, nullptr);
    //const auto numNodes = countDescendants(startNode, nullptr);

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
#endif

void decompose(Node* startNode, vector<vector<int>>& blee, int indentLevel = 0)
{
    if (startNode->doNotExplore)
        return;
    auto countPair = [&blee](Node* node1, Node* node2)
    {
        assert(node1 != node2);
        if (node1->index > node2->index)
            swap(node1, node2);
        blee[node1->index][node2->index]++;
        blee[node2->index][node1->index]++;
    };
    const string indent(indentLevel, ' ');
    //cout << indent << "Decomposing graph containing " << startNode->index << endl;
    const auto numNodes = countDescendants(startNode, nullptr);
    Node* centroid = findCentroid(startNode);
    //cout << indent << " centroid: " << centroid->index << " num nodes: " << numNodes << endl;
    //cout << " indentLevel: " << indentLevel << " numNodes: " << numNodes << endl;

    static int numNodesTotal = 0;
    numNodesTotal += numNodes;
    cout << "numNodesTotal: " << numNodesTotal << endl;

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
    const int maxNodes = 10000;
    const int numNodes = (rand() % maxNodes) + 1;
    //const int numNodes = 20000;
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

    const auto median = findCentroid(&(nodes.front()));
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
            assert(blee[i][j] == 1);
        }
    }
    
}



