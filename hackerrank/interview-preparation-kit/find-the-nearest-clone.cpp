// Simon St James (ssjgz) - 2019-06-16
#include <iostream>
#include <vector>
#include <cassert>


#include <algorithm>
#include <set>
#include <sys/time.h>

using namespace std;

struct Node
{
    int colour = -1;
    bool visited = false;
    vector<Node*> neigbours;

    bool shouldReExploreAfterVisitFrom(Node* visitor)
    {
        if (numDistinctSources > 1)
            return false;

        const int originalNumDistinctSources = numDistinctSources;
        for (int i = 0; i < visitor->numDistinctSources; i++)
        {
            bool hasSourceAlready = false;
            auto visitorSource = visitor->distinctSources[i].node;
            auto visitorDistance = visitor->distinctSources[i].distance;
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
                distinctSources[numDistinctSources].distance = visitorDistance + 1;

                numDistinctSources++;
            }
        }
        if (originalNumDistinctSources != numDistinctSources)
        {
            return true;
        }
        return false;
    }

    int numDistinctSources = 0;
    struct DistinctSource
    {
        Node* node = nullptr;
        int distance = -1;
    };
    DistinctSource distinctSources[2] = {};

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
            nodesToExplore.push_back(&node);
            node.numDistinctSources = 1;
            node.distinctSources[0].node = &node;
            node.distinctSources[0].distance = 0;
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
            if (node->colour == colourToSolveFor && node->numDistinctSources > 1)
            {
                return numIterations;
            }
            for (auto neighbour : node->neigbours)
            {
                if (neighbour->shouldReExploreAfterVisitFrom(node))
                {
                    //cout << "  adding neighbour: " << neighbour << " colour: " << neighbour->colour << endl;
                    nextNodesToExplore.push_back(neighbour);
                }
                //neighbour->visited = true;
            }
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

        const int numNodes = rand() % 100 + 1;
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

    const auto bruteForceSolution = solveBruteForce(nodes, colourToSolveFor);

    const auto optimisedSolution = solveOptimised(nodes, colourToSolveFor);
    cout << "bruteForceSolution: " << bruteForceSolution << " optimisedSolution: " << optimisedSolution << endl;

    assert(bruteForceSolution == optimisedSolution);
}
