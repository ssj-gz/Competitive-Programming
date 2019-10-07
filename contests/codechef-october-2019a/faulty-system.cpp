// Simon St James (ssjgz) - 2019-10-07
// 
// Solution to: https://www.codechef.com/OCT19A/problems/CNNCT2
//
// No idea how to solve this - just doing an O(N x 2^M) brute-force
// solution to scrounge up some points XD
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

struct Edge;
struct Node
{
    vector<Edge*> edges;
    bool visited = false;
};

struct Edge
{
    bool isOpen = false;

    Node* nodeA[2] = {};
    Node* nodeB[2] = {};

    Node* otherNode(Node* node, int stateNum)
    {
        if (nodeA[stateNum] == node)
            return nodeB[stateNum];
        if (nodeB[stateNum] == node)
            return nodeA[stateNum];
        assert(false);
        return nullptr;
    }
};

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

bool isConnected(vector<Node>& nodes, int stateNum)
{
    for (auto& node : nodes)
        node.visited = false;

    vector<Node*> toExplore = { &(nodes.front()) };
    while (!toExplore.empty())
    {
        vector<Node*> nextToExplore;
        for (auto nodeToExplore : toExplore)
        {
            for (const auto edge : nodeToExplore->edges)
            {
                if (edge->isOpen)
                {
                    auto neighbour = edge->otherNode(nodeToExplore, stateNum);
                    if (!neighbour->visited)
                    {
                        neighbour->visited = true;
                        nextToExplore.push_back(neighbour);
                    }
                }
            }
        }

        toExplore = nextToExplore;
    }

    for (auto& node : nodes)
    {
        if (!node.visited)
            return false;
    }
    return true;
}

int solveBruteForce(vector<Node>& state1Nodes, vector<Node>& state2Nodes, vector<Edge>& edges)
{
    for (int numToChoose = 1; numToChoose <= edges.size(); numToChoose++)
    {
        vector<int> choices(numToChoose);
        for (int i = 0; i < numToChoose; i++)
        {
            choices[i] = i;
        }
        while (true)
        {
            // Test this set of choices.
            for (auto& edge : edges)
                edge.isOpen = false;

            for (auto choice : choices)
            {
                edges[choice].isOpen = true;
            }

            if (isConnected(state1Nodes, 0) && isConnected(state2Nodes, 0))
                return numToChoose;

            // Next choices.
            int index = numToChoose - 1;
            while (index >= 0 && choices[index] == edges.size() - 1 - (numToChoose - 1 - index))
            {
                index--;
            }
            if (index < 0)
                break;
            int nextChoice = choices[index] + 1;
            for (int i = index; i < numToChoose; i++)
            {
                choices[i] = nextChoice;
                nextChoice++;
            }

        }
    }
    return -1;
}

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
        const int N = read<int>();
        const int M = read<int>();

        vector<Edge> edges(M);

        vector<Node> state1Nodes(N);
        {
            for (int i = 0; i < M; i++)
            {
                Node* nodeU = &(state1Nodes[read<int>() - 1]);
                Node* nodeV = &(state1Nodes[read<int>() - 1]);

                Edge* edge = &(edges[i]);
                edge->nodeA[0] = nodeU;
                edge->nodeB[0] = nodeV;

                nodeU->edges.push_back(edge);
                nodeV->edges.push_back(edge);
            }
        }
        vector<Node> state2Nodes(N);
        {
            for (int i = 0; i < M; i++)
            {
                Node* nodeU = &(state2Nodes[read<int>() - 1]);
                Node* nodeV = &(state2Nodes[read<int>() - 1]);

                Edge* edge = &(edges[i]);
                edge->nodeA[1] = nodeU;
                edge->nodeB[1] = nodeV;

                nodeU->edges.push_back(edge);
                nodeV->edges.push_back(edge);
            }
        }
#ifdef BRUTE_FORCE
#if 0
        const auto solutionBruteForce = solveBruteForce();
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
