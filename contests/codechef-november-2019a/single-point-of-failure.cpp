// Simon St James (ssjgz) - 2019-11-02
// 
// Solution to: https://www.codechef.com/NOV19A/problems/FAILURE
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

struct Node
{
    vector<Node*> neighbours;
    int id = -1;
    bool isInCycle = false;
    Node* nextInCycle = nullptr;

    bool visitedInDFS = false;
    bool isInDFSStack = false;
    bool isRemoved = false;
    int componentNum = -1;
};

class NodeMultiSet
{
    public:
        NodeMultiSet(int maxNodeId)
            : m_numNodesWithId{maxNodeId + 1}
        {
        }
        void addNode(Node* nodeToAdd)
        {
            m_numNodesWithId[nodeToAdd->id]++;
            if (m_numNodesWithId[nodeToAdd->id] == 1)
            {
                // Node was not in set; add it.
                m_nodesInSet.push_back(nodeToAdd);
            }
        }
        vector<std::pair<Node*, int>> nodesAndOccurrences() const
        {
            vector<std::pair<Node*, int>> nodesAndOccurrences;
            for (auto node : m_nodesInSet)
            {
                nodesAndOccurrences.push_back({node, m_numNodesWithId[node->id]});
            }
            return nodesAndOccurrences;
        }
    private:
        vector<Node*> m_nodesInSet;
        vector<int> m_numNodesWithId;
};

void findACycleAux(Node* currentNode, Node* parentNode, vector<Node*>& destCycle, vector<Node*>& ancestors)
{
    if (currentNode->isRemoved)
        return;
    if (currentNode->visitedInDFS)
    {
        if (currentNode->isInDFSStack)
        {
            //cout << " found cycle: currentNode: " << (currentNode->id) << " parent: " << (parentNode == nullptr ? -1 : parentNode->id) << " # ancestors: " << ancestors.size() << endl;
            destCycle = ancestors;
        }
        return;
    }
    currentNode->visitedInDFS = true;
    ancestors.push_back(currentNode);
    currentNode->isInDFSStack = true;

    for (auto childNode : currentNode->neighbours)
    {
        if (childNode == parentNode)
            continue;

        findACycleAux(childNode, currentNode, destCycle, ancestors);
    }

    ancestors.pop_back();
    currentNode->isInDFSStack = false;

}

vector<Node*> findACycle(Node* startNode, vector<Node>& nodes)
{
    for (auto& node : nodes)
    {
        node.visitedInDFS = false;
        node.isInDFSStack = false;
    }
    vector<Node*> cycle;
    vector<Node*> ancestors;
    findACycleAux(startNode, nullptr, cycle, ancestors);
    
    return cycle;
}

bool isRobust(vector<Node>& nodes)
{
    for (auto& startNode : nodes)
    {
        const auto cycle = findACycle(&startNode, nodes);
        if (!cycle.empty())
        {
#if 0
            cout << " found cycle: ";
            for (const auto node : cycle)
            {
                cout << " " << node->id;
            }
            cout << endl;
#endif
            return true;
        }
    }

    return false;
}


#if 1
int solveBruteForce(vector<Node>& nodes)
{
    if (!isRobust(nodes))
        return -1;

    for (auto& node : nodes)
    {
        //cout << "Removing node with id: " << node.id << endl;
        node.isRemoved = true;
        const bool stillRobust = isRobust(nodes);
        //cout << "stillRobust? " << stillRobust << endl;
        if (!stillRobust)
            return node.id;
        node.isRemoved = false;
    }
    
    return -1;
}
#endif

vector<Node*> getComponent(Node* rootNode, int markAsComponentNum)
{
    assert(rootNode->componentNum == -1);
    assert(!rootNode->isRemoved);
    vector<Node*> component = { rootNode };
    vector<Node*> toExplore = {rootNode};
    rootNode->componentNum = markAsComponentNum;

    while (!toExplore.empty())
    {
        vector<Node*> nextToExplore;
        for (auto node : toExplore)
        {
            for (auto neighbour : node->neighbours)
            {
                if (!neighbour->isRemoved && neighbour->componentNum == -1)
                {
                    neighbour->componentNum = markAsComponentNum;
                    component.push_back(neighbour);
                    nextToExplore.push_back(neighbour);
                }
            }
        }

        toExplore = nextToExplore;
    }

    return component;

}

bool componentHasCycle(const vector<Node*>& component)
{
    int numEdgesTimesTwo = 0;
    for (const auto node : component)
    {
        numEdgesTimesTwo += node->neighbours.size();
    }
    assert(numEdgesTimesTwo % 2 == 0);
    return (numEdgesTimesTwo / 2) != component.size() - 1;
}

#if 1
int solveOptimised(vector<Node>& nodes)
{
    int result = 0;
    vector<vector<Node*>> components;
    for (auto& startNode : nodes)
    {
        if (startNode.componentNum == -1)
        {
            auto component = getComponent(&startNode, components.size());
            components.push_back(component);
        }
    }

    vector<Node*> cycle;
    for (const auto& component : components)
    {
        cycle = findACycle(component.front(), nodes);
        if (!cycle.empty())
            break;
    }

    if (cycle.empty())
        return -1;

    for (int i = 0; i < cycle.size(); i++)
    {
        cycle[i]->isInCycle = true;
        cycle[i]->isRemoved = true;
        cycle[i]->nextInCycle = cycle[(i + 1) % cycle.size()];
    }

    for (auto& node : nodes)
    {
        node.componentNum = -1;
    }
    components.clear();
    for (auto& startNode : nodes)
    {
        if (startNode.componentNum == -1)
        {
            auto component = getComponent(&startNode, components.size());
            components.push_back(component);
        }
    }
    for (const auto& component : components)
    {
        if (componentHasCycle(component))
            return -1;
    }

    // None of the components has a cycle in G-C.
    
    return result;
}
#endif


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            vector<pair<int, int>> allEdges;
            const int N = 2 + rand() % 100; // TODO - Not sure if self-loops are permitted, yet - if so, allow N = 1.
            for (int i = 1; i <= N; i++)
            {
                for (int j = i + 1; j <= N; j++)
                {
                    allEdges.push_back({i, j});
                }
            }
            random_shuffle(allEdges.begin(), allEdges.end());
            const int numEdges = 1 + rand() % allEdges.size();

            cout << N << " " << numEdges << endl;
            for (int i = 0; i < numEdges; i++)
            {
                cout << allEdges[i].first << " " << allEdges[i].second << endl;
            }
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        cout << "t: " << (t + 1) << endl;

        const int numNodes = read<int>();
        const int numEdges = read<int>();

        vector<Node> nodes;
        nodes.reserve(numNodes + numEdges); // We might be creating up to numEdges synthetic nodes.
        nodes.resize(numNodes);
        for (int i = 0; i < numNodes; i++)
        {
            nodes[i].id = (i + 1);
        }

        for (int i = 0; i < numEdges; i++)
        {
            const int u = read<int>() - 1;
            const int v = read<int>() - 1;

            nodes[u].neighbours.push_back(&(nodes[v]));
            nodes[v].neighbours.push_back(&(nodes[u]));
        }

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(nodes);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(nodes);
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
