// Simon St James (ssjgz) - 2019-06-16
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>


#include <algorithm>
#include <set>
#include <sys/time.h>

using namespace std;

struct Node
{
    Node()
    {
        distinctSourceInfo.containerNode = this;
        nextDistinctSourceInfo.containerNode = this;
    }
    int colour = -1;
    bool visited = false;
    vector<Node*> neigbours;
    int id = -1;
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
        Node* containerNode = nullptr;

        void incorporateSourcesFrom(Node* visitor)
        {
            //cout << " incorporateSourcesFrom: " << visitor->id << " this node: " << containerNode->id << " colour: " << containerNode->colour << " numDistinctSources: " << numDistinctSources << endl;
            if (numDistinctSources > 1)
            {
                //cout << " already saturated";
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
            //cout << "  new numDistinctSources: " << numDistinctSources << endl;
        }
    };

    DistinctSourceInfo distinctSourceInfo;
    DistinctSourceInfo nextDistinctSourceInfo;

    bool visitedBruteForce = false;
};

int solveBruteForce(vector<Node>& nodes, int colourToSolveFor)
{
    int minDistance = numeric_limits<int>::max();
    Node *bestNode1 = nullptr;
    Node *bestNode2 = nullptr;
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
                    if (numIterations < minDistance)
                    {
                        bestNode1 = &rootNode;
                        bestNode2 = node;
                        minDistance = numIterations;
                    }
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

    if (minDistance == std::numeric_limits<int>::max())
        minDistance = -1;

    if (minDistance != -1)
    {
        cout << "Coloured nodes " << bestNode1->id << " and " << bestNode2->id << " can reach each other in " << minDistance << endl;
    }

    return minDistance;
}

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
            node.nextDistinctSourceInfo = node.distinctSourceInfo;
        }
    }

    int numIterations = 0;
    while (!nodesToExplore.empty())
    {
        //cout << "iteration: " << numIterations << " nodes to explore: ";
        //for (auto node : nodesToExplore)
        //{
            //cout << node->id << " ";
        //}
        //cout << endl;

        vector<Node*> nextNodesToExplore;
        for (auto node : nodesToExplore)
        {
            if (node->colour == colourToSolveFor && node->distinctSourceInfo.numDistinctSources > 1)
            {
                //cout << " found correct node " << node->id << " sources: " << node->distinctSourceInfo.distinctSources[0].node->id << ", " << node->distinctSourceInfo.distinctSources[1].node->id << endl;
                return numIterations;
            }
        }
        for (auto node : nodesToExplore)
        {
            //cout << " exploring node: " << node->id << " colour: " << node->colour << " numDistinctSources: " << node->distinctSourceInfo.numDistinctSources << endl;
            for (auto neighbour : node->neigbours)
            {
                if (!neighbour->alreadyInNextNodesToExplore && neighbour->shouldReExploreAfterVisitFrom(node))
                {
                    //cout << "  adding neighbour: " << neighbour << " colour: " << neighbour->colour << endl;
                    nextNodesToExplore.push_back(neighbour);
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
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numNodes = rand() % 10 + 1;
        const int maxEdges = numNodes * (numNodes - 1) / 2;
        const int minEdges = numNodes - 1;
        const int numEdges = (rand() % (maxEdges - minEdges + 1)) + minEdges;
        assert(numEdges >= minEdges && numEdges <= maxEdges);

        set<pair<int, int>> allPossibleEdges;
        for (int node1 = 1; node1 <= numNodes; node1++)
        {
            for (int node2 = node1 + 1; node2 <= numNodes; node2++)
            {
                allPossibleEdges.insert({node1, node2});
            }
        }
        assert(allPossibleEdges.size() == maxEdges);

        vector<pair<int, int>> edges;
        for (int i = 0; i < numNodes; i++)
        {
            if (i > 0)
            {
                const pair<int, int> edge = { (rand() % i) + 1, i + 1 };
                assert(edge.first < edge.second);
                edges.push_back(edge);
                allPossibleEdges.erase(edge);
            }
        }
        vector<pair<int, int>> remainingEdges(allPossibleEdges.begin(), allPossibleEdges.end());
        random_shuffle(remainingEdges.begin(), remainingEdges.end());

        while (edges.size() < numEdges)
        {
            edges.push_back(remainingEdges.back());
            remainingEdges.pop_back();
        }

        cout << numNodes << " " << numEdges << endl;

        for (const auto edge : edges)
        {
            cout << edge.first << " " << edge.second << endl;
        }

        const int maxColour = rand() % 100 + 1;

        for (int i = 0; i < numNodes; i++)
        {
            cout << ((rand() % maxColour) + 1) << endl;
        }

        cout << ((rand() % maxColour) + 1) << endl;


        return 0;

    }
    int numNodes;
    cin >> numNodes;

    int numEdges;
    cin >> numEdges;

    vector<Node> nodes(numNodes);
    for (int i = 0; i < numNodes; i++)
    {
        nodes[i].id = i + 1;
    }

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
    assert(cin);

    int colourToSolveFor;
    cin >> colourToSolveFor;

#ifdef BRUTE_FORCE
    cout << "colourToSolveFor: " << colourToSolveFor << endl;

    const auto bruteForceSolution = solveBruteForce(nodes, colourToSolveFor);

    const auto optimisedSolution = solveOptimised(nodes, colourToSolveFor);
    cout << "bruteForceSolution: " << bruteForceSolution << " optimisedSolution: " << optimisedSolution << endl;

    assert(bruteForceSolution == optimisedSolution);
#else
    cout << solveOptimised(nodes, colourToSolveFor) << endl;
#endif
}
