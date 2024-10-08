// Simon St James (ssjgz) - 2019-09-09
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/CHEFK1
//
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int solveBruteForceIsh(int64_t N, int64_t M)
{
    // Leaving this "brute force" solution here as a reference, as it's good documentation :)
    // It's not really "brute force" as it uses our analysis of how to build the optimal
    // graph, but it's still handy for debugging/ learning from, etc.
    if (M < N - 1)
        return -1;
    if (N == 1)
    {
        if (M == 1)
            return 1;
        if (M == 0)
            return 0;
        return -1;
    }
    if (M > ((N + 1) * N) / 2)
        return -1;
    vector<int> nodeDegree(N);
    vector<bool> nodeHasSelfLoop(N, false);
    // Join all vertices in a line, consuming N - 1 vertices.
    nodeDegree[0] = 1;
    nodeDegree[1] = 1;
    for (int i = 2; i < N; i++)
    {
        nodeDegree[i] = 2;
    }
    // Remaining vertices - add self-loop to minimum vertex when we can; else
    // add pair between the two vertices with lowest degree.
    for (int i = N; i <= M; i++)
    {
        cout << "Placing next edge; current node degrees: " << endl;
        for (const auto x : nodeDegree)
        {
            cout << x << " ";
        }
        cout << endl;

        vector<int> nodeIndicesByDegree;
        for (int i = 0; i < N; i++)
        {
            nodeIndicesByDegree.push_back(i);
        }
        sort(nodeIndicesByDegree.begin(), nodeIndicesByDegree.end(), [&nodeDegree](const auto nodeIndex1, const auto nodeIndex2)
                {
                if (nodeDegree[nodeIndex1] != nodeDegree[nodeIndex2])
                    return nodeDegree[nodeIndex1] < nodeDegree[nodeIndex2];
                return nodeIndex1 < nodeIndex2;
                });

        bool addedSelfLoop = false;
        for (int i = 0; i < N; i++)
        {
            const int nodeIndex = nodeIndicesByDegree[i];
            if (!nodeHasSelfLoop[nodeIndex])
            {
                nodeHasSelfLoop[nodeIndex] = true;
                nodeDegree[nodeIndex]++;
                addedSelfLoop = true;
                cout << " added a self-loop to node: " << (nodeIndex + 1) << endl;
                break;
            }
        }
        if (!addedSelfLoop)
        {
            // Connect two lowest-degree vertices.
            nodeDegree[nodeIndicesByDegree[0]]++;
            nodeDegree[nodeIndicesByDegree[1]]++;
            cout << " added an edge between nodes: " << (nodeIndicesByDegree[0] + 1) << " and " << (nodeIndicesByDegree[1] + 1) << endl;
        }
        const int maxNodeDegree = *max_element(nodeDegree.begin(), nodeDegree.end());
        cout << "N: " << N << " edges placed: " << i << " maxNodeDegree: " << maxNodeDegree << endl;

    }
    const int maxNodeDegree = *max_element(nodeDegree.begin(), nodeDegree.end());
    return maxNodeDegree;
}

int findMinHighestVertexDegree(int64_t N, int64_t M)
{
    if (M == 0 && N == 1)
    {
        // Graph consisting of just the one node with no edges presumably
        // counts as connected.
        return 0;
    }
    if (M < N - 1)
    {
        // Can't make a connected graph.
        return -1;
    }
    if (M > (N * (N + 1)) / 2)
    {
        // More edges than will fit on a complete
        // graph on N vertices (plus one self-loop per vertex).
        // NB: N <= 10^6, so N * N + N will not overflow an int64_t.
        return -1;
    }
    if (M == N - 1)
    {
        // Just connect the vertices in a line.
        if (N == 2)
        {
            // A pair of vertices connected together.
            return 1;
        }
        else
        {
            // At least one of the vertices will have degree 2.
            return 2;
        }
    }
    if (M == N)
    {
        if (N >= 2)
        {
            // For N > 2, a line through all vertices, then a self-loop through a vertex with degree 1.  At least one will have degree 2.
            // None will have degree 3.
            // For N == 2, connect the pair and add a self-loop from one to itself.
            // Exactly one will have degree 2.
            return 2;
        }
        else if (N == 1)
        {
            // For N == 1, a loop from the vertex to itself: this vertex will have degree 1.
            return 1;
        }
    }
    if (M == N + 1)
    {
        // Line through all vertices, then self-loops through the two vertices that have degree 1 -
        // max degree then is two.
        assert(N >= 2);
        return 2;
    }
    if (M > N + 1 && M <= 2 * N)
    {
        // For N > 2, a cycle through all vertices, then a self-loop from each vertex to itself
        // until M is exhausted.  At least one self-loop will be required; this will have
        // degree 3.  Vertices without self-loops will have degree 2.
        if (N > 2)
        {
            return 3;
        }
        if (N == 2)
        {
            // M == 3 is the only possibility.
            assert(M == 3);
            // Line connecting them, plus self-loop for each vertex.
            return 2;

        }
    }
    // M > 2 * N.
    assert(M > 2 * N);
    // For M == 2 * N, we have a cycle through all vertice and a self-loop through
    // each vertex. The strategy from here on in is just to connect the pair of 
    // vertices which have the smallest degrees until we're done.
    // It can be shown that we can fast-forward through the process using the formulae below.
    const auto initialHighestVertexDegree = 4; // M == 2 * N + 1 has a vertex of degree 4, and it just gets worse from there!
    if ((N % 2) == 0)
    {
        const auto numCompleteLayerings = (M - (2 * N + 1)) / (N / 2);
        return numCompleteLayerings + initialHighestVertexDegree;
    }
    else
    {
        const auto numCompleteDoubleLayerings = (M - (2 * N + 1)) / N;
        const auto progressThroughLastLayering = (M - (2 * N + 1)) % N;
        return (initialHighestVertexDegree + 2 * numCompleteDoubleLayerings + ((progressThroughLastLayering >= (N - 1) / 2) ? 1 : 0));
    }
}

