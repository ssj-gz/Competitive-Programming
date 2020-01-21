// O(N^2) solution to Equitree - should be able to pass Subtasks #1 and #2.
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <cassert>

using namespace std;

//#define DIAGNOSTICS

struct Node
{
    vector<Node*> neighbours;

    bool isSuitable = false;
    bool visitedInBFS = false;
    int id = -1;
};

int64_t numTriples(int64_t n)
{
    if (n < 3)
        return 0;
    return (n * (n - 1) * (n - 2));
}

void bfs(Node& rootNode, int64_t& result)
{

    const int numChildren = rootNode.neighbours.size();
#ifdef DIAGNOSTICS
    cout << "bfs from node: " << rootNode.id << endl;
    cout << " numChildren: " << numChildren << endl;
#endif


    vector<vector<Node*>> visitedViaChild;

    vector<Node*> visitedNodes = { &rootNode };
    for (auto rootNodeNeighbour : rootNode.neighbours)
    {
        visitedViaChild.push_back({rootNodeNeighbour});
        rootNodeNeighbour->visitedInBFS = true;
        visitedNodes.push_back(rootNodeNeighbour);
    }
    rootNode.visitedInBFS = true; 

    int iterationNum = 0;
    while (!visitedViaChild.empty())
    {
        if (visitedViaChild.size() < 3)
        {
#ifdef DIAGNOSTICS
            cout << "  breaking at iteration: " << iterationNum << " after visiting " << visitedNodes.size() << endl;
#endif
            break;
        }
#ifdef DIAGNOSTICS
        cout << " Iteration.   # visitedViaChild: " << visitedViaChild.size() << endl;
#endif
        int64_t numSuitable = 0;
        int64_t numSuitablePairsViaDifferentChild = 0;
        for (const auto& viaChild : visitedViaChild)
        {
            const int numSuitableViaThisChild = count_if(viaChild.begin(), viaChild.end(), [](const auto node) { return node->isSuitable; });
            const int64_t numNewTriples = numSuitablePairsViaDifferentChild * numSuitableViaThisChild * 6;
#ifdef DIAGNOSTICS
            cout << "  numSuitable: " << numSuitable << " numSuitablePairsViaDifferentChild: " << numSuitablePairsViaDifferentChild << " numNewTriples: " << numNewTriples << endl;
#endif
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

        const int original = visitedViaChild.size();
        visitedViaChild.erase(remove_if(visitedViaChild.begin(), visitedViaChild.end(), [](const auto viaChild) { return viaChild.empty(); }), visitedViaChild.end());
        const int newSize = visitedViaChild.size();

        if (newSize < original)
        {
#ifdef DIAGNOSTICS
            cout << "Pruned " << (original - newSize) << " at iteration: " << iterationNum << endl;
#endif
        }

        iterationNum++;
    }


    for (auto node : visitedNodes)
        node->visitedInBFS = false;
}

int64_t findNumTriplets(vector<Node>& nodes)
{
    int64_t result = 0;

    for (auto& node : nodes)
        bfs(node, result);

    return result;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    int numTestCases;
    cin >> numTestCases;

    for (int t = 0; t < numTestCases; t++)
    {
        int numNodes;
        cin >> numNodes;
        assert(1 <= numNodes && numNodes <= 200'000);

        vector<Node> nodes(numNodes);
        for (int i = 0; i < numNodes; i++)
            nodes[i].id = (i + 1);

        for (int i = 0; i < numNodes - 1; i++)
        {
            int u;
            cin >> u;
            int v;
            cin >> v;

            assert(1 <= u && u <= 200'000);
            assert(1 <= v && v <= 200'000);

            // Make 0-relative.
            u--;
            v--;

            nodes[u].neighbours.push_back(&(nodes[v]));
            nodes[v].neighbours.push_back(&(nodes[u]));
        }
        for (int i = 0; i < numNodes; i++)
        {
            int isSuitable;
            cin >> isSuitable;

            assert(isSuitable == 0 || isSuitable == 1);

            nodes[i].isSuitable = (isSuitable == 1);
        }

        const auto numTriplets = findNumTriplets(nodes);
        cout << numTriplets << endl;
    }
    assert(cin);
}


