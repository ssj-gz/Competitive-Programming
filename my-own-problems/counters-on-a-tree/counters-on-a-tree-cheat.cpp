// "Cheat" solution using Centroid Decomposition and then a "naive"
// way of propagating grundy numbers within each centroid-subtree.
// Is at least O(N^2), but runs incredibly quickly :(
#pragma GCC optimize("Ofast")
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr auto maxDist = 300'000;

struct Node
{
    bool hasCounter = false;
    vector<Node*> neighbours;
    int id = -1;

    int grundyNumberIfRoot = 0;
};


int countDescendants(Node* node, Node* parentNode)
{
    int numDescendants = 1; // Current node.

    for (const auto& child : node->neighbours)
    {
        if (child == parentNode)
            continue;

        numDescendants += countDescendants(child, node);
    }

    return numDescendants;
}

int findCentroidAux(Node* currentNode, Node* parentNode, const int totalNodes, Node** centroid)
{
    int numDescendents = 1;

    bool childHasTooManyDescendants = false;

    for (const auto& child : currentNode->neighbours)
    {
        if (child == parentNode)
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

void findNumCoinsAtDist(Node* currentNode, Node* parentNode, int distance, int* numCoinsAtDist)
{
    if (currentNode->hasCounter)
    {
        numCoinsAtDist[distance] = 1 - numCoinsAtDist[distance];
    }

    for (const auto& child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        findNumCoinsAtDist(child, currentNode, distance + 1, numCoinsAtDist);
    }

}

void findMaxDistance(Node* currentNode, Node* parentNode, int distance, int& maxDist)
{
    if (currentNode->hasCounter)
        maxDist = max(maxDist, distance);

    for (const auto& child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        findMaxDistance(child, currentNode, distance + 1, maxDist);
    }

}

int grundyNumberForNode[maxDist];
int distOfNode[maxDist];

void findGrundyNumberForNode(Node* currentNode, Node* parentNode, int distance, int& descendentIndex)
{
    grundyNumberForNode[descendentIndex] = currentNode->grundyNumberIfRoot;
    distOfNode[descendentIndex] = distance;
    descendentIndex++;

    for (const auto& child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        findGrundyNumberForNode(child, currentNode, distance + 1, descendentIndex);
    }
}

void setGrundyNumberForNode(Node* currentNode, Node* parentNode, int& descendentIndex)
{
    currentNode->grundyNumberIfRoot = grundyNumberForNode[descendentIndex];
    descendentIndex++;

    for (const auto& child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        setGrundyNumberForNode(child, currentNode, descendentIndex);
    }
}

void dealWithCentroid(Node* currentNode, Node* parentNode, int distance, const bool centroidHasCoin, int& centroidGrundy)
{
    if (currentNode->hasCounter)
        centroidGrundy ^= distance;
    if (centroidHasCoin)
        currentNode->grundyNumberIfRoot ^= distance;
    for (const auto& child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        dealWithCentroid(child, currentNode, distance + 1, centroidHasCoin, centroidGrundy);
    }
}

int64_t approxNumComputations = 0;
void doCentroidDecomposition(Node* startNode)
{
    Node* centroid = findCentroid(startNode);

    const auto numNodesInComponent = countDescendants(startNode, nullptr);

    for (int pass = 0; pass < 2; pass++)
    {
        int maxMaxDistance = 0;
        int numCoinsAtDist[numNodesInComponent] = {};
        for (auto& neighbour : centroid->neighbours)
        {
            int descendentIndex = 0;
            findGrundyNumberForNode(neighbour, centroid, 1, descendentIndex);
            const auto numDescendents = descendentIndex;

            int distancesWithCoins[maxMaxDistance] = {};
            int numDistanceWithCoin = 0;
            for (int distance = 0; distance <= maxMaxDistance; distance++)
            {
                assert(numCoinsAtDist[distance] == 0 || numCoinsAtDist[distance] == 1);
                if (numCoinsAtDist[distance] == 1)
                {
                    distancesWithCoins[numDistanceWithCoin] = distance;
                    numDistanceWithCoin++;
                }
            }
            for (int i = 0; i < numDistanceWithCoin; i++)
            {
                const int distance = distancesWithCoins[i];
                for (int j = 0; j < numDescendents; j++)
                {
                    grundyNumberForNode[j] ^= (distance + distOfNode[j]);
                }
            }

            approxNumComputations += static_cast<int64_t>(numDistanceWithCoin) * numDescendents;

            descendentIndex = 0;
            setGrundyNumberForNode(neighbour, centroid, descendentIndex);

            int maxDistance = 0;
            findMaxDistance(neighbour, centroid, 1, maxDistance);
            maxMaxDistance = max(maxMaxDistance, maxDistance);
            findNumCoinsAtDist(neighbour, centroid, 1, numCoinsAtDist);
        }
        if (pass == 0)
        {
            dealWithCentroid(centroid, nullptr, 0, centroid->hasCounter, centroid->grundyNumberIfRoot);
        }
        std::reverse(centroid->neighbours.begin(), centroid->neighbours.end());
    }

    for (auto& neighbour : centroid->neighbours)
    {
        assert(std::find(neighbour->neighbours.begin(), neighbour->neighbours.end(), centroid) != neighbour->neighbours.end());
        // Erase the edge from the centroid's neighbour to the centroid, essentially "chopping off" each child into its own
        // component.
        neighbour->neighbours.erase(std::find(neighbour->neighbours.begin(), neighbour->neighbours.end(), centroid));
        doCentroidDecomposition(neighbour);
    }
}

int main()
{
    ios::sync_with_stdio(false);
    auto readInt = [](){ int x; cin >> x; return x; };

    const auto numTestcases = readInt();
    for (int t = 0; t < numTestcases; t++)
    {
        const auto numNodes = readInt();
        vector<Node> nodes(numNodes);

        for (auto edgeNum = 0; edgeNum < numNodes - 1; edgeNum++)
        {
            const auto node1Index = readInt() - 1;
            const auto node2Index = readInt() - 1;

            nodes[node1Index].neighbours.push_back(&(nodes[node2Index]));
            nodes[node2Index].neighbours.push_back(&(nodes[node1Index]));
        }

        int numNodesWithCounter = 0;
        int nodeId = 1;
        for (auto& node : nodes)
        {
            const auto numCounters = readInt();
            node.id = nodeId;
            nodeId++;

            node.hasCounter = ((numCounters % 2) == 1);
            if (node.hasCounter)
                numNodesWithCounter++;
        }

        auto rootNode = &(nodes.front());
        doCentroidDecomposition(rootNode);

        int64_t MOD = 1'000'000'007;
        int64_t result = 0;
        int64_t powerOf2 = 2; // 2 to the power of 1.
        for (auto& node : nodes)
        {
            if (node.grundyNumberIfRoot == 0)
            {
                result = (result + powerOf2) % MOD;
            }
            powerOf2 = (powerOf2 * 2) % MOD;
        }
        cout << result << endl;
    }
}
