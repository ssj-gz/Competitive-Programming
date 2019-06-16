#include <iostream>
#include <vector>

using namespace std;

struct Node
{
    int colour = -1;
    bool visited = false;
    vector<Node*> neigbours;
};

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

    vector<Node*> nodesToExplore;
    for (auto& node : nodes)
    {
        if (node.colour == colourToSolveFor)
        {
            // Don't mark as visited!
            nodesToExplore.push_back(&node);
        }
    }

    int distance = -1;
    int numIterations = 0;
    while (distance != -1 && !nodesToExplore.empty())
    {
        vector<Node*> nextNodesToExplore;
        for (auto node : nodesToExplore)
        {
            if (numIterations != 0 && node->colour == colourToSolveFor)
            {
                distance = numIterations;
                break;
            }
            for (auto neighbour : node->neigbours)
            {
                if (!neighbour->visited)
                {
                    nextNodesToExplore.push_back(neighbour);
                }
                neighbour->visited = true;
            }
        }
        nodesToExplore = nextNodesToExplore;
    }

    cout << distance << endl;
}
