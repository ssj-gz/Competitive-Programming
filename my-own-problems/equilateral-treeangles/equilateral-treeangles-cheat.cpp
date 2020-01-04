// Simon St James (ssjgz) - simpler solution than the Editorial solution that I hope to break! - 2020-01-04
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <cassert>

using namespace std;

struct Node
{
    vector<Node*> neighbours;

    bool hasPerson = false;
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
    cout << "bfs from node: " << rootNode.id << endl;

    const int numChildren = rootNode.neighbours.size();
    //cout << " numChildren: " << numChildren << endl;


    vector<vector<Node*>> visitedViaChild;

    vector<Node*> visitedNodes = { &rootNode };
    for (auto rootNodeNeighbour : rootNode.neighbours)
    {
        visitedViaChild.push_back({rootNodeNeighbour});
        rootNodeNeighbour->visitedInBFS = true;
        visitedNodes.push_back(rootNodeNeighbour);
    }
    rootNode.visitedInBFS = true; 

    while (!visitedViaChild.empty())
    {
        //cout << " Iteration.   # visitedViaChild: " << visitedViaChild.size() << endl;
        int64_t numWithPerson = 0;
        int64_t numPairsWithPersonViaDifferentChild = 0;
        for (const auto& viaChild : visitedViaChild)
        {
            const int numWithPersonViaThisChild = count_if(viaChild.begin(), viaChild.end(), [](const auto node) { return node->hasPerson; });
            const int64_t numNewTriples = numPairsWithPersonViaDifferentChild * numWithPersonViaThisChild * 6;
            //cout << "  numWithPerson: " << numWithPerson << " numPairsWithPersonViaDifferentChild: " << numPairsWithPersonViaDifferentChild << " numNewTriples: " << numNewTriples << endl;
            result += numNewTriples;
            numPairsWithPersonViaDifferentChild += numWithPerson * numWithPersonViaThisChild;
            numWithPerson += numWithPersonViaThisChild;
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

        visitedViaChild.erase(remove_if(visitedViaChild.begin(), visitedViaChild.end(), [](const auto viaChild) { return viaChild.empty(); }), visitedViaChild.end());

        if (visitedViaChild.size() < 3)
            break;
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

    int numNodes;
    cin >> numNodes;
    assert(1 <= numNodes && numNodes <= 100'000);

    vector<Node> nodes(numNodes);
    for (int i = 0; i < numNodes; i++)
        nodes[i].id = (i + 1);

    for (int i = 0; i < numNodes - 1; i++)
    {
        int u;
        cin >> u;
        int v;
        cin >> v;

        assert(1 <= u && u <= 100'000);
        assert(1 <= v && v <= 100'000);

        // Make 0-relative.
        u--;
        v--;

        nodes[u].neighbours.push_back(&(nodes[v]));
        nodes[v].neighbours.push_back(&(nodes[u]));
    }
    for (int i = 0; i < numNodes; i++)
    {
        int hasPerson;
        cin >> hasPerson;

        assert(hasPerson == 0 || hasPerson == 1);

        nodes[i].hasPerson = (hasPerson == 1);
    }
    assert(cin);

    const auto numTriplets = findNumTriplets(nodes);
    cout << numTriplets << endl;
}


