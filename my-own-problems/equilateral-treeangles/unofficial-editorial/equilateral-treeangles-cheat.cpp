// O(N^2) solution to CHGORAM2 - should be able to pass Subtasks #1 and #2.
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

struct Node
{
    vector<Node*> neighbours;

    bool isSuitable = false;
    bool visitedInBFS = false;
    int id = -1;
};

int64_t numTriples(int64_t n)
{
    // i.e. "n choose 3".
    if (n < 3)
        return 0;
    return (n * (n - 1) * (n - 2));
}

void bfs(Node& centreNode, int64_t& result)
{
    // Will store, for each child of centreNode, the list of nodes
    // reachable from centreNode via that child.
    vector<vector<Node*>> visitedViaChild;

    vector<Node*> visitedNodes = { &centreNode };
    for (auto rootNodeNeighbour : centreNode.neighbours)
    {
        visitedViaChild.push_back({rootNodeNeighbour});
        rootNodeNeighbour->visitedInBFS = true;
        visitedNodes.push_back(rootNodeNeighbour);
    }
    centreNode.visitedInBFS = true; 

    while (!visitedViaChild.empty())
    {
        if (visitedViaChild.size() < 3)
        {
            // Stop here - for a triple to be centred around centreNode, there need to be
            // three nodes, equidistant from centreNode, and reached via centreNode via
            // different children.  This is no longer possible.
            break;
        }
        int64_t numSuitable = 0;
        int64_t numSuitablePairsViaDifferentChild = 0;
        for (const auto& viaChild : visitedViaChild)
        {
            const int numSuitableViaThisChild = count_if(viaChild.begin(), viaChild.end(), [](const auto node) { return node->isSuitable; });
            const int64_t numNewTriples = numSuitablePairsViaDifferentChild * numSuitableViaThisChild * 6;
            result += numNewTriples;
            numSuitablePairsViaDifferentChild += numSuitable * numSuitableViaThisChild;
            numSuitable += numSuitableViaThisChild;
        }
        for (auto& viaChild : visitedViaChild)
        {
            vector<Node*> nextVisitedViaChild;
            for (auto node : viaChild)
            {
                for (auto neighbour : node->neighbours)
                {
                    if (!neighbour->visitedInBFS)
                    {
                        neighbour->visitedInBFS = true;
                        nextVisitedViaChild.push_back(neighbour);
                        visitedNodes.push_back(neighbour);
                    }
                }
            }
            viaChild = nextVisitedViaChild;
        }

        // Cull children of centreNode such that there are no longer any nodes at this distance from centreNode
        // reachable from centreNode via that child.
        visitedViaChild.erase(remove_if(visitedViaChild.begin(), visitedViaChild.end(), [](const auto viaChild) { return viaChild.empty(); }), visitedViaChild.end());
    }

    for (auto node : visitedNodes)
        node->visitedInBFS = false;
}

int64_t findNumTriplets(vector<Node>& nodes)
{
    int64_t numTriplets = 0;

    for (auto& node : nodes)
        bfs(node, numTriplets);

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
        for (auto i = 0; i < numNodes; i++)
            nodes[i].id = (i + 1);

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
        }

        const auto numTriplets = findNumTriplets(nodes);
        cout << numTriplets << endl;
    }
    assert(cin);
}


