// Simon St James (ssjgz) - 2019-10-05
// 
// Solution to: https://www.codechef.com/OCT19A/problems/EVEDG
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
//#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>

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
    int colour = -1;

    int id = -1;
};

bool isValidPartitioning(const vector<Node>& nodes)
{
    int numSubgraphs = -1;
    for (const auto& node : nodes)
    {
        if (node.subgraphNum + 1 > numSubgraphs)
        {
            numSubgraphs = node.subgraphNum + 1;
        }
    }
    vector<int> twiceNumEdgesInSubgraph(numSubgraphs, 0);
    for (const auto& node : nodes)
    {
        for (const auto neighbour : node.neighbours)
        {
            if (neighbour->subgraphNum == node.subgraphNum)
            {
                twiceNumEdgesInSubgraph[node.subgraphNum]++;
            }
        }
    }
    for (const auto twiceNumEdges : twiceNumEdgesInSubgraph)
    {
        const auto numEdges = twiceNumEdges / 2;
        if ((numEdges % 2) == 1)
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
        cout << " Trying numsubGraphs: " << numsubGraphs << endl;
    }
    
    return {-1, vector<int>()};
}

std::pair<int, vector<int>> findMinSubgraphPartitioning(vector<Node>& nodes, const int numEdges)
{
    vector<int> subgraphForNode(nodes.size(), 1);
    if (numEdges % 2 == 0)
    {
        // Already satisfies requirements; no partitioning required.
        return {1, subgraphForNode};
    }

    // From this point on, the number of edges is odd.
    for (const auto& node : nodes)
    {
        if (node.neighbours.size() % 2 == 1)
        {
            // Found a node with an odd number of edges incident; if we move
            // this node into its own partition, then the resulting graph
            // will have exactly the same set of vertices as the original *minus*
            // the (odd number of) edges incident with node.
            //
            // Since an odd number minus an odd number is even, this partitioning
            // gives us an even number of edges.
            subgraphForNode[node.id] = 2;
            return {2, subgraphForNode};
        }
    }

    // From this point on, we have an odd number of edges, but every node has an
    // even number of edges incident with it.
    //
    // For reasons which aren't completely clear to me, no partitioning into 
    // two partitions can satisfy the requirements, so we must use (at least) 3.
    //
    // However, as we'll see, 3 is sufficient.

    for (const auto& node : nodes)
    {
        if (!node.neighbours.empty())
        {
            // A node with > 0 incident edges must exist, as the total number of 
            // edges is odd.
            //
            // If we put this node into its own partition (partition #3), then the
            // resulting graph has the same edges as the original, *minus* those
            // incident with node.
            //
            // Since each neighbour of node now has one incident edge less, and originally
            // had an even number of incident edges, each now has an odd number of edges.
            //
            // We then pick any one such neighbour and move it into its own partition (#2) and
            // we have our partition for the reasons described in the "Found a node with an odd 
            // number of edges incident" case, above.
            subgraphForNode[node.id] = 3;
            subgraphForNode[node.neighbours.front()->id] = 2;
            break;
        }
    }

    return {3, subgraphForNode};
}

int main(int argc, char* argv[])
{
    // Slightly confused by this one: the code above is hopefully mostly self-explanatory, but there's
    // one thing I can't figure out:
    //
    //    If the number of edges is odd but every node has an even degree, then no partitioning into
    //    two or less partitions can satisfy the requirement.
    //
    // I've got this far: if the degree of each vertex is even, then there is an Eulerian Circuit/ Cycle on the
    // graph.  Since the number of edges is odd, this Cycle has odd length.  A graph that has an odd length cycle
    // cannot be bipartite.
    //
    // Now, bipartite-ness is *sufficient* for a partitioning into two partitions to satisfy the condition (bipartite-ness
    // implies the vertices can be divided into two partitions, with no vertices in the same partition being connected
    // by an edge - with this partitioning, *no* edges remain in the resulting graph, so the number of edges is 0, 
    // which is even), but I can't think of any reason why bipartiteness should be *necessary*.
    //
    // Oh well - will just have to wait for the Editorial, I guess :)
    ios::sync_with_stdio(false);
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

        for (int i = 0; i < N; i++)
        {
            nodes[i].id = i;
        }

        const auto minSubgraphPartitioning = findMinSubgraphPartitioning(nodes, M);
        const int numSubgraphs = minSubgraphPartitioning.first;
        const auto& subgraphForNode = minSubgraphPartitioning.second;

        cout << numSubgraphs << endl;
#if 0
        for (const auto x : subgraphForNode)
        {
            cout << x << " ";
        }
        cout << endl;
#endif
    }

    assert(cin);
}
