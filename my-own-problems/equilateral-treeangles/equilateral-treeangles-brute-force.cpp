// O(N^3) solution to Equitree - should be able to pass Subtask #1.
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

    int numNodesProcessed = 0;
    for (const auto& node : nodes)
    {
        if (!node.isSuitable)
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
                        result++;
                    }
                }
            }
        }
        numNodesProcessed++;
    }

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
            int isSuitable;
            cin >> isSuitable;

            assert(isSuitable == 0 || isSuitable == 1);

            nodes[i].isSuitable = (isSuitable == 1);

            nodes[i].index = i;
        }
        assert(cin);



        const auto solutionBruteForce = solveBruteForce(nodes);
        cout << solutionBruteForce << endl; 
    }
}