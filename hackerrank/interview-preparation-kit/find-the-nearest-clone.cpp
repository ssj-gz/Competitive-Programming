// Simon St James (ssjgz) - 2019-06-16
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

struct Node
{
    int colour = -1;
    vector<Node*> neigbours;
    bool alreadyInNextNodesToExplore = false;

    bool shouldReExploreAfterVisitFrom(Node* visitor)
    {
        if (distinctSourceInfo.distinctSources.size() >= 2)
        {
            // We have two distinct sources for this; no need to re-explore.
            return false;
        }
        for (auto visitorSource : visitor->distinctSourceInfo.distinctSources)
        {
            const auto haveVisitorSource = (std::find(distinctSourceInfo.distinctSources.begin(), distinctSourceInfo.distinctSources.end(), visitorSource) != distinctSourceInfo.distinctSources.end());
            if (!haveVisitorSource)
                return true;
        }
        return false;
    }

    struct DistinctSourceInfo
    {
        vector<Node*> distinctSources;

        void incorporateSourcesFrom(Node* visitor)
        {
            if (distinctSources.size() > 1)
            {
                // Already have all the sources we need.
                return;
            }
            for (auto visitorSource : visitor->distinctSourceInfo.distinctSources)
            {
                const auto alreadyHaveVisitorSource = (std::find(distinctSources.begin(), distinctSources.end(), visitorSource) != distinctSources.end());
                if (!alreadyHaveVisitorSource)
                {
                    distinctSources.push_back(visitorSource);
                    if (distinctSources.size() > 1)
                    {
                        // Already have all the sources we need.
                        return;
                    }
                }
            }
        }
    };

    DistinctSourceInfo distinctSourceInfo;
    DistinctSourceInfo nextDistinctSourceInfo;
};

int findNearestClone(vector<Node>& nodes, int cloneColour)
{
    vector<Node*> nodesToExplore;
    for (auto& node : nodes)
    {
        if (node.colour == cloneColour)
        {
            nodesToExplore.push_back(&node);
            node.distinctSourceInfo.distinctSources = {&node};
        }
    }

    int numIterations = 0;
    while (!nodesToExplore.empty())
    {
        vector<Node*> nextNodesToExplore;
        for (auto node : nodesToExplore)
        {
            if (node->colour == cloneColour && node->distinctSourceInfo.distinctSources.size() > 1)
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

    const int cloneColour = readInt();

    cout << findNearestClone(nodes, cloneColour) << endl;
}
