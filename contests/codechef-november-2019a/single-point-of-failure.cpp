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

    bool visitedInDFS = false;
    bool isInDFSStack = false;
    bool isRemoved = false;
    int componentNum = -1;
};

void findACycleAux(Node* currentNode, Node* parentNode, vector<Node*>& destCycle, vector<Node*>& ancestors)
{
    if (currentNode->visitedInDFS)
    {
        if (currentNode->isInDFSStack)
        {
            destCycle = ancestors;
            return;
        }
    }
    currentNode->visitedInDFS = true;
    ancestors.push_back(currentNode);
    currentNode->isInDFSStack = true;

    for (auto childNode : currentNode->neighbours)
    {
        if (childNode == parentNode)
            continue;

        findACycleAux(childNode, parentNode, destCycle, ancestors);
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


#if 1
int solveBruteForce(vector<Node>& nodes)
{
    int result = -1;

    cout << "Cycle size: " << findACycle(&(nodes.front()), nodes).size() << endl;
    
    return result;
}
#endif

#if 0
SolutionType solveOptimised()
{
    SolutionType result;
    
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
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {

        const int numNodes = read<int>();
        const int numEdges = read<int>();

        vector<Node> nodes(numNodes);
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
#if 0
        const auto solutionOptimised = solveOptimised();
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
