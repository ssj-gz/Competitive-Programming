#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h>

using namespace std;

struct Node
{
    vector<Node*> neighbours;
    int id = -1; // 1-relative - handy for debugging.
    bool hasPerson = false;

    int index = -1; // 0-relative.

    bool visitedInBruteForceDFS = false;
};

void distDFS(const int rootNodeIndex, const Node* currentNode, const Node* parentNode, int height, vector<vector<int>>& distanceBetweenNodes)
{
    distanceBetweenNodes[rootNodeIndex][currentNode->index] = height;
    for (auto childNode : currentNode->neighbours)
    {
        if (childNode == parentNode)
            continue;

        distDFS(rootNodeIndex, childNode, currentNode, height + 1, distanceBetweenNodes);
    }
}

int64_t solveBruteForce(const vector<Node>& nodes)
{
    int64_t result = 0;

    const int numNodes = nodes.size();
    vector<vector<int>> distanceBetweenNodes(numNodes, vector<int>(numNodes, -1));
    for (const auto& rootNode : nodes)
    {
        distDFS(rootNode.index, &rootNode, nullptr, 0, distanceBetweenNodes);
    }
    for (int i = 0; i < numNodes; i++)
    {
        for (int j = 0; j < numNodes; j++)
        {
            assert(distanceBetweenNodes[i][j] != -1);
            assert(distanceBetweenNodes[i][j] == distanceBetweenNodes[j][i]);
            assert(distanceBetweenNodes[i][j] < numNodes);
        }
    }
    // TODO - all the rest of this XD
    for (const auto& node : nodes)
    {
        if (!node.hasPerson)
            continue;
        vector<vector<const Node*>> nodesAtDistance(numNodes);
        for (int i = 0; i < numNodes; i++)
        {
            nodesAtDistance[distanceBetweenNodes[node.index][i]].push_back(&(nodes[i]));
        }
        for (int distance = 1; distance < numNodes; distance++)
        {
            const auto& nd = nodesAtDistance[distance];

            for (const auto node1 : nd)
            {
                if (!node1->hasPerson)
                    continue;
                for (const auto node2 : nd)
                {
                    if (!node2->hasPerson)
                        continue;
                    assert(distanceBetweenNodes[node.index][node1->index] == distance);
                    assert(distanceBetweenNodes[node.index][node2->index] == distance);

                    if (distanceBetweenNodes[node1->index][node2->index] == distance)
                    {
                        result++;
                    }
                }
            }
        }
    }

    return result;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numNodes = 1 + rand() % 100;
        cout << numNodes << endl;

        for (int i = 0; i < numNodes - 1; i++)
        {
            cout << (i + 2) << " " << ((rand() % (i + 1) + 1)) << endl;
        }
        for (int i = 0; i < numNodes; i++)
        {
            cout << (rand() % 2) << endl;
        }
        return 0;
    }
    int numNodes;
    cin >> numNodes;

    vector<Node> nodes(numNodes);

    for (int i = 0; i < numNodes - 1; i++)
    {
        int u;
        cin >> u;
        int v;
        cin >> v;

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

        nodes[i].hasPerson = (hasPerson == 1);

        nodes[i].index = i;
        nodes[i].id = i + 1;
    }
    assert(cin);

    const auto solutionBruteForce = solveBruteForce(nodes);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
}
