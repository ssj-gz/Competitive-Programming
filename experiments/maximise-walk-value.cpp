// Simon St James (ssjgz) - 2019-XX-XX
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h>

using namespace std;

    template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

#if 0
vector<int64_t> solveBruteForce()
{
    vector<int64_t> result;

    return result;
}
#endif

const auto maxCost = 1000;

struct CostInfo
{
    bool canBeGenerated = false;
    int nextLowestGeneratableCost = -1;
};

struct Node
{
    vector<Node*> neighbours;
    int id = -1;

    int cost = 0;

    int specialNodeIndex = -1;

    vector<vector<CostInfo>> generatableCostsOnPathToSpecialNode;
};

void doDfsFromSpecialNode(Node* currentNode, Node* parent, const int sourceSpecialNodeId)
{
    auto& generatableCostsOnPathToSpecialNode = currentNode->generatableCostsOnPathToSpecialNode[sourceSpecialNodeId];
    if (parent)
    {
        generatableCostsOnPathToSpecialNode = parent->generatableCostsOnPathToSpecialNode[sourceSpecialNodeId];
        for (int i = 0; i <= maxCost; i++)
        {
            if (generatableCostsOnPathToSpecialNode[i].canBeGenerated)
            {
                if (i + currentNode->cost <= maxCost)
                {
                    generatableCostsOnPathToSpecialNode[i + currentNode->cost].canBeGenerated = true;
                }
                else
                {
                    break;
                }
            }
        }
    }
    else
    {
        generatableCostsOnPathToSpecialNode.resize(maxCost + 1);
        generatableCostsOnPathToSpecialNode[0].canBeGenerated = true;
        generatableCostsOnPathToSpecialNode[currentNode->cost].canBeGenerated = true;
    }

    int nextLowestGeneratableCost = -1;
    for (int cost = 0; cost <= maxCost; cost++)
    {
        generatableCostsOnPathToSpecialNode[cost].nextLowestGeneratableCost = nextLowestGeneratableCost;
        if (generatableCostsOnPathToSpecialNode[cost].canBeGenerated)
            nextLowestGeneratableCost = cost;
    }
}

void buildLookupTables(vector<Node>& nodes, const vector<Node*>& specialNodes)
{
    for (auto& node : nodes)
    {
        node.generatableCostsOnPathToSpecialNode.resize(specialNodes.size());
    }

    for (auto specialNode : specialNodes)
    {
    }
}

vector<int64_t> solveOptimised()
{
    vector<int64_t> result;

    return result;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        return 0;
    }

    const int numNodes = read<int>();
    const int numSpecialNodes = read<int>();
    const int numQueries = read<int>();

    vector<Node> nodes(numNodes);
    for (int i = 0; i < numNodes; i++)
    {
        nodes[i].id = i + 1;
    }

    for (int i = 0; i < numNodes - 1; i++)
    {
        const int v = read<int>() - 1;

        nodes[v].neighbours.push_back(&(nodes[i]));
        nodes[i].neighbours.push_back(&(nodes[v]));
    }

    for (int i = 0; i < numNodes; i++)
    {
        nodes[i].cost = read<int>();
    }

    vector<Node*> specialNodes;

    for (int i = 0; i < numSpecialNodes; i++)
    {
        specialNodes.push_back(&(nodes[read<int>() - 1]));
        specialNodes.back()->specialNodeIndex = i;
    }

    for (int q = 0; q < numQueries; q++)
    {
        Node* sourceNode = &(nodes[read<int>() - 1]);
        Node* destNode = &(nodes[read<int>() - 1]);
        int maxCost = read<int>() - 1;
    }

#ifdef BRUTE_FORCE
#if 0
    const auto solutionBruteForce = solveBruteForce();
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 0
    const auto solutionOptimised = solveOptimised();
    cout << "solutionOptimised: " << solutionOptimised << endl;

    assert(solutionOptimised == solutionBruteForce);
#endif
#else
    const auto solutionOptimised = solveOptimised();
    cout << solutionOptimised << endl;
#endif

}


