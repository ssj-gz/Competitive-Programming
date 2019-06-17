// Simon St James (ssjgz) - 2019-06-16
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct Node
{
    int colour = -1;
    bool visited = false;
    vector<Node*> neigbours;

    bool visitedBruteForce = false;
};

int solveBruteForce(vector<Node>& nodes, int colourToSolveFor)
{
    int minDistance = numeric_limits<int>::max();
    for (auto& rootNode : nodes)
    {
        if (rootNode.colour != colourToSolveFor)
        {
            continue;
        }
        for (auto& node : nodes)
        {
            node.visitedBruteForce = false;
        }

        vector<Node*> toExplore = { &rootNode };
        rootNode.visitedBruteForce = true;

        int numIterations = 0;
        while (!toExplore.empty())
        {
            vector<Node*> nextToExplore;
            for (auto node : toExplore)
            {
                if (numIterations != 0 && node->colour == colourToSolveFor)
                {
                    minDistance = min(minDistance, numIterations);
                }

                for (auto neighbour : node->neigbours)
                {
                    if (!neighbour->visitedBruteForce)
                    {
                        nextToExplore.push_back(neighbour);
                        neighbour->visitedBruteForce = true;
                    }
                }

            }

            toExplore = nextToExplore;
            numIterations++;

        }
    }

    return (minDistance == std::numeric_limits<int>::max() ? -1 : minDistance);
}

int solveOptimised(vector<Node>& nodes, int colourToSolveFor)
{
    vector<Node*> nodesToExplore;
    for (auto& node : nodes)
    {
        if (node.colour == colourToSolveFor)
        {
            // Don't mark as visited!
            nodesToExplore.push_back(&node);
        }
    }

    if (nodesToExplore.size() <= 1)
    {
        cout << -1 << endl;
        return -1;
    }

    int numIterations = 0;
    while (!nodesToExplore.empty())
    {
        //cout << "iteration: " << numIterations << endl;
        vector<Node*> nextNodesToExplore;
        for (auto node : nodesToExplore)
        {
            //cout << " exploring node: " << node << " colour: " << node->colour << endl;
            if (numIterations != 0 && node->colour == colourToSolveFor)
            {
                return numIterations;
            }
            for (auto neighbour : node->neigbours)
            {
                if (!neighbour->visited)
                {
                    //cout << "  adding neighbour: " << neighbour << " colour: " << neighbour->colour << endl;
                    nextNodesToExplore.push_back(neighbour);
                }
                neighbour->visited = true;
            }
        }
        nodesToExplore = nextNodesToExplore;
        numIterations++;
    }
    return -1;
}

int main()
{
    int numNodes;
    cin >> numNodes;

    int numEdges;
    cin >> numEdges;

    vector<Node> nodes(numNodes);

    for (int i = 0; i < numEdges; i++)
    {
        int node1;
        cin >> node1;
        int node2;
        cin >> node2;

        node1--;
        node2--;

        nodes[node1].neigbours.push_back(&(nodes[node2]));
        nodes[node2].neigbours.push_back(&(nodes[node1]));
    }

    for (int i = 0; i < numNodes; i++)
    {
        cin >> nodes[i].colour;
    }

    int colourToSolveFor;
    cin >> colourToSolveFor;

    const auto bruteForceSolution = solveBruteForce(nodes, colourToSolveFor);
    cout << "bruteForceSolution: " << bruteForceSolution << endl;

    const auto optimisedSolution = solveOptimised(nodes, colourToSolveFor);
    cout << "optimisedSolution: " << optimisedSolution << endl;
}
