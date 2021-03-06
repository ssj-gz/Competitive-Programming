// O(N^3) solution to CHGORAM2 - should be able to pass Subtask #1.
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

struct Node
{
    vector<Node*> neighbours;
    bool isSuitable = false;
    int index = -1;
};

void addDistancesFromRootNode(const int rootNodeIndex, const Node* currentNode, const Node* parentNode, int dist, vector<vector<int>>& distanceBetweenNodes)
{
    distanceBetweenNodes[rootNodeIndex][currentNode->index] = dist;
    for (auto childNode : currentNode->neighbours)
    {
        if (childNode == parentNode)
            continue;

        addDistancesFromRootNode(rootNodeIndex, childNode, currentNode, dist + 1, distanceBetweenNodes);
    }
}

vector<vector<int>> computeNodeDistanceLookup(const vector<Node>& nodes)
{
    vector<vector<int>> distanceBetweenNodes(nodes.size(), vector<int>(nodes.size(), -1));
    for (const auto& rootNode : nodes)
    {
        addDistancesFromRootNode(rootNode.index, &rootNode, nullptr, 0, distanceBetweenNodes);
    }

    return distanceBetweenNodes;
}



int64_t solveBruteForce(const vector<Node>& nodes)
{
    int64_t numTriplets = 0;

    const int numNodes = nodes.size();

    const auto distanceBetweenNodes = computeNodeDistanceLookup(nodes);

    for (const auto& node : nodes)
    {
        if (!node.isSuitable)
            continue;

        vector<vector<const Node*>> nodesAtDistance(numNodes);
        for (auto i = 0; i < numNodes; i++)
        {
            nodesAtDistance[distanceBetweenNodes[node.index][i]].push_back(&(nodes[i]));
        }
        for (auto distance = 1; distance < numNodes; distance++)
        {
            const auto& nd = nodesAtDistance[distance];

            for (const auto node1 : nd)
            {
                if (!node1->isSuitable)
                    continue;
                for (const auto node2 : nd)
                {
                    if (!node2->isSuitable)
                        continue;
                    assert(distanceBetweenNodes[node.index][node1->index] == distance);
                    assert(distanceBetweenNodes[node.index][node2->index] == distance);

                    if (distanceBetweenNodes[node1->index][node2->index] == distance)
                    {
                        numTriplets++;
                    }
                }
            }
        }
    }

    return numTriplets;
}

int readInt()
{
    int toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    const auto numTestCases = readInt();

    for (auto t = 0; t < numTestCases; t++)
    {
        const auto numNodes = readInt();

        vector<Node> nodes(numNodes);

        for (auto i = 0; i < numNodes - 1; i++)
        {
            const auto u = readInt() - 1;
            const auto v = readInt() - 1;

            nodes[u].neighbours.push_back(&(nodes[v]));
            nodes[v].neighbours.push_back(&(nodes[u]));
        }
        for (auto i = 0; i < numNodes; i++)
        {
            const auto isSuitableValue = readInt();

            assert(isSuitableValue == 0 || isSuitableValue == 1);

            nodes[i].isSuitable = (isSuitableValue == 1);

            nodes[i].index = i;
        }
        assert(cin);

        const auto solutionBruteForce = solveBruteForce(nodes);
        cout << solutionBruteForce << endl; 
    }
}