int main(int argc, char* argv[])
{
    // Not even going to attempt to prove this, but basically we just need to be "greedy"
    // when forming our graph and for each new edge that needs placing, just make the
    // choice that keeps the maximum node degree minimal - generally by either placing
    // a self-loop, if we have any left, or by connecting the two nodes of lowest degree.
    //
    // In more detail:
    //
    // For M < N - 1, there is no way to connect the all computers, so this will be -1
    // (well-known result about # edges in a connected tree on N nodes).
    // 
    // For M == N - 1, we must connect all N nodes in a "chain", with all but two nodes
    // having degree 2 (the two "loose ends" that both have degree 1).
    //
    // For adding the M == Nth edge, it seems intuitive that we'd connect the two "loose ends"
    // to make a cycle, with all nodes now having degree 2.  Let's go with that, for now (foreshadowing!)
    //
    // For M == N + 1, it seems we have no choice but to add a self-loop to some random edge (we're
    // being greedy, remember; adding an edge between two nodes would increase the degree of two
    // nodes instead of just one from a self-loop).  Since all vertices had degree 2, our
    // new max node degree, after adding a self-loop, is 3 ...
    //
    // ... and we've made a mistake: it's actually possible to get M = N + 1 with with max degree of 2:
    // connect all nodes with the first N - 1 edges; then add self-loops to each of the two "loose ends"
    // which had degree 1, leaving all nodes with degree 2.  I only included this mistake as it's one
    // I blundered into myself XD
    //
    // So actually, our algorithm so far looks like this:
    //
    //    For M up to N - 1 - just work towards chaining all N nodes together
    //    For M == N - add a self-loop to one of the two loose ends
    //    For M == N + 1 - add a self-loop to the remaining two loose end
    //
    // For the next N - 2 edges, we use one of our remaining N - 2 self-loops until we get to 
    // M == (N + 1) + (N - 2) == 2 * N - 1.
    //
    // At this point, every node has a self-loop, and all but two nodes have degree 3 - the two "loose
    // end" nodes from the chain, which still have degree 2.  Continuing our greediness, for edge
    // 2 * N we connect them, so every node now has degree 3.

    // TODO - explain adding remaining values in layers - different for odd N vs even N.  Use these
    // diagrams:

    //  <snip>
    //  N: 6 edges placed: 12 maxNodeDegree: 3 <-- M == 2 * N - all nodes have degree 3.
    //  N: 6 edges placed: 13 maxNodeDegree: 4 ┐
    //  N: 6 edges placed: 14 maxNodeDegree: 4 ├  N / 2 edges added - a "layer".
    //  N: 6 edges placed: 15 maxNodeDegree: 4 ┘
    //  N: 6 edges placed: 16 maxNodeDegree: 5 ┐
    //  N: 6 edges placed: 17 maxNodeDegree: 5 ├  N / 2 edges added - a "layer".
    //  N: 6 edges placed: 18 maxNodeDegree: 5 ┘
    //  N: 6 edges placed: 19 maxNodeDegree: 6 ┐
    //  N: 6 edges placed: 20 maxNodeDegree: 6 ├  N / 2 edges added - a "layer".
    //  N: 6 edges placed: 21 maxNodeDegree: 6 ┘
    //  <snip>

    //  <snip>
    //  N: 9 edges placed: 18 maxNodeDegree: 3 <-- M == 2 * N - all nodes have degree 3.
    //  N: 9 edges placed: 19 maxNodeDegree: 4 ┐
    //  N: 9 edges placed: 20 maxNodeDegree: 4 │
    //  N: 9 edges placed: 21 maxNodeDegree: 4 │
    //  N: 9 edges placed: 22 maxNodeDegree: 4 │  
    //  N: 9 edges placed: 23 maxNodeDegree: 5 ├  N edges added - a "double-layer"
    //  N: 9 edges placed: 24 maxNodeDegree: 5 │
    //  N: 9 edges placed: 25 maxNodeDegree: 5 │
    //  N: 9 edges placed: 26 maxNodeDegree: 5 │
    //  N: 9 edges placed: 27 maxNodeDegree: 5 ┘
    //  N: 9 edges placed: 28 maxNodeDegree: 6 ┐
    //  N: 9 edges placed: 29 maxNodeDegree: 6 │
    //  N: 9 edges placed: 30 maxNodeDegree: 6 │
    //  N: 9 edges placed: 31 maxNodeDegree: 6 │
    //  N: 9 edges placed: 32 maxNodeDegree: 7 ├  N edges added - a "double-layer"
    //  N: 9 edges placed: 33 maxNodeDegree: 7 │
    //  N: 9 edges placed: 34 maxNodeDegree: 7 │
    //  N: 9 edges placed: 35 maxNodeDegree: 7 │
    //  N: 9 edges placed: 36 maxNodeDegree: 7 ┘
    //  N: 9 edges placed: 37 maxNodeDegree: 8 ┐
    //  N: 9 edges placed: 38 maxNodeDegree: 8 │
    //  N: 9 edges placed: 39 maxNodeDegree: 8 │
    //  N: 9 edges placed: 40 maxNodeDegree: 8 …
    //  <snip>
    ios::sync_with_stdio(false);
    const auto T = read<int>();


    for (int t = 0; t < T; t++)
    {
        const auto N = read<int64_t>();
        const auto M = read<int64_t>();

        cout << findMinHighestVertexDegree(N, M) << endl;
    }

    assert(cin);
}
