// Simon St James (ssjgz) - 2019-10-05
// 
// Solution to: https://www.codechef.com/OCT19A/problems/EVEDG
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
    int subgraphNum = -1;
};

bool isValidPartitioning(const vector<Node>& nodes)
{
    for (const auto& node : nodes)
    {
        int nodeDegree = 0;
        for (const auto neighbour : node.neighbours)
        {
            if (neighbour->subgraphNum == node.subgraphNum)
            {
                nodeDegree++;
            }
        }
        if ((nodeDegree % 2) == 1)
            return false;
    }
    return true;
}

std::pair<int, vector<int>> solveBruteForce(vector<Node>& nodes)
{
    const int N = nodes.size();

    int numsubGraphs = 1;
    while (true)
    {
        vector<int> subgraphForNode(N, 0);
        while (true)
        {
            for (int i = 0; i < N; i++)
            {
                nodes[i].subgraphNum = subgraphForNode[i];
            }
            if (isValidPartitioning(nodes))
                return {numsubGraphs, subgraphForNode};

            int index = 0;
            while(index < N && subgraphForNode[index] == (numsubGraphs - 1))
            {
                subgraphForNode[index] = 0;
                index++;
            }
            if (index == N)
                break;
            subgraphForNode[index]++;
        }

        numsubGraphs++;
    }
    
    return {-1, vector<int>()};
}

#if 0
int solveOptimised()
{
    int result = 0;
    
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
        vector<Node> nodes(N);

        const int M = read<int>();

        for (int i = 0; i < M; i++)
        {
            const int u = read<int>() - 1;
            const int v = read<int>() - 1;

            nodes[u].neighbours.push_back(&(nodes[v]));
            nodes[v].neighbours.push_back(&(nodes[u]));
        }

#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(nodes);
        cout << "solutionBruteForce: " << solutionBruteForce.first << endl;
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
