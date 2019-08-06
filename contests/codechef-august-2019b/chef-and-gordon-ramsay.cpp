// Simon St James (ssjgz) - 2019-08-06

// O(N^3) solution - no chance of getting AC, but let's check correctness
// and scrounge up some points at least :)

#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

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

struct Node
{
    vector<Node*> neighbours;
    int id = -1;
};

void solutionBruteForceAux(Node* startNode, Node* currentNode, Node* parentNode, const array<int, 3>& P, vector<Node*> nodesSoFar, int64_t& result)
{

    //cout << "startNode: " << startNode->id << " currentNode: " << currentNode->id << endl;

    auto havePOrdering = [&P](Node* node1, Node* node2, int pIndex1, int pIndex2) -> bool
    {
       return (((node1->id < node2->id) && (P[pIndex1] < P[pIndex2])) ||
               ((node1->id > node2->id) && (P[pIndex1] > P[pIndex2])));
    };

    if (havePOrdering(startNode, currentNode, 0, 2))
    {
        //cout << " have POrdering" << endl;
        for (auto nodeinPath : nodesSoFar)
        {
            //cout << "  nodeinPath: " << nodeinPath->id << endl;
            if (havePOrdering(startNode, nodeinPath, 0, 1) && havePOrdering(nodeinPath, currentNode, 1, 2))
            {
                //cout << "Found! (" << startNode->id << ", " << nodeinPath->id << ", " << currentNode->id << ")" << endl;
                result++;
            }
        }

    }


    if (currentNode != startNode)
        nodesSoFar.push_back(currentNode);

    for (auto child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;
        solutionBruteForceAux(startNode, child, currentNode, P, nodesSoFar, result);
    }
    
    if (currentNode != startNode)
        nodesSoFar.pop_back();
}


#if 1
int64_t solveBruteForce(vector<Node>& nodes, const array<int, 3>& P)
{
    int64_t result = 0;

    for (auto& node : nodes)
    {
        vector<Node*> nodesSoFar;
        solutionBruteForceAux(&node, &node, nullptr, P, nodesSoFar, result);
    }

    return result;
}
#endif

#if 1
int64_t solveOptimised(vector<Node>& nodes, const array<int, 3>& P)
{
    int64_t result = solveBruteForce(nodes, P);

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

        const int maxN = 10;
        const int N = rand() % maxN + 1;

        vector<int> P = {1, 2, 3};
        random_shuffle(P.begin(), P.end());

        cout << 1 << endl;
        cout << N << endl;

        cout << P[0] << " " << P[1] << " " << P[2] << endl;

        vector<int> nodeIds;
        for (int i = 1; i <= N; i++)
        {
            nodeIds.push_back(i);
        }
        random_shuffle(nodeIds.begin(), nodeIds.end());

        vector<int> usedNodeIds = { nodeIds.back() };
        nodeIds.pop_back();

        for (int i = 0; i < N - 1; i++)
        {
            const int newNodeId = nodeIds.back();
            const int oldNodeId = usedNodeIds[rand() % usedNodeIds.size()];

            cout << newNodeId << " " << oldNodeId << endl;

            usedNodeIds.push_back(newNodeId);
            nodeIds.pop_back();
        }

        return 0;
    }

    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();

        vector<Node> nodes(N);
        for (int i = 0; i < N; i++)
        {
            nodes[i].id = i + 1;
        }

        array<int, 3> P;
        for (auto& p : P)
        {
            p = read<int>();
        }

        for (int i = 0; i < N - 1; i++)
        {
            const int u = read<int>() - 1;
            const int v = read<int>() - 1;

            nodes[u].neighbours.push_back(&(nodes[v]));
            nodes[v].neighbours.push_back(&(nodes[u]));
        }
#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(nodes, P);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 0
        const auto solutionOptimised = solveOptimised();
        cout << "solutionOptimised: " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised(nodes, P);
        cout << solutionOptimised << endl;
#endif
    }


}


