// Simon St James (ssjgz) - 2019-06-16
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct Node
{
    int colour = -1;
    vector<Node*> neigbours;
    bool alreadyInNextNodesToExplore = false;

    bool shouldReExploreAfterVisitFrom(Node* visitor)
    {
        if (distinctSourceInfo.numDistinctSources > 1)
            return false;
        for (int i = 0; i < visitor->distinctSourceInfo.numDistinctSources; i++)
        {
            bool hasSourceAlready = false;
            auto visitorSource = visitor->distinctSourceInfo.distinctSources[i].node;
            for (int j = 0; j < distinctSourceInfo.numDistinctSources; j++)
            {
                if (visitorSource == distinctSourceInfo.distinctSources[j].node)
                {
                    hasSourceAlready = true;
                    break;
                }
            }
            if (!hasSourceAlready)
                return true;
        }
        return false;
    }

    struct DistinctSource
    {
        Node* node = nullptr;
    };
    struct DistinctSourceInfo
    {
        int numDistinctSources = 0;
        DistinctSource distinctSources[2] = {};

        void incorporateSourcesFrom(Node* visitor)
        {
            if (numDistinctSources > 1)
            {
                // Already have all the sources we need.
                return;
            }
            for (int i = 0; i < visitor->distinctSourceInfo.numDistinctSources; i++)
            {
                bool hasSourceAlready = false;
                auto visitorSource = visitor->distinctSourceInfo.distinctSources[i].node;
                for (int j = 0; j < numDistinctSources; j++)
                {
                    if (visitorSource == distinctSources[j].node)
                    {
                        hasSourceAlready = true;
                        break;
                    }
                }
                if (!hasSourceAlready)
                {
                    distinctSources[numDistinctSources].node = visitorSource;
                    numDistinctSources++;
                }
            }
        }
    };

    DistinctSourceInfo distinctSourceInfo;
    DistinctSourceInfo nextDistinctSourceInfo;

    bool visitedBruteForce = false;
};

int solveOptimised(vector<Node>& nodes, int colourToSolveFor)
{
    vector<Node*> nodesToExplore;
    for (auto& node : nodes)
    {
        if (node.colour == colourToSolveFor)
        {
            nodesToExplore.push_back(&node);
            node.distinctSourceInfo.numDistinctSources = 1;
            node.distinctSourceInfo.distinctSources[0].node = &node;
        }
    }

    int numIterations = 0;
    while (!nodesToExplore.empty())
    {
        vector<Node*> nextNodesToExplore;
        for (auto node : nodesToExplore)
        {
            if (node->colour == colourToSolveFor && node->distinctSourceInfo.numDistinctSources > 1)
            {
                return numIterations;
            }
        }
        for (auto node : nodesToExplore)
        {
            for (auto neighbour : node->neigbours)
            {
                if (!neighbour->alreadyInNextNodesToExplore && neighbour->shouldReExploreAfterVisitFrom(node))
                {
                    nextNodesToExplore.push_back(neighbour);
                    neighbour->nextDistinctSourceInfo = neighbour->distinctSourceInfo;
                    neighbour->alreadyInNextNodesToExplore = true;
                }
                neighbour->nextDistinctSourceInfo.incorporateSourcesFrom(node);
            }
        }

        for (auto node : nextNodesToExplore)
        {
            node->distinctSourceInfo = node->nextDistinctSourceInfo;
            node->alreadyInNextNodesToExplore = false;
        }
        nodesToExplore = nextNodesToExplore;
        numIterations++;
    }
    return -1;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    auto readInt = []() { int x; cin >> x; assert(cin); return x; };
    const int numNodes = readInt();

    const int numEdges = readInt();

    vector<Node> nodes(numNodes);

    for (int i = 0; i < numEdges; i++)
    {
        // The "- 1" makes node ids 0-relative.
        const int node1 = readInt() - 1;
        const int node2 = readInt() - 1;

        nodes[node1].neigbours.push_back(&(nodes[node2]));
        nodes[node2].neigbours.push_back(&(nodes[node1]));
    }

    for (int i = 0; i < numNodes; i++)
    {
        cin >> nodes[i].colour;
    }
    assert(cin);

    const int colourToSolveFor = readInt();

    cout << solveOptimised(nodes, colourToSolveFor) << endl;
}
