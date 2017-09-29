// Simon St James (ssjgz) 2017-09-29 14:20
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    // Ouch - a hard one, this, I thought, and I'm quite surprised by the high completion rate (~44.05% at the time of writing).
    // So: we are only interested in the last digit of costs, which strongly hints that we should be using arithmetic modulo 10
    // throughout.   Let P = ei1, ei2, ... eik be a path (eij are edges; since there can be multiple edges between nodes,
    // we must express paths in terms of edges, not nodes.  Let C(eij) be the cost of traversing edge eij.  Let CM(P) be the cost of
    // the path P, modulo 10; then CM(P) = (sum j = 1...k {eij}) % 10.  
    //
    // Let ~eij denote the cost of traversing the edge eij backwards; from the problem statement, we have C(~eij) = 1000 - C(eij).
    // Let ~P be the reverse of P i.e. ~P = ~eik, ~eik-1, ... ~ei1; then CM(~P) = (sum j = 1...k {~eij}) % 10 = 
    // (k * 1000 + sum j = 1...k {-eij}) % 10 = (k * 1000 + -CM(P)) % 10 = -CM(P) % 10.  Thus, we see that traversing a path P
    // from x to y and then traversing the path ~P from y to x is a path with 0 cost!
    //
    // Anyway, the problem initially seems very hard: how can we possibly compute all pairs (x,y) with a path P from x to y with
    // CM(P) = d? It's slightly simpler than it looks.  For the time being, let's restrict ourselves to the case where the 
    // graph is connected; the case of multiple components poses no additional difficulties.
    //
    // Here is the trick: pick any node as a "root" node and call it A, say.  Define N(A, d) to be, for some digit d (0..9),
    // the set of nodes x for which there is a path P from A to x such that CM(P) = d.  I claim that, for a pair of nodes
    // x and y and digit d, there is a path Pxy with C(Pxy) = d if and only if there are dx and dy such that
    // x is in N(A, dx) and y is in N(A, dy) and dy - dx = d (mod 10, of course) i.e. that  there is a path of cost d from
    // x to y if and only if there is a path Py from A to y and a path Px from a to x with CM(Py) - CM(Px) = d.
    struct Junction;
    struct Road
    {
        Junction* junction;
        int cost = 0;
    };
    struct Junction
    {
        vector<Road> neighbours;
        int index = 0;
        bool reached = false;
    };

    int n, e;
    cin >> n >> e;

    vector<Junction> junctions(n);
    for (int i = 0; i < n; i++)
    {
        junctions[i].index = i;
    }

    for (int i = 0; i < e; i++)
    {
        int x, y, r;
        cin >> x >> y >> r;
        x--;
        y--;
        r %= 10;

        Road roadXY;
        roadXY.cost = r;
        roadXY.junction = &(junctions[y]);
        junctions[x].neighbours.push_back(roadXY);

        // Add reverse edge.
        Road roadYX;
        roadYX.junction = &(junctions[x]);
        roadYX.cost = (10 - r ) % 10;
        junctions[y].neighbours.push_back(roadYX);
    }

    int64_t numPairsWithCost[10] = {};

    // We use char as bool is *vastly* slower.
    // NB: it looks like we might have to re-initialise this for each component of the 
    // graph, but that would be O(N ^ 2) and it is in fact non-necessary: a given
    // Junction can not be in more than one component i.e. reachable from more than one
    // component root!
    vector<vector<char>> isJunctionIndexReachableFromRootWithCost(10, vector<char>(n));

    for (int componentRootIndex = 0; componentRootIndex < n; componentRootIndex++)
    {
        if (junctions[componentRootIndex].reached)
            continue;
        Junction* componentRoot = &(junctions[componentRootIndex]);

        int64_t numJunctionsReachableFromRootWithCost[10] = {};

        struct JunctionAndCost
        {
            Junction *junction = nullptr;
            int cost = 0;
        };

        JunctionAndCost rootJunctionAndCost;
        rootJunctionAndCost.junction = componentRoot;

        vector<JunctionAndCost> junctionsAndCostsToExplore = { rootJunctionAndCost };


        while (!junctionsAndCostsToExplore.empty())
        {
            vector<JunctionAndCost> nextJunctionsAndCostsToExplore;
            for (const auto& junctionAndCost : junctionsAndCostsToExplore)
            {
                auto junction = junctionAndCost.junction;
                const auto cost = junctionAndCost.cost;
                if (isJunctionIndexReachableFromRootWithCost[junctionAndCost.cost][junction->index])
                    continue;
                isJunctionIndexReachableFromRootWithCost[junctionAndCost.cost][junction->index] = true;
                junction->reached = true;
                numJunctionsReachableFromRootWithCost[cost]++;

                for (const auto& road : junction->neighbours)
                {
                    auto neighbourJunction = road.junction;
                    const auto costToReachNeighbour = (cost + road.cost) % 10;
                    if (!isJunctionIndexReachableFromRootWithCost[costToReachNeighbour][neighbourJunction->index])
                    {
                        JunctionAndCost neighbourJunctionAndCost;
                        neighbourJunctionAndCost.junction = neighbourJunction;
                        neighbourJunctionAndCost.cost = costToReachNeighbour;
                        nextJunctionsAndCostsToExplore.push_back(neighbourJunctionAndCost);
                    }
                }
            }
            junctionsAndCostsToExplore = nextJunctionsAndCostsToExplore;
        }

        vector<int> rootSelfLoopCosts;
        for (int cost = 0; cost <= 9; cost++)
        {
            if (isJunctionIndexReachableFromRootWithCost[cost][componentRoot->index])
            {
                rootSelfLoopCosts.push_back(cost);
            }
        }

        for (int cost = 0; cost <= 9; cost++)
        {
            vector<int> processedXCosts;
            int xCost = 0;
            int yCost = xCost + cost;
            const bool junctionsReachableWithXAndYCostsAreEqual = (find(rootSelfLoopCosts.begin(), rootSelfLoopCosts.end(), cost) != rootSelfLoopCosts.end());

            int64_t numPairs = 0;
            bool alreadyHandledEquivalent = false;
            while (xCost <= 9)
            {
                for (const auto processedXCost : processedXCosts)
                {
                    for (const auto rootJunctionSelfLoopCost : rootSelfLoopCosts)
                    {
                        if ((processedXCost + rootJunctionSelfLoopCost) % 10 == xCost)
                        {
                            // i.e. if we've already handled xCost = 1, and there is a root self-loop with cost 5, don't handle xCost = 6;
                            // we'll overcount pairs.
                            alreadyHandledEquivalent = true;
                        }
                    }
                }
                if (!alreadyHandledEquivalent)
                {
                    numPairs += numJunctionsReachableFromRootWithCost[yCost] * numJunctionsReachableFromRootWithCost[xCost];
                    // The product above will count pairs where the junctions are the same; subtract these.
                    // The overCount is the number of vertices that are reachable with both xCost and yCost.
                    const int64_t overCount = (junctionsReachableWithXAndYCostAreEqual ? numJunctionsReachableFromRootWithCost[xCost] : 0);
                    numPairs -= overCount;

                }
                processedXCosts.push_back(xCost);
                xCost++;
                yCost = (yCost + 1) % 10;
            }
            numPairsWithCost[cost] += numPairs;
        }
    }
    for (const auto numPairs : numPairsWithCost)
    {
        cout << numPairs << endl;
    }
}
