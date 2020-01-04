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
    //cout << "bfs from node: " << rootNode.id << endl;

    vector<Node*> toExplore = { &rootNode };
    vector<Node*> visitedNodes = { &rootNode };
    rootNode.visitedInBFS = true; 

    while (!toExplore.empty())
    {
        //cout << " # toExplore: " << toExplore.size() << endl;
        vector<Node*> nextToExplore;

        int64_t numWithPerson = 0;
        for (auto node : toExplore)
        {
            if (node->hasPerson)
                numWithPerson++;
        }
        //cout << " numWithPerson: " << numWithPerson << endl;

        result += numTriples(numWithPerson);

        for (auto node : toExplore)
        {
            for (auto neighbour : node->neighbours)
            {
                if (!neighbour->visitedInBFS)
                {
                    neighbour->visitedInBFS = true;
                    nextToExplore.push_back(neighbour);
                    visitedNodes.push_back(neighbour);
                }
            }
        }

        toExplore = nextToExplore;
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


