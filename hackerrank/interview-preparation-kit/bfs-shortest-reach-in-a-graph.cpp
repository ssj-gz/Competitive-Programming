// Simon St James (ssjgz) - 2019-06-17
#include <iostream>
#include <vector>

using namespace std;

struct Node
{
    vector<Node*> neighbours;
    bool visited = false;
    int distanceFromRoot = -1;
};

int main()
{
    // "Hard"?? This is utterly trivial - and what's weirder, a challenge that precedes
    // this in this track - Find the Nearest Clone - is rated Medium but is considerably
    // more difficult.
    //
    // Anyway - just do a BFS to find minimum distance from the root (== number of steps
    // to reach it, times 6).  Bizarre.
    int Q;
    cin >> Q;

    for (int q = 0; q < Q; q++)
    {
        int numNodes;
        cin >> numNodes;

        int numEdges;
        cin >> numEdges;

        vector<Node> nodes(numNodes);
        for (int i = 0; i < numEdges; i++)
        {
            int u, v;
            cin >> u >> v;

            // Make 0-relative.
            u--;
            v--;

            nodes[u].neighbours.push_back(&(nodes[v]));
            nodes[v].neighbours.push_back(&(nodes[u]));
        }

        int rootNodeId;
        cin >> rootNodeId;
        // Make 0-relative.
        rootNodeId--;

        auto rootNode = &(nodes[rootNodeId]);
        vector<Node*> nodesToExplore = { rootNode };

        int numIterations = 0;
        while (!nodesToExplore.empty())
        {
            vector<Node*> nextNodesToExplore;
            for (auto node : nodesToExplore)
            {
                for (auto neighbour : node->neighbours)
                {
                    if (neighbour->visited)
                        continue;

                    neighbour->distanceFromRoot = 6 * (numIterations + 1);
                    neighbour->visited = true;
                    nextNodesToExplore.push_back(neighbour);
                }
            }

            numIterations++;
            nodesToExplore = nextNodesToExplore;
        }

        for (auto& node : nodes)
        {
            if (&node == rootNode)
                continue;
            cout << node.distanceFromRoot << " ";
        }
        cout << endl;
    }

}
